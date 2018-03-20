#include"DrawBrush.h"
#include"DrawFont.h"
int InitFTLibrary(const char* font,FTLibrary *libraryS) {
    
    return InitLibrary(font,(FTLibrary*)libraryS);
   
}
void WriteGlyph(FTLibrary *libraryS,const unsigned long ch,int size,
                const struct RGBA*fontCol,
                const struct RGBA* outlineCol,
                float outlineWidth,struct Result *result) {

    
    FT_Library &library = ((FTLibrary*)libraryS)->library;
    FT_Face &face = ((FTLibrary*)libraryS)->face;
    WriteGlyphAsTGA(library,ch,face,size,Pixel32(fontCol->r, fontCol->g, fontCol->b, fontCol->a),Pixel32(outlineCol->r, outlineCol->g, outlineCol->b, outlineCol->a),outlineWidth,result);
}

void WriteGlyph2(struct FTLibrary *libraryS,const unsigned long ch,int size,struct Result *result) {
    FT_Library &library = ((FTLibrary*)libraryS)->library;
    FT_Face &face = ((FTLibrary*)libraryS)->face;
    WriteGlyphAsTGA(library,ch,face,size,Pixel32(255,90,30),Pixel32(255,255,255),3.0f,result);
}

void InitRGBA(struct RGBA* rgba,uint8 r,uint8 g,uint8 b,uint8 a) {
    rgba->r = r;
    rgba->g = g;
    rgba->b = b;
    rgba->a = a;
}

void FreeLibrary(struct FTLibrary *libraryS) {
    FT_Done_FreeType(libraryS->library);
}

