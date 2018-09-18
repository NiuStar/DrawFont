#include "DrawFont.h"



bool
WriteTGA(const std::string &filename,
         const Pixel32 *pxl,
         uint16 width,
         uint16 height)
{
    std::ofstream file(filename.c_str(), std::ios::binary);
    if (file)
    {
        TGAHeader header;
        memset(&header, 0, sizeof(TGAHeader));
        header.imageType  = 2;
        header.width = width;
        header.height = height;
        header.depth = 32;
        header.descriptor = 0x20;
        
        file.write((const char *)&header, sizeof(TGAHeader));
        file.write((const char *)pxl, sizeof(Pixel32) * width * height);
        
        return true;
    }
    return false;
}



void
WriteGlyphAsTGA(FT_Library &library,
                //const std::string &fileName,
                const unsigned long ch,
                FT_Face &face,
                int size,
                const Pixel32 &fontCol,
                const Pixel32 outlineCol,
                float outlineWidth,Result *result)
{
    // Set the size to use.
    if (FT_Set_Char_Size(face, size << 6, size << 6, 90, 90) == 0)
    {
        // Load the glyph we are looking for.
        FT_UInt gindex = FT_Get_Char_Index(face, ch);
        if (FT_Load_Glyph(face, gindex, FT_LOAD_NO_BITMAP) == 0)
        {
            // Need an outline for this to work.
            if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
            {
                // Render the basic glyph to a span list.
                Spans spans;
                RenderSpans(library, &face->glyph->outline, &spans);
                
                // Next we need the spans for the outline.
                Spans outlineSpans;
                
                // Set up a stroker.
                FT_Stroker stroker;
                FT_Stroker_New(library, &stroker);
                FT_Stroker_Set(stroker,
                               (int)(outlineWidth * 64),
                               FT_STROKER_LINECAP_ROUND,
                               FT_STROKER_LINEJOIN_ROUND,
                               0);
                
                FT_Glyph glyph;
                if (FT_Get_Glyph(face->glyph, &glyph) == 0)
                {
                    FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
                    // Again, this needs to be an outline to work.
                    if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
                    {
                        // Render the outline spans to the span list
                        FT_Outline *o =
                        &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
                        RenderSpans(library, o, &outlineSpans);
                    }
                    
                    // Clean up afterwards.
                    FT_Stroker_Done(stroker);
                    FT_Done_Glyph(glyph);
                    
                    // Now we need to put it all together.
                    if (!spans.empty())
                    {
                        // Figure out what the bounding rect is for both the span lists.
                        Rect rect(spans.front().x,
                                  spans.front().y,
                                  spans.front().x,
                                  spans.front().y);
                        for (Spans::iterator s = spans.begin();
                             s != spans.end(); ++s)
                        {
                            rect.Include(Vec2(s->x, s->y));
                            rect.Include(Vec2(s->x + s->width - 1, s->y));
                        }
                        for (Spans::iterator s = outlineSpans.begin();
                             s != outlineSpans.end(); ++s)
                        {
                            rect.Include(Vec2(s->x, s->y));
                            rect.Include(Vec2(s->x + s->width - 1, s->y));
                        }
                        
#if 0
                        // This is unused in this test but you would need this to draw
                        // more than one glyph.
                        float bearingX = face->glyph->metrics.horiBearingX >> 6;
                        float bearingY = face->glyph->metrics.horiBearingY >> 6;
                        float advance = face->glyph->advance.x >> 6;
#endif
                        
                        // Get some metrics of our image.
                        int imgWidth = rect.Width(),
                        imgHeight = rect.Height(),
                        imgSize = imgWidth * imgHeight;
                        
                        // Allocate data for our image and clear it out to transparent.
                        Pixel32 *pxl = new Pixel32[imgSize];
                        memset(pxl, 0, sizeof(Pixel32) * imgSize);
                        
                        // Loop over the outline spans and just draw them into the
                        // image.
                        for (Spans::iterator s = outlineSpans.begin();
                             s != outlineSpans.end(); ++s)
                            for (int w = 0; w < s->width; ++w)
                                pxl[(int)((imgHeight - 1 - (s->y - rect.ymin)) * imgWidth
                                          + s->x - rect.xmin + w)] =
                                Pixel32(outlineCol.r, outlineCol.g, outlineCol.b,
                                        s->coverage);
                        
                        // Then loop over the regular glyph spans and blend them into
                        // the image.
                        for (Spans::iterator s = spans.begin();
                             s != spans.end(); ++s)
                            for (int w = 0; w < s->width; ++w)
                            {
                                Pixel32 &dst =
                                pxl[(int)((imgHeight - 1 - (s->y - rect.ymin)) * imgWidth
                                          + s->x - rect.xmin + w)];
                                Pixel32 src = Pixel32(fontCol.r, fontCol.g, fontCol.b,
                                                      s->coverage);
                                dst.r = (int)(dst.r + ((src.r - dst.r) * src.a) / 255.0f);
                                dst.g = (int)(dst.g + ((src.g - dst.g) * src.a) / 255.0f);
                                dst.b = (int)(dst.b + ((src.b - dst.b) * src.a) / 255.0f);
                                dst.a = MIN(255, dst.a + src.a);
                            }
                        
                        result->data = (uint8_t*)malloc(sizeof(Pixel32) * imgWidth * imgHeight);
                        result->len = sizeof(Pixel32) * imgWidth * imgHeight;
                        result->width = imgWidth;
                        result->height = imgHeight;
                        
                        memcpy(result->data,pxl,result->len);
                        // Dump the image to disk.
                
                        //WriteTGA(fileName, pxl, imgWidth, imgHeight);
                        
                        delete [] pxl;
                    }
                }
            }
        }
    }
}



void
RasterCallback(const int y,
               const int count,
               const FT_Span * const spans,
               void * const user)
{
    Spans *sptr = (Spans *)user;
    for (int i = 0; i < count; ++i)
        sptr->push_back(Span(spans[i].x, y, spans[i].len, spans[i].coverage));
}

void
RenderSpans(FT_Library &library,
            FT_Outline * const outline,
            Spans *spans)
{
    FT_Raster_Params params;
    memset(&params, 0, sizeof(params));
    params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
    params.gray_spans = RasterCallback;
    params.user = spans;
    
    FT_Outline_Render(library, outline, &params);
}

bool InitLibrary(const char* font,FTLibrary *libraryS) {
    
    FT_Library library;
    FT_Init_FreeType(&library);
    
    libraryS->library = library;
    
    std::ifstream fontFile(font, std::ios::binary);
    if (fontFile)
    {
        // Read the entire file to a memory buffer.
        fontFile.seekg(0, std::ios::end);
        std::fstream::pos_type fontFileSize = fontFile.tellg();
        fontFile.seekg(0);
        unsigned char *fontBuffer = new unsigned char[fontFileSize];
        fontFile.read((char *)fontBuffer, fontFileSize);
        FT_Face face;
        FT_New_Memory_Face(library, fontBuffer, fontFileSize, 0, &face);
        
        libraryS->face = face;
        return true;
    }
    return false;
};
