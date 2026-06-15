package main

import (
	"image"
	"image/draw"
	"image/png"
	"os"
	"path/filepath"
	"testing"
)

// TestGenerateSpriteSheet composes the individual sprite PNGs in assets/ into a
// single oneko-layout sheet at assets/neko.png, using spriteSheetLayout as the
// single source of truth for cell placement. Cells not present in the layout
// are left transparent. It is a generator rather than a check, so it only runs
// when GENSHEET is set:
//
//	GENSHEET=1 go test -run TestGenerateSpriteSheet
func TestGenerateSpriteSheet(t *testing.T) {
	if os.Getenv("GENSHEET") == "" {
		t.Skip("set GENSHEET=1 to regenerate assets/neko.png")
	}

	sheet := image.NewNRGBA(image.Rect(0, 0, sheetColumns*width, sheetRows*height))

	for name, cell := range spriteSheetLayout {
		// "wash" is a single frame drawn from the first wash sprite.
		file := name + ".png"
		if name == "wash" {
			file = "wash1.png"
		}
		src := filepath.Join("sprites", file)
		f, err := os.Open(src)
		if err != nil {
			t.Fatalf("open %s: %v", src, err)
		}
		img, err := png.Decode(f)
		f.Close()
		if err != nil {
			t.Fatalf("decode %s: %v", src, err)
		}

		dst := image.Rect(
			cell.col*width,
			cell.row*height,
			cell.col*width+width,
			cell.row*height+height,
		)
		draw.Draw(sheet, dst, img, img.Bounds().Min, draw.Src)
	}

	out := filepath.Join("assets", "neko.png")
	f, err := os.Create(out)
	if err != nil {
		t.Fatalf("create %s: %v", out, err)
	}
	defer f.Close()
	if err := png.Encode(f, sheet); err != nil {
		t.Fatalf("encode %s: %v", out, err)
	}

	t.Logf("wrote %s (%d sprites)", out, len(spriteSheetLayout))
}
