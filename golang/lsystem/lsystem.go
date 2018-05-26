package main

import (
	"fmt"
	"image"
	"image/color"
	"image/jpeg"
	"math"
	"os"
)

type canvas struct {
	img   *image.RGBA
	Color color.Color
}

type pointer struct {
	X     float64
	Y     float64
	Phi   float64
	V     int
	Color color.Color
}

type lsystem struct {
	omega string
	P     map[string]string
}

func generate(l lsystem, i int) string {
	if i == 0 {
		return l.omega
	} else {
		currentOmega := generate(l, i-1)
		nextOmega := ""
		for _, ch := range currentOmega {
			nextOmega += l.P[string(ch)]
		}
		return nextOmega
	}
}

func (p *pointer) stroke(c canvas) {
	for i := 0; i < p.V; i++ {
		p.X += math.Cos(p.Phi)
		p.Y += math.Sin(p.Phi)
		c.set(int(p.X), int(p.Y), p.Color)
	}
}

func (p *pointer) setPhi(phi float64) {
	p.Phi = phi
}

func (p *pointer) setColor(color color.Color) {
	p.Color = color
}

func (c *canvas) set(x, y int, col color.Color) {
	c.img.Set(x, c.img.Rect.Max.Y-y, col)
}

func (c *canvas) fill() {
	rect := c.img.Rect

	for y := rect.Min.Y; y < rect.Max.Y; y++ {
		for x := rect.Min.X; x < rect.Max.X; x++ {
			c.set(x, y, c.Color)
		}
	}
}

func (c *canvas) drawPlant(p *pointer, dna string) {
	for _, a := range dna {
		switch string(a) {
		case "F":
			p.stroke(*c)
		case "+":
			p.setPhi(math.Mod(p.Phi+(math.Pi/2), 2*math.Pi))
		case "-":
			p.setPhi(math.Mod(p.Phi-(math.Pi/2), 2*math.Pi))
		}
	}
}

func main() {
	filename := "lsystem.jpeg"
	cBlack := color.RGBA{0, 0, 0, 0}
	cWhite := color.RGBA{255, 255, 255, 0}
	img := image.NewRGBA(image.Rect(0, 0, 500, 500))
	c := canvas{img, cBlack}
	p := pointer{1, 1, 0, 3, cWhite}

	c.fill()
	p.setColor(cWhite)

	pp := map[string]string{
		"F": "F+F-F-F+F",
		"+": "+",
		"-": "-",
	}
	l := lsystem{"F", pp}
	var dna = generate(l, 5)
	fmt.Println(dna)

	c.drawPlant(&p, dna)

	file, _ := os.Create(filename)
	defer file.Close()

	if err := jpeg.Encode(file, img, &jpeg.Options{Quality: 100}); err != nil {
		panic(err)
	}
}
