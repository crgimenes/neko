package main

import (
	"bytes"
	"embed"
	"image"
	_ "image/png"
	"io/fs"
	"log"
	"math"
	"path/filepath"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"

	"crg.eti.br/go/config"
	_ "crg.eti.br/go/config/ini"
)

type neko struct {
	x          int
	y          int
	distance   int
	count      int
	min        int
	max        int
	state      int
	sprite     string
	lastSprite string
	img        *ebiten.Image
}

type Config struct {
	Speed int     `ini:"speed" cfg:"speed" cfgDefault:"2" cfgHelper:"The speed of the cat."`
	Scale float64 `ini:"scale" cfg:"scale" cfgDefault:"2.0" cfgHelper:"The scale of the cat."`
}

var (
	mSprite map[string]*ebiten.Image

	//go:embed assets/*.png
	f embed.FS

	width  = 32
	height = 32

	cfg = &Config{}
)

func (m *neko) Layout(outsideWidth, outsideHeight int) (int, int) {
	return width, height
}

func (m *neko) Update() error {
	mx, my := ebiten.CursorPosition()

	m.count++

	// sw, sh := ebiten.ScreenSizeInFullscreen()

	// set the window position
	ebiten.SetWindowPosition(m.x, m.y)

	x := mx - (height / 2)
	y := my - (width / 2)

	// get distance from sprite to mouse
	dy, dx := y, x
	if dy < 0 {
		dy *= -1
	}

	if dx < 0 {
		dx *= -1
	}

	m.distance = dx + dy
	if m.distance < width {
		// idle state
		switch m.state {
		case 0:
			m.state = 1
			fallthrough

		case 1, 2, 3:
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

		return nil
	}

	m.state = 0
	m.min = 8
	m.max = 16

	tr := 0.0
	// get mouse direction
	r := math.Atan2(float64(y), float64(x))
	if r <= 0 {
		tr = 360
	}

	a := (r / math.Pi * 180) + tr

	switch {
	case a <= 292.5 && a > 247.5:
		m.y -= cfg.Speed
	case a <= 337.5 && a > 292.5:
		m.x += cfg.Speed
		m.y -= cfg.Speed
	case a <= 22.5 || a > 337.5:
		m.x += cfg.Speed
	case a <= 67.5 && a > 22.5:
		m.x += cfg.Speed
		m.y += cfg.Speed
	case a <= 112.5 && a > 67.5:
		m.y += cfg.Speed
	case a <= 157.5 && a > 112.5:
		m.x -= cfg.Speed
		m.y += cfg.Speed
	case a <= 202.5 && a > 157.5:
		m.x -= cfg.Speed
	case a <= 247.5 && a > 202.5:
		m.x -= cfg.Speed
		m.y -= cfg.Speed
	}

	switch {
	case a < 292 && a > 247:
		m.sprite = "up"
	case a < 337 && a > 292:
		m.sprite = "upright"
	case a < 22 || a > 337:
		m.sprite = "right"
	case a < 67 && a > 22:
		m.sprite = "downright"
	case a < 112 && a > 67:
		m.sprite = "down"
	case a < 157 && a > 112:
		m.sprite = "downleft"
	case a < 202 && a > 157:
		m.sprite = "left"
	case a < 247 && a > 202:
		m.sprite = "upleft"
	}

	return nil
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

	m.img = mSprite[sprite]

	if m.count > m.max {
		m.count = 0

		if m.state > 0 {
			m.state++
		}
	}

	if m.lastSprite == sprite {
		return
	}

	m.lastSprite = sprite

	screen.Clear()

	op := &ebiten.DrawImageOptions{}
	op.GeoM.Scale(cfg.Scale, cfg.Scale)
	screen.DrawImage(m.img, op)
}

func main() {
	config.PrefixEnv = "NEKO"
	config.File = "neko.ini"
	config.Parse(cfg)

	width *= int(cfg.Scale)
	height *= int(cfg.Scale)

	mSprite = make(map[string]*ebiten.Image)

	a, _ := fs.ReadDir(f, "assets")
	for _, v := range a {
		data, _ := f.ReadFile("assets/" + v.Name())

		img, _, err := image.Decode(bytes.NewReader(data))
		if err != nil {
			log.Fatal(err)
		}

		name := strings.TrimSuffix(v.Name(), filepath.Ext(v.Name()))
		mSprite[name] = ebiten.NewImageFromImage(img)
	}

	sw, sh := ebiten.ScreenSizeInFullscreen()
	n := &neko{
		x:   sw / 2,
		y:   sh / 2,
		min: 8,
		max: 16,
	}

	ebiten.SetWindowDecorated(false)
	ebiten.SetWindowFloating(true)
	ebiten.SetWindowSize(width, height)
	ebiten.SetTPS(50)
	ebiten.SetScreenClearedEveryFrame(false)
	ebiten.SetVsyncEnabled(true)

	err := ebiten.RunGameWithOptions(n, &ebiten.RunGameOptions{
		ScreenTransparent: true,
	})
	if err != nil {
		log.Fatal(err)
	}
}
