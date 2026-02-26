package main

import (
	"bufio"
	"bytes"
	"embed"
	"fmt"
	"image"
	_ "image/png"
	"io"
	"io/fs"
	"log"
	"math"
	"os"
	"path"
	"path/filepath"
	"strconv"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"

	"github.com/hajimehoshi/ebiten/v2/audio"
	"github.com/hajimehoshi/ebiten/v2/audio/wav"
	"github.com/hajimehoshi/ebiten/v2/inpututil"
)

type neko struct {
	waiting    bool
	x          float64
	y          float64
	distance   int
	count      int
	min        int
	max        int
	state      int
	sprite        string
	lastSprite    string
	lastDirection string
	img        *ebiten.Image

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
}

// loadConfig reads neko.ini from the same directory as the executable.
// Supported keys: speed, scale, quiet, mousepassthrough
func loadConfig() *Config {
	cfg := &Config{
		Speed: 2.0,
		Scale: 2.0,
	}

	exePath, err := os.Executable()
	if err != nil {
		return cfg
	}
	iniPath := filepath.Join(filepath.Dir(exePath), "neko.ini")

	f, err := os.Open(iniPath)
	if err != nil {
		return cfg
	}
	defer f.Close()

	scanner := bufio.NewScanner(f)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" || strings.HasPrefix(line, ";") || strings.HasPrefix(line, "#") {
			continue
		}
		parts := strings.SplitN(line, "=", 2)
		if len(parts) != 2 {
			continue
		}
		key := strings.ToLower(strings.TrimSpace(parts[0]))
		val := strings.TrimSpace(parts[1])
		switch key {
		case "speed":
			if v, err := strconv.ParseFloat(val, 64); err == nil {
				cfg.Speed = v
			}
		case "scale":
			if v, err := strconv.ParseFloat(val, 64); err == nil {
				cfg.Scale = v
			}
		case "quiet":
			cfg.Quiet = val == "true" || val == "1"
		case "mousepassthrough":
			cfg.MousePassthrough = val == "true" || val == "1"
		}
	}
	return cfg
}

const (
	width       = 32
	height      = 32
	sampleRate  = 44100
	soundVolume = 0.3
)

