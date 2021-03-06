#pragma once


#include "FTLibrary.h"

#include <vector>
#include <fstream>
#include <iostream>


#ifdef _MSC_VER
#define MIN __min
#define MAX __max
#else
#define MIN std::min
#define MAX std::max
#endif


// Define some fixed size types.

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;


// Try to figure out what endian this machine is using. Note that the test
// below might fail for cross compilation; additionally, multi-byte
// characters are implementation-defined in C preprocessors.

#if (('1234' >> 24) == '1')
#elif (('4321' >> 24) == '1')
#define BIG_ENDIAN
#else
#error "Couldn't determine the endianness!"
#endif


// A simple 32-bit pixel.

union Pixel32
{
    Pixel32()
    : integer(0) { }
    Pixel32(uint8 ri, uint8 gi, uint8 bi, uint8 ai = 255)
    {
        b = bi;
        g = gi;
        r = ri;
        a = ai;
    }
    
    uint32 integer;
    
    struct
    {
        uint8 r, g, b , a;
    };
};


struct Vec2
{
    Vec2() { }
    Vec2(float a, float b)
    : x(a), y(b) { }
    
    float x, y;
};


struct Rect
{
    Rect() { }
    Rect(float left, float top, float right, float bottom)
    : xmin(left), xmax(right), ymin(top), ymax(bottom) { }
    
    void Include(const Vec2 &r)
    {
        xmin = MIN(xmin, r.x);
        ymin = MIN(ymin, r.y);
        xmax = MAX(xmax, r.x);
        ymax = MAX(ymax, r.y);
    }
    
    float Width() const { return xmax - xmin + 1; }
    float Height() const { return ymax - ymin + 1; }
    
    float xmin, xmax, ymin, ymax;
};

struct Span
{
    Span() { }
    Span(int _x, int _y, int _width, int _coverage)
    : x(_x), y(_y), width(_width), coverage(_coverage) { }
    
    int x, y, width, coverage;
};

typedef std::vector<Span> Spans;
// TGA Header struct to make it simple to dump a TGA to disc.

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(push, 1)
#pragma pack(1)               // Dont pad the following struct.
#endif

struct TGAHeader
{
    uint8   idLength,           // Length of optional identification sequence.
    paletteType,        // Is a palette present? (1=yes)
    imageType;          // Image data type (0=none, 1=indexed, 2=rgb,
    // 3=grey, +8=rle packed).
    uint16  firstPaletteEntry,  // First palette index, if present.
    numPaletteEntries;  // Number of palette entries, if present.
    uint8   paletteBits;        // Number of bits per palette entry.
    uint16  x,                  // Horiz. pixel coord. of lower left of image.
    y,                  // Vert. pixel coord. of lower left of image.
    width,              // Image width in pixels.
    height;             // Image height in pixels.
    uint8   depth,              // Image color depth (bits per pixel).
    descriptor;         // Image attribute flags.
};

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(pop)
#endif




// Each time the renderer calls us back we just push another span entry on
// our list.

void
RasterCallback(const int y,
               const int count,
               const FT_Span * const spans,
               void * const user);


// Set up the raster parameters and render the outline.

void
RenderSpans(FT_Library &library,
            FT_Outline * const outline,
            Spans *spans);



// Render the specified character as a colored glyph with a colored outline
// and dump it to a TGA.

void
WriteGlyphAsTGA(FT_Library &library,
               
                const unsigned long ch,
                FT_Face &face,
                int size,
                const Pixel32 &fontCol,
                const Pixel32 outlineCol,
                float outlineWidth,Result *result);


bool InitLibrary(const char* font,FTLibrary *libraryS);
// Local Variables:
// coding: utf-8
// End:
