package main

import (
	"bytes"
	"embed"
	"flag"
	"fmt"
	"image"
	_ "image/gif"
	_ "image/png"
	"io"
	"io/fs"
	"log"
	"math"
	"os"
	"path"
	"path/filepath"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"

	"github.com/hajimehoshi/ebiten/v2/audio"
	"github.com/hajimehoshi/ebiten/v2/audio/wav"
	"github.com/hajimehoshi/ebiten/v2/inpututil"

	"github.com/crgimenes/filo"
)

type neko struct {
	waiting    bool
	x          float64
	y          float64
	count      int
	min        int
	max        int
	state      int
	sprite     string
	lastSprite string
	img        *ebiten.Image
	monitor    *ebiten.MonitorType

	cfg           *Config
	sprites       map[string]*ebiten.Image
	sounds        map[string][]byte
	audioContext  *audio.Context
	currentPlayer *audio.Player
}

type Config struct {
	Speed            float64
	Scale            float64
	Quiet            bool
	MousePassthrough bool
	SpriteSheet      string
}

type movementDirection int

const (
	directionRight movementDirection = iota
	directionDownRight
	directionDown
	directionDownLeft
	directionLeft
	directionUpLeft
	directionUp
	directionUpRight
	directionCount
)

const (
	width                = 32
	height               = 32
	sampleRate           = 44100
	soundVolume          = 0.3
	directionSectorAngle = 45.0
	directionHalfSector  = directionSectorAngle / 2
	arrivalSlowdownRange = width * 3
	// Disabled by default: a cat should keep its speed while pouncing.
	arrivalSlowdownEnabled = false
	// A small overlap keeps the animation stable without changing movement.
	directionHysteresis = 3.0
)

// Idle animation timeline. While the cat is idle, m.state advances through
// these phases on each animation cycle; a phase spans every tick from its
// threshold up to the next one. stateMoving means the cat is chasing the
// cursor rather than idling.
const (
	stateMoving     = 0
	stateAwakeStart = 1
	stateScratch    = 4
	stateWash       = 7
	stateYawn       = 10
	stateSleep      = 13
)

// spriteCell is the column and row of a 32px tile inside a sprite sheet.
type spriteCell struct {
	col int
	row int
}

// The sprite sheet is an 8x4 grid of 32px tiles (256x128), following the
// layout used by adryd325/oneko.js (https://github.com/adryd325/oneko.js).
// Sharing that layout lets third-party oneko sheets be loaded as skins.
const (
	sheetColumns = 8
	sheetRows    = 4
)

// spriteSheetLayout maps each frame Neko draws to its cell in the sheet. The
// eight movement directions and the sleeping frames sit on their canonical
// oneko cells so foreign oneko sheets animate correctly; the remaining idle
// frames reuse the nearest oneko state, since Neko has a few states oneko does
// not. Cells absent from this map are unused and may be left transparent.
var spriteSheetLayout = map[string]spriteCell{
	// Movement directions, two frames each (oneko N, NE, E, SE, S, SW, W, NW).
	"up1":        {1, 2},
	"up2":        {1, 3},
	"upright1":   {0, 2},
	"upright2":   {0, 3},
	"right1":     {3, 0},
	"right2":     {3, 1},
	"downright1": {5, 1},
	"downright2": {5, 2},
	"down1":      {6, 3},
	"down2":      {7, 2},
	"downleft1":  {5, 3},
	"downleft2":  {6, 1},
	"left1":      {4, 2},
	"left2":      {4, 3},
	"upleft1":    {1, 0},
	"upleft2":    {1, 1},

	// Idle states.
	"awake":    {7, 3}, // oneko "alert"
	"sleep1":   {2, 0}, // oneko "sleeping"
	"sleep2":   {2, 1},
	"yawn1":    {3, 2}, // oneko "tired"
	"yawn2":    {3, 3}, // oneko "idle" (oneko has no second tired frame)
	"scratch1": {5, 0}, // oneko "scratchSelf"
	"scratch2": {6, 0},
	"wash":     {7, 0}, // oneko "scratchSelf" (third frame); single frame, since
	//                     oneko grooming is the closest match to washing and
	//                     only three scratchSelf cells exist.
}

var (
	// Only the generated sprite sheet and the sounds ship in the binary. The
	// individual source sprites under sprites/ are build-time inputs for the
	// generator (see gensheet_test.go), not runtime assets.
	//go:embed assets/neko.png
	//go:embed assets/*.wav
	embeddedFS embed.FS
)

