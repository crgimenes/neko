package main

import (
	"bytes"
	"embed"
	_ "embed"
	"image"
	_ "image/png"
	"io/fs"
	"log"
	"math"
	"math/rand"
	"path/filepath"
	"strings"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
)

const (
	scale  = 2.0
	width  = 32 * scale
	height = 32 * scale
)

var (
	mSprite map[string]*ebiten.Image

	//go:embed assets/*.png
	f embed.FS
)

type neko struct {
	x        int
	y        int
	distance int
	count    int
	sprite   string
}

func (m *neko) Layout(outsideWidth, outsideHeight int) (int, int) {
	return width, height
}

func (m *neko) Update() error {
	mx, my := ebiten.CursorPosition()

	m.count++

	//sw, sh := ebiten.ScreenSizeInFullscreen()

	// set the window position
	ebiten.SetWindowPosition(m.x, m.y)

	x := mx - (height / 2)
	y := my - (width / 2)

	// get distance from sprite to mouse
	dy, dx := y, x
	if dy < 0 {
		dy = dy * (-1)
	}
	if dx < 0 {
		dx = dx * (-1)
	}
	m.distance = dx + dy

	if m.distance < width {
		// idle state
		m.sprite = "wash"
		return nil
	}

	// get mouse direction
	r := math.Atan2(float64(y), float64(x))
	tr := 0.0
	if r <= 0 {
		tr = 360
	}
	a := (r / math.Pi * 180) + tr

	switch {
	case a <= 292.5 && a > 247.5:
		m.y--
	case a <= 337.5 && a > 292.5:
		m.x++
		m.y--
	case a <= 22.5 || a > 337.5:
		m.x++
	case a <= 67.5 && a > 22.5:
		m.x++
		m.y++
	case a <= 112.5 && a > 67.5:
		m.y++
	case a <= 157.5 && a > 112.5:
		m.x--
		m.y++
	case a <= 202.5 && a > 157.5:
		m.x--
	case a <= 247.5 && a > 202.5:
		m.x--
		m.y--
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
	img := mSprite["up1"]
	switch {
	case m.count < 8:
		img = mSprite[m.sprite+"1"]
	default:
		img = mSprite[m.sprite+"2"]
	}
	if m.count > 16 {
		m.count = 0
	}

	op := &ebiten.DrawImageOptions{}
	op.GeoM.Scale(scale, scale)
	screen.DrawImage(img, op)
}

func main() {
	rand.Seed(time.Now().UnixNano())
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

		//fmt.Printf("%q\n", name)
	}

	sw, sh := ebiten.ScreenSizeInFullscreen()
	n := &neko{
		x: sw / 2,
		y: sh / 2,
	}

	ebiten.SetScreenTransparent(true)
	ebiten.SetWindowDecorated(false)
	ebiten.SetWindowFloating(true)
	ebiten.SetWindowSize(width, height)
	err := ebiten.RunGame(n)
	if err != nil {
		log.Fatal(err)
	}
}