var (
	//go:embed assets/*
	f embed.FS
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

func absInt(v int) int {
	if v < 0 {
		return -v
	}
	return v
}

func (m *neko) Update() error {
	m.count++
	if m.state == 10 && m.count == m.min {
		m.playSound("idle3")
	}

	// Move the window to follow the cat position (no clamping — allows multi-monitor roaming).
	ebiten.SetWindowPosition(int(math.Round(m.x)), int(math.Round(m.y)))

	mx, my := ebiten.CursorPosition()
	x := mx - (width / 2)
	y := my - (height / 2)

	m.distance = absInt(x) + absInt(y)
	if m.distance < width || m.waiting {
		m.stayIdle()
		if inpututil.IsMouseButtonJustPressed(ebiten.MouseButtonLeft) {
			m.waiting = !m.waiting
		}
		return nil
	}

	if m.state >= 13 {
		m.playSound("awake")
	}
	m.catchCursor(x, y)
	return nil
}

func (m *neko) stayIdle() {
	// idle state
	switch m.state {
	case 0, 1, 2, 3:
		if m.state == 0 {
			m.state = 1
		}
		m.sprite = "awake"

	case 4, 5, 6:
		m.sprite = "scratch"

	case 7, 8, 9:
		m.sprite = "wash"

	case 10, 11, 12:
		m.min = 32
		m.max = 64
		m.sprite = "yawn"

	default:
		m.sprite = "sleep"
	}
}

func (m *neko) catchCursor(x, y int) {
	m.state = 0
	m.min = 8
	m.max = 16

	// get mouse direction
	r := math.Atan2(float64(y), float64(x))
	a := math.Mod((r/math.Pi*180)+360, 360) // Normalizing angle to [0, 360)

	// Hysteresis: if the angle is within 5° of any direction boundary,
	// keep the last direction to prevent flickering between animations.
	const hysteresis = 5.0
	boundaries := [8]float64{22.5, 67.5, 112.5, 157.5, 202.5, 247.5, 292.5, 337.5}
	for _, b := range boundaries {
		diff := math.Abs(a - b)
		if diff > 180 {
			diff = 360 - diff
		}
		if diff < hysteresis && m.lastDirection != "" {
			m.sprite = m.lastDirection
			// Still move in the last direction so the cat doesn't freeze
			switch m.lastDirection {
			case "up":
				m.y -= m.cfg.Speed
			case "upright":
				m.x += m.cfg.Speed / math.Sqrt2
				m.y -= m.cfg.Speed / math.Sqrt2
			case "right":
				m.x += m.cfg.Speed
			case "downright":
				m.x += m.cfg.Speed / math.Sqrt2
				m.y += m.cfg.Speed / math.Sqrt2
			case "down":
				m.y += m.cfg.Speed
			case "downleft":
				m.x -= m.cfg.Speed / math.Sqrt2
				m.y += m.cfg.Speed / math.Sqrt2
			case "left":
				m.x -= m.cfg.Speed
			case "upleft":
				m.x -= m.cfg.Speed / math.Sqrt2
				m.y -= m.cfg.Speed / math.Sqrt2
			}
			return
		}
	}

	switch {
	case a <= 292.5 && a > 247.5: // up
		m.y -= m.cfg.Speed
		m.sprite = "up"
	case a <= 337.5 && a > 292.5: // up right
		m.x += m.cfg.Speed / math.Sqrt2
		m.y -= m.cfg.Speed / math.Sqrt2
		m.sprite = "upright"
	case a <= 22.5 || a > 337.5: // right
		m.x += m.cfg.Speed
		m.sprite = "right"
	case a <= 67.5 && a > 22.5: // down right
		m.x += m.cfg.Speed / math.Sqrt2
		m.y += m.cfg.Speed / math.Sqrt2
		m.sprite = "downright"
	case a <= 112.5 && a > 67.5: // down
		m.y += m.cfg.Speed
		m.sprite = "down"
	case a <= 157.5 && a > 112.5: // down left
		m.x -= m.cfg.Speed / math.Sqrt2
		m.y += m.cfg.Speed / math.Sqrt2
		m.sprite = "downleft"
	case a <= 202.5 && a > 157.5: // left
		m.x -= m.cfg.Speed
		m.sprite = "left"
	case a <= 247.5 && a > 202.5: // up left
		m.x -= m.cfg.Speed / math.Sqrt2
		m.y -= m.cfg.Speed / math.Sqrt2
		m.sprite = "upleft"
	}
	m.lastDirection = m.sprite
}

func (m *neko) Draw(screen *ebiten.Image) {
	var sprite string
	switch {
	case m.sprite == "awake":
		sprite = m.sprite
	case m.count < m.min:
		sprite = m.sprite + "1"
	default:
		sprite = m.sprite + "2"
	}

	m.img = m.sprites[sprite]

	if m.count > m.max {
		m.count = 0

		if m.state > 0 {
			m.state++
			switch m.state {
			case 13:
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

func loadAssets(assetsFS fs.FS, sampleRate int) (map[string]*ebiten.Image, map[string][]byte, error) {
	sprites := make(map[string]*ebiten.Image)
	sounds := make(map[string][]byte)

	entries, err := fs.ReadDir(assetsFS, "assets")
	if err != nil {
		return nil, nil, fmt.Errorf("read assets directory: %w", err)
	}

	for _, entry := range entries {
		if entry.IsDir() {
			continue
		}

		assetPath := path.Join("assets", entry.Name())
		data, err := fs.ReadFile(assetsFS, assetPath)
		if err != nil {
			return nil, nil, fmt.Errorf("read %q: %w", assetPath, err)
		}

		name := strings.TrimSuffix(entry.Name(), path.Ext(entry.Name()))
		switch path.Ext(entry.Name()) {
		case ".png":
			img, _, err := image.Decode(bytes.NewReader(data))
			if err != nil {
				return nil, nil, fmt.Errorf("decode sprite %q: %w", entry.Name(), err)
			}
			sprites[name] = ebiten.NewImageFromImage(img)

		case ".wav":
			stream, err := wav.DecodeWithSampleRate(sampleRate, bytes.NewReader(data))
			if err != nil {
				return nil, nil, fmt.Errorf("decode sound %q: %w", entry.Name(), err)
			}
			soundData, err := io.ReadAll(stream)
			if err != nil {
				return nil, nil, fmt.Errorf("read sound %q: %w", entry.Name(), err)
			}
			sounds[name] = soundData
		}
	}

	return sprites, sounds, nil
}

func main() {
	cfg := loadConfig()

	sprites, sounds, err := loadAssets(f, sampleRate)
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