func (m *neko) Layout(outsideWidth, outsideHeight int) (int, int) {
	return width, height
}

func (m *neko) playSound(soundName string) {
	if m.cfg.Quiet {
		return
	}
	sound, ok := m.sounds[soundName]
	if !ok {
		return
	}
	if m.currentPlayer != nil {
		_ = m.currentPlayer.Close()
		m.currentPlayer = nil
	}
	m.currentPlayer = m.audioContext.NewPlayerFromBytes(sound)
	m.currentPlayer.SetVolume(soundVolume)
	m.currentPlayer.Play()
}

func angularDistance(a, b float64) float64 {
	distance := math.Abs(a - b)
	return min(distance, 360-distance)
}

func movementVector(x, y, speed float64, slowdown bool) (float64, float64) {
	distance := math.Hypot(x, y)
	if distance == 0 || speed <= 0 {
		return 0, 0
	}

	if slowdown && distance < arrivalSlowdownRange {
		speed *= distance / arrivalSlowdownRange
	}
	speed = min(speed, distance)

	scale := speed / distance
	return x * scale, y * scale
}

func parseMovementDirection(sprite string) (movementDirection, bool) {
	switch sprite {
	case "right":
		return directionRight, true
	case "downright":
		return directionDownRight, true
	case "down":
		return directionDown, true
	case "downleft":
		return directionDownLeft, true
	case "left":
		return directionLeft, true
	case "upleft":
		return directionUpLeft, true
	case "up":
		return directionUp, true
	case "upright":
		return directionUpRight, true
	default:
		return 0, false
	}
}

func (d movementDirection) sprite() string {
	switch d {
	case directionRight:
		return "right"
	case directionDownRight:
		return "downright"
	case directionDown:
		return "down"
	case directionDownLeft:
		return "downleft"
	case directionLeft:
		return "left"
	case directionUpLeft:
		return "upleft"
	case directionUp:
		return "up"
	case directionUpRight:
		return "upright"
	default:
		return ""
	}
}

func spriteDirection(angle float64, previous string) string {
	if direction, ok := parseMovementDirection(previous); ok {
		center := float64(direction) * directionSectorAngle
		if angularDistance(angle, center) <= directionHalfSector+directionHysteresis {
			return previous
		}
	}

	index := int(math.Ceil((angle - directionHalfSector) / directionSectorAngle))
	index = (index + int(directionCount)) % int(directionCount)
	return movementDirection(index).sprite()
}

func (m *neko) syncMonitor(
	current *ebiten.MonitorType,
	windowPosition func() (int, int),
) {
	if current == nil {
		return
	}
	if m.monitor == nil {
		m.monitor = current
		return
	}
	if current == m.monitor {
		return
	}

	x, y := windowPosition()
	m.x = float64(x)
	m.y = float64(y)
	m.monitor = current
}

func (m *neko) Update() error {
	m.count++
	if m.state == stateYawn && m.count == m.min {
		m.playSound("idle3")
	}

	// Window coordinates are relative to the current monitor. Rebase the
	// internal position when the window crosses into another monitor.
	m.syncMonitor(ebiten.Monitor(), ebiten.WindowPosition)
	ebiten.SetWindowPosition(int(math.Round(m.x)), int(math.Round(m.y)))

	mx, my := ebiten.CursorPositionF()
	x := mx - (width / 2)
	y := my - (height / 2)

	distance := math.Abs(x) + math.Abs(y)
	if distance < width || m.waiting {
		m.stayIdle()
		if inpututil.IsMouseButtonJustPressed(ebiten.MouseButtonLeft) {
			m.waiting = !m.waiting
		}
		return nil
	}

	if m.state >= stateSleep {
		m.playSound("awake")
	}
	m.catchCursor(x, y)
	return nil
}

func (m *neko) stayIdle() {
	switch {
	case m.state < stateScratch:
		if m.state == stateMoving {
			m.state = stateAwakeStart
		}
		m.sprite = "awake"

	case m.state < stateWash:
		m.sprite = "scratch"

	case m.state < stateYawn:
		m.sprite = "wash"

	case m.state < stateSleep:
		m.min = 32
		m.max = 64
		m.sprite = "yawn"

	default:
		m.sprite = "sleep"
	}
}

func (m *neko) catchCursor(x, y float64) {
	m.state = 0
	m.min = 8
	m.max = 16

	// get mouse direction
	r := math.Atan2(y, x)
	a := math.Mod((r/math.Pi*180)+360, 360) // Normazing angle to [0, 360)
	m.sprite = spriteDirection(a, m.sprite)

	dx, dy := movementVector(x, y, m.cfg.Speed, arrivalSlowdownEnabled)
	m.x += dx
	m.y += dy
}

