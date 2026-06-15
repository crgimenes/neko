package main

import (
	"math"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
)

func TestMovementVector(t *testing.T) {
	t.Parallel()

	tests := []struct {
		name      string
		x         float64
		y         float64
		speed     float64
		slowdown  bool
		wantDX    float64
		wantDY    float64
		wantSpeed float64
	}{
		{
			name:      "horizontal",
			x:         100,
			speed:     2,
			wantDX:    2,
			wantSpeed: 2,
		},
		{
			name:      "vertical",
			y:         -100,
			speed:     2,
			wantDY:    -2,
			wantSpeed: 2,
		},
		{
			name:      "diagonal",
			x:         60,
			y:         80,
			speed:     2,
			wantDX:    1.2,
			wantDY:    1.6,
			wantSpeed: 2,
		},
		{
			name:      "slow arrival",
			x:         48,
			speed:     2,
			slowdown:  true,
			wantDX:    1,
			wantSpeed: 1,
		},
		{
			name:      "slow arrival disabled",
			x:         48,
			speed:     2,
			wantDX:    2,
			wantSpeed: 2,
		},
		{
			name:      "limit final step",
			x:         0.5,
			speed:     200,
			wantDX:    0.5,
			wantSpeed: 0.5,
		},
		{
			name:  "zero distance",
			speed: 2,
		},
		{
			name:  "non-positive speed",
			x:     100,
			speed: -1,
		},
	}

	for _, test := range tests {
		t.Run(test.name, func(t *testing.T) {
			t.Parallel()

			dx, dy := movementVector(test.x, test.y, test.speed, test.slowdown)
			if !almostEqual(dx, test.wantDX) {
				t.Errorf("dx = %v, want %v", dx, test.wantDX)
			}
			if !almostEqual(dy, test.wantDY) {
				t.Errorf("dy = %v, want %v", dy, test.wantDY)
			}
			if got := math.Hypot(dx, dy); !almostEqual(got, test.wantSpeed) {
				t.Errorf("speed = %v, want %v", got, test.wantSpeed)
			}
		})
	}
}

func almostEqual(a, b float64) bool {
	const tolerance = 1e-12

	return math.Abs(a-b) <= tolerance
}

func TestSyncMonitor(t *testing.T) {
	t.Parallel()

	first := new(ebiten.MonitorType)
	second := new(ebiten.MonitorType)
	n := &neko{x: 100, y: 200}

	windowPositionCalls := 0
	windowPosition := func() (int, int) {
		windowPositionCalls++
		return 20, 30
	}

	n.syncMonitor(first, windowPosition)
	if n.monitor != first {
		t.Fatal("initial monitor was not recorded")
	}
	if n.x != 100 || n.y != 200 {
		t.Fatalf("initial position = (%v, %v), want (100, 200)", n.x, n.y)
	}

	n.syncMonitor(first, windowPosition)
	if windowPositionCalls != 0 {
		t.Fatalf("WindowPosition called %d times without a monitor change", windowPositionCalls)
	}

	n.syncMonitor(second, windowPosition)
	if n.monitor != second {
		t.Fatal("new monitor was not recorded")
	}
	if n.x != 20 || n.y != 30 {
		t.Fatalf("rebased position = (%v, %v), want (20, 30)", n.x, n.y)
	}
	if windowPositionCalls != 1 {
		t.Fatalf("WindowPosition called %d times, want 1", windowPositionCalls)
	}
}

func TestSpriteDirectionWithoutHistory(t *testing.T) {
	t.Parallel()

	tests := []struct {
		name  string
		angle float64
		want  string
	}{
		{name: "right upper boundary", angle: 22.5, want: "right"},
		{name: "downright lower boundary", angle: 22.5001, want: "downright"},
		{name: "downright upper boundary", angle: 67.5, want: "downright"},
		{name: "down lower boundary", angle: 67.5001, want: "down"},
		{name: "upright upper boundary", angle: 337.5, want: "upright"},
		{name: "right lower boundary", angle: 337.5001, want: "right"},
	}

	for _, test := range tests {
		t.Run(test.name, func(t *testing.T) {
			t.Parallel()

			if got := spriteDirection(test.angle, ""); got != test.want {
				t.Fatalf("spriteDirection(%v, \"\") = %q, want %q", test.angle, got, test.want)
			}
		})
	}
}

func TestSpriteDirectionHysteresis(t *testing.T) {
	t.Parallel()

	tests := []struct {
		name     string
		angle    float64
		previous string
		want     string
	}{
		{
			name:     "retain downright after movement crosses boundary",
			angle:    67.946769,
			previous: "downright",
			want:     "downright",
		},
		{
			name:     "change from downright after hysteresis",
			angle:    70.5001,
			previous: "downright",
			want:     "down",
		},
		{
			name:     "retain down before nominal boundary",
			angle:    64.5,
			previous: "down",
			want:     "down",
		},
		{
			name:     "change from down after hysteresis",
			angle:    64.4999,
			previous: "down",
			want:     "downright",
		},
		{
			name:     "retain right across zero degrees",
			angle:    338,
			previous: "right",
			want:     "right",
		},
	}

	for _, test := range tests {
		t.Run(test.name, func(t *testing.T) {
			t.Parallel()

			if got := spriteDirection(test.angle, test.previous); got != test.want {
				t.Fatalf(
					"spriteDirection(%v, %q) = %q, want %q",
					test.angle,
					test.previous,
					got,
					test.want,
				)
			}
		})
	}
}
