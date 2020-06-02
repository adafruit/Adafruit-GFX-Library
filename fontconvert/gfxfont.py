#!/usr/bin/env python3

# typedef struct {
#   uint16_t bitmapOffset; ///< Pointer into GFXfont->bitmap
#   uint8_t width;         ///< Bitmap dimensions in pixels
#   uint8_t height;        ///< Bitmap dimensions in pixels
#   uint8_t xAdvance;      ///< Distance to advance cursor (x axis)
#   int8_t xOffset;        ///< X dist from cursor pos to UL corner
#   int8_t yOffset;        ///< Y dist from cursor pos to UL corner
# } GFXglyph;
class Glyph:
    def __init__(self, bo, w, h, xa, xo, yo):
        self.bo = bo
        self.w = w
        self.h = h
        self.xa = xa
        self.xo = xo
        self.yo = yo

# typedef struct {
#   uint8_t *bitmap;  ///< Glyph bitmaps, concatenated
#   GFXglyph *glyph;  ///< Glyph array
#   uint16_t first;   ///< ASCII extents (first char)
#   uint16_t last;    ///< ASCII extents (last char)
#   uint8_t yAdvance; ///< Newline distance (y axis)
# } GFXfont;
class Font:
    def __init__(self, name, bitmap, glyphs, first, last, ya):
        self.name = name
        self.bitmap = bitmap
        self.glyphs = glyphs
        self.first = first
        self.last = last
        self.ya = ya