func (m *neko) Draw(screen *ebiten.Image) {
	var sprite string
	switch {
	case m.sprite == "awake" || m.sprite == "wash":
		sprite = m.sprite
	case m.count < m.min:
		sprite = m.sprite + "1"
	default:
		sprite = m.sprite + "2"
	}

	m.img = m.sprites[sprite]

	if m.count > m.max {
		m.count = 0

		if m.state > stateMoving {
			m.state++
			switch m.state {
			case stateSleep:
				m.playSound("sleep")
			}
		}
	}

	if m.lastSprite == sprite {
		return
	}

	m.lastSprite = sprite

	screen.Clear()

	screen.DrawImage(m.img, nil)
}

// subImager is implemented by the standard library image types that back a
// decoded PNG, allowing zero-copy crops of a sprite sheet.
type subImager interface {
	SubImage(r image.Rectangle) image.Image
}

// loadSpriteSheet returns the raw PNG bytes of the sprite sheet to use: the
// user-provided file when sheetPath is set, otherwise the embedded default.
func loadSpriteSheet(sheetPath string) ([]byte, error) {
	if sheetPath != "" {
		data, err := os.ReadFile(filepath.Clean(sheetPath))
		if err != nil {
			return nil, fmt.Errorf("read sprite sheet %q: %w", sheetPath, err)
		}
		return data, nil
	}
	return embeddedFS.ReadFile("assets/neko.png")
}

// loadSprites decodes an oneko-layout sprite sheet and slices it into the
// individual frames Neko draws. The crops share the decoded sheet's pixels, so
// slicing is effectively free at load time.
func loadSprites(sheet []byte) (map[string]*ebiten.Image, error) {
	img, _, err := image.Decode(bytes.NewReader(sheet))
	if err != nil {
		return nil, fmt.Errorf("decode sprite sheet: %w", err)
	}

	cropper, ok := img.(subImager)
	if !ok {
		return nil, fmt.Errorf("sprite sheet type %T does not support cropping", img)
	}

	bounds := img.Bounds()
	if bounds.Dx() < sheetColumns*width || bounds.Dy() < sheetRows*height {
		return nil, fmt.Errorf(
			"sprite sheet is %dx%d, need at least %dx%d",
			bounds.Dx(), bounds.Dy(), sheetColumns*width, sheetRows*height,
		)
	}

	sprites := make(map[string]*ebiten.Image, len(spriteSheetLayout))
	for name, cell := range spriteSheetLayout {
		x0 := bounds.Min.X + cell.col*width
		y0 := bounds.Min.Y + cell.row*height
		rect := image.Rect(x0, y0, x0+width, y0+height)
		sprites[name] = ebiten.NewImageFromImage(cropper.SubImage(rect))
	}

	return sprites, nil
}

// loadSounds decodes the embedded .wav assets into raw PCM byte slices keyed by
// file name without extension.
func loadSounds(assetsFS fs.FS, sampleRate int) (map[string][]byte, error) {
	sounds := make(map[string][]byte)

	entries, err := fs.ReadDir(assetsFS, "assets")
	if err != nil {
		return nil, fmt.Errorf("read assets directory: %w", err)
	}

	for _, entry := range entries {
		if entry.IsDir() || path.Ext(entry.Name()) != ".wav" {
			continue
		}

		assetPath := path.Join("assets", entry.Name())
		data, err := fs.ReadFile(assetsFS, assetPath)
		if err != nil {
			return nil, fmt.Errorf("read %q: %w", assetPath, err)
		}

		stream, err := wav.DecodeWithSampleRate(sampleRate, bytes.NewReader(data))
		if err != nil {
			return nil, fmt.Errorf("decode sound %q: %w", entry.Name(), err)
		}
		soundData, err := io.ReadAll(stream)
		if err != nil {
			return nil, fmt.Errorf("read sound %q: %w", entry.Name(), err)
		}

		name := strings.TrimSuffix(entry.Name(), path.Ext(entry.Name()))
		sounds[name] = soundData
	}

	return sounds, nil
}

