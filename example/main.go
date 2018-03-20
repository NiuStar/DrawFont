package main

import (
	"bufio"
	"flag"
	"fmt"
	"image/png"
	"log"
	"os"

	//"unicode"
	"image/color"
	"github.com/NiuStar/DrawFont"
	"image"
	"image/draw"
)

var (
	dpi      = flag.Float64("dpi", 72, "screen resolution in Dots Per Inch")
	fontfile = flag.String("fontfile", "./wryhBold.ttf", "filename of the ttf font")
	hinting  = flag.String("hinting", "none", "none | full")
	size     = flag.Float64("size", 280, "font size in points")
	outLineSize     = flag.Float64("outLineSize", 10, "font size in points")
	spacing  = flag.Float64("spacing", 1.5, "line spacing (e.g. 2 means double spaced)")
	wonb     = flag.Bool("whiteonblack", false, "white text on a black background")
)

var text = []string{
	"欢迎",
	"四川不知道啥的信息化中心",
	"某一个一行",
	"莅临参观指导",

}



func main() {

	b1,err := os.Open("background.png")

	if err != nil {
		panic(err)
	}

	rgba,_, err := image.Decode(b1)
	if err != nil {
		panic(err)
	}

	font := DrawFont.NewDrawFont("./wryhBold.ttf")

	whiteG := color.RGBA{0xff,0xff,0xff,0xff}
	yellowG := color.RGBA{0xff,241,0,0xff}
	outG := color.RGBA{27,91,97,0xff}


	for index, s := range text {

		var f *image.RGBA
		if index == 0 || index == (len(text) - 1) {
			f = font.DrawStringWithOutLine(s,whiteG,outG,220,20.0)
		} else {
			f = font.DrawStringWithOutLine(s,yellowG,outG,220,20.0)
		}

		pt := image.Pt(int((rgba.Bounds().Dx()- f.Bounds().Dx())/2),
			int((rgba.Bounds().Dy()-(f.Bounds().Dy() / 5 + f.Bounds().Dy())*(len(text)-index*int(2)) )/2))

		sr := f.Bounds()         // 获取要复制图片的尺寸
		r := sr.Sub(sr.Min).Add(pt)   // 目标图的要剪切区域

		draw.Draw(rgba.(draw.Image), r, f, sr.Min, draw.Over)
	}
	font.FreeDraw()

	// Save that RGBA image to disk.
	outFile, err := os.Create("out.png")
	if err != nil {
		log.Println(err)
		os.Exit(1)
	}
	defer outFile.Close()
	b := bufio.NewWriter(outFile)
	err = png.Encode(b, rgba)
	if err != nil {
		log.Println(err)
		os.Exit(1)
	}
	err = b.Flush()
	if err != nil {
		log.Println(err)
		os.Exit(1)
	}
	fmt.Println("Wrote out.png OK.")
}