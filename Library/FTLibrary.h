#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
typedef unsigned char uint8;
typedef struct FTLibrary{

    FT_Library library;
    FT_Face face;
};

typedef struct RGBA{

    uint8 r, g, b,a;
};

typedef struct Result{
    uint8 *data;
    int len;
    int width;
    int height;
};
