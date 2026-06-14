package main

import "testing"

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