// configPath returns the path to the Filo config file. A local "neko_init.filo"
// in the current directory takes precedence; otherwise the XDG config location
// "$XDG_CONFIG_HOME/neko/init.filo" (defaulting to ~/.config/neko/init.filo) is
// used.
func configPath() string {
	const local = "neko_init.filo"
	if fileExists(local) {
		return local
	}

	configHome := os.Getenv("XDG_CONFIG_HOME")
	if configHome == "" {
		home, err := os.UserHomeDir()
		if err != nil {
			return local
		}
		configHome = filepath.Join(home, ".config")
	}
	return filepath.Join(configHome, "neko", "init.filo")
}

func fileExists(name string) bool {
	info, err := os.Stat(name)
	return err == nil && !info.IsDir()
}

// loadConfig builds the configuration from defaults, overriding them with any
// values set in the Filo config file (if present).
func loadConfig() *Config {
	cfg := &Config{
		Speed:            2.0,
		Scale:            2.0,
		Quiet:            false,
		MousePassthrough: false,
		SpriteSheet:      "",
	}

	name := configPath()
	if !fileExists(name) {
		return cfg
	}

	f := filo.New()
	defer f.Close()

	f.SetGlobal("Speed", cfg.Speed)
	f.SetGlobal("Scale", cfg.Scale)
	f.SetGlobal("Quiet", cfg.Quiet)
	f.SetGlobal("MousePassthrough", cfg.MousePassthrough)
	f.SetGlobal("SpriteSheet", cfg.SpriteSheet)

	b, err := os.ReadFile(filepath.Clean(name))
	if err != nil {
		log.Fatal(err)
	}
	if err := f.DoString(string(b)); err != nil {
		log.Fatal(err)
	}

	cfg.Speed = f.MustGetNumber("Speed")
	cfg.Scale = f.MustGetNumber("Scale")
	cfg.Quiet = f.MustGetBool("Quiet")
	cfg.MousePassthrough = f.MustGetBool("MousePassthrough")
	cfg.SpriteSheet = f.MustGetString("SpriteSheet")

	return cfg
}

func main() {
	cfg := loadConfig()

	// Command-line flags override the Filo config file. Their defaults are the
	// values loaded above, so a flag only takes effect when explicitly passed.
	flag.Float64Var(&cfg.Speed, "speed", cfg.Speed, "The speed of the cat.")
	flag.Float64Var(&cfg.Scale, "scale", cfg.Scale, "The scale of the cat on the screen.")
	flag.BoolVar(&cfg.Quiet, "quiet", cfg.Quiet, "Disable sound.")
	flag.BoolVar(&cfg.MousePassthrough, "mousepassthrough", cfg.MousePassthrough, "Enable mouse passthrough.")
	flag.StringVar(&cfg.SpriteSheet, "spritesheet", cfg.SpriteSheet, "Path to a custom oneko-layout sprite sheet (PNG).")
	flag.Parse()

	sheet, err := loadSpriteSheet(cfg.SpriteSheet)
	if err != nil {
		log.Fatal(err)
	}
	sprites, err := loadSprites(sheet)
	if err != nil {
		log.Fatal(err)
	}
	sounds, err := loadSounds(embeddedFS, sampleRate)
	if err != nil {
		log.Fatal(err)
	}

	audioContext := audio.NewContext(sampleRate)

	// Workaround: for some reason playing the first sound can incur significant delay.
	// So let's do this at the start.
	audioContext.NewPlayerFromBytes([]byte{}).Play()

	monitorWidth, monitorHeight := ebiten.Monitor().Size()

	n := &neko{
		x:            float64(monitorWidth / 2),
		y:            float64(monitorHeight / 2),
		min:          8,
		max:          16,
		cfg:          cfg,
		sprites:      sprites,
		sounds:       sounds,
		audioContext: audioContext,
	}

	ebiten.SetRunnableOnUnfocused(true)
	ebiten.SetScreenClearedEveryFrame(false)
	ebiten.SetTPS(50)
	ebiten.SetVsyncEnabled(true)
	ebiten.SetWindowDecorated(false)
	ebiten.SetWindowFloating(true)
	ebiten.SetWindowMousePassthrough(cfg.MousePassthrough)
	ebiten.SetWindowSize(int(float64(width)*cfg.Scale), int(float64(height)*cfg.Scale))
	ebiten.SetWindowTitle("Neko")

	err = ebiten.RunGameWithOptions(n, &ebiten.RunGameOptions{
		InitUnfocused:     true,
		ScreenTransparent: true,
		SkipTaskbar:       true,
		X11ClassName:      "Neko",
		X11InstanceName:   "Neko",
	})
	if err != nil {
		log.Fatal(err)
	}
}
