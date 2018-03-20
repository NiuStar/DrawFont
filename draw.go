package DrawFont


import (
	/*
	 #cgo LDFLAGS: -L./ -L/Volumes/Mac12/opt/X11/lib -lfreetype -lDrawBrush -lstdc++
	 #cgo CFLAGS: -I./ -I/Volumes/Mac12/opt/X11/include/freetype2
	 #include "DrawBrush.h"
	typedef struct FTLibrary ft_library;
	typedef struct RGBA Color32;

	static void CreateRGBA(struct RGBA *r,uint8 ri, uint8 gi, uint8 bi, uint8 ai)
	{
		InitRGBA(r,ri,gi,bi,ai);

	}

	static void Free(struct Result *r)
	{
		free(r->data);

	}

	typedef struct Result Result;

	*/
	"C"
	"image"
	"unsafe"
	"fmt"
	"reflect"
	"image/color"
	"image/draw"
	"golang.org/x/image/math/fixed"
)

type  DrawFont struct {
	library C.ft_library
	sideScale int //被除数
}


func C2GOUint8(buf *C.uint8, size int) (ret []uint8) {
	hdr := (*reflect.SliceHeader)((unsafe.Pointer(&ret)))
	hdr.Cap = size
	hdr.Len = size
	hdr.Data = uintptr(unsafe.Pointer(buf))
	return
}

func NewDrawFont(fontName string) *DrawFont {
	d := &DrawFont{sideScale:16}
	font := []byte(fontName)
	r := C.InitFTLibrary((*C.char)(unsafe.Pointer(&font[0])),&d.library)
	if int(r) == 1 {
		fmt.Println("InitFTLibrary OK")
		return d
	} else {
		fmt.Println("InitFTLibrary faild")
		return nil
	}
}

func (d *DrawFont)FreeDraw() {
	C.FreeLibrary(&d.library)
}

func (d *DrawFont) PointToFixed(x float64) fixed.Int26_6 {
	return fixed.Int26_6(x * 64.0)
}

func (d *DrawFont)DrawText(text rune,fontColor color.RGBA,fontsize int) *image.RGBA {

	var scolor,fcolor C.Color32

	C.CreateRGBA(&scolor,0,0,0,0)
	C.CreateRGBA(&fcolor,C.uint8(fontColor.R),C.uint8(fontColor.G),C.uint8(fontColor.B),C.uint8(fontColor.A))

	var data C.Result
	C.WriteGlyph(&d.library,C.ulong(text),C.int(fontsize),&fcolor,&scolor,0.0,&data,
	)

	return &image.RGBA{
		Pix:            C2GOUint8(data.data, int(data.len)),
		Stride:         int(data.width * 4),
		Rect:           image.Rect(0, 0, int(data.width), int(data.height)),
	}

}

func (d *DrawFont)DrawTextWithOutLine(text rune,fontColor,outLineColor color.RGBA,fontsize,outlineSize float64) *image.RGBA {

	var scolor,fcolor C.Color32

	C.CreateRGBA(&scolor,C.uint8(outLineColor.R),C.uint8(outLineColor.G),C.uint8(outLineColor.B),C.uint8(outLineColor.A))
	C.CreateRGBA(&fcolor,C.uint8(fontColor.R),C.uint8(fontColor.G),C.uint8(fontColor.B),C.uint8(fontColor.A))

	var data C.Result
	C.WriteGlyph(&d.library,
		C.ulong(text),C.int(fontsize),&fcolor,&scolor,C.float(outlineSize),&data,
	)

	return &image.RGBA{
		Pix:            C2GOUint8(data.data, int(data.len)),
		Stride:         int(data.width * 4),
		Rect:           image.Rect(0, 0, int(data.width), int(data.height)),
	}
}

func (d *DrawFont)DrawStringWithOutLine(text string,fontColor,outLineColor color.RGBA,fontsize,outlineSize float64) *image.RGBA {

	var scolor,fcolor C.Color32

	C.CreateRGBA(&scolor,C.uint8(outLineColor.R),C.uint8(outLineColor.G),C.uint8(outLineColor.B),C.uint8(outLineColor.A))
	C.CreateRGBA(&fcolor,C.uint8(fontColor.R),C.uint8(fontColor.G),C.uint8(fontColor.B),C.uint8(fontColor.A))


	var img *image.RGBA

	var list []*image.RGBA
	var offsetx int = 0
	var height int = 0
	var width int = 0
	var dlist []C.Result
	for _,s := range []rune(text) {
		var data C.Result
		C.WriteGlyph(&d.library,
			C.ulong(s),C.int(fontsize),&fcolor,&scolor,C.float(outlineSize),&data,
		)
		f := &image.RGBA{
			Pix:            C2GOUint8(data.data, int(data.len)),
			Stride:         int(data.width * 4),
			Rect:           image.Rect(0, 0, int(data.width), int(data.height)),
		}
		list = append(list,f)

		dlist = append(dlist,data)

		if width < int(data.width) {
			width = int(data.width)
		}
		if height < int(data.height) {
			height = int(data.height)
		}
	}

	fmt.Println("width:",int(width))


	if img == nil {
		img = image.NewRGBA(image.Rect(0, 0,(int(width) + int(fontsize) / d.sideScale) * len([]rune(text)),int(height)))
	}

	for _,f := range list {


		sr := f.Bounds()         // 获取要复制图片的尺寸 (height - f.Bounds().Dy()) / 2)
		r := sr.Sub(sr.Min).Add(image.Pt(offsetx,(height - f.Bounds().Dy()) / 2))   // 目标图的要剪切区域

		offsetx += width + int(fontsize) / d.sideScale
		draw.Draw(img, r, f, sr.Min, draw.Src)
	}

	for _,f := range dlist {

		C.Free(&f);
	}
	return img
}