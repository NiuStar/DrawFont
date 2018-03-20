#pragma once

#ifdef __cplusplus
extern "C" {
    #endif
#include "FTLibrary.h"
    
    int InitFTLibrary(const char* font,struct FTLibrary *libraryS);
    void WriteGlyph(struct FTLibrary *libraryS,const unsigned long ch,int size,
                     const struct RGBA*fontCol,
                     const struct RGBA* outlineCol,
                      float outlineWidth,
                   struct Result *result);
    
    void WriteGlyph2(struct FTLibrary *libraryS,const unsigned long ch,int size,struct Result *result);
    
    void InitRGBA(struct RGBA* rgba,uint8 r,uint8 g,uint8 b,uint8 a);
    
    void FreeLibrary(struct FTLibrary *libraryS);
    #ifdef __cplusplus
}
#endif
