#!/usr/bin/env python3

import sys
import re
import json

# typedef struct {
#   uint16_t bitmapOffset; ///< Pointer into GFXfont->bitmap
#   uint8_t width;         ///< Bitmap dimensions in pixels
#   uint8_t height;        ///< Bitmap dimensions in pixels
#   uint8_t xAdvance;      ///< Distance to advance cursor (x axis)
#   int8_t xOffset;        ///< X dist from cursor pos to UL corner
#   int8_t yOffset;        ///< Y dist from cursor pos to UL corner
# } GFXglyph;
#
# typedef struct {
#   uint8_t *bitmap;  ///< Glyph bitmaps, concatenated
#   GFXglyph *glyph;  ///< Glyph array
#   uint16_t first;   ///< ASCII extents (first char)
#   uint16_t last;    ///< ASCII extents (last char)
#   uint8_t yAdvance; ///< Newline distance (y axis)
# } GFXfont;

class GFX_Glyph:
    def __init__(self, bo, w, h, xa, xo, yo):
        self.bo = bo
        self.w = w
        self.h = h
        self.xa = xa
        self.xo = xo
        self.yo = yo

class GFX_Font:
    def __init__(self, name, bitmap, glyphs, first, last, ya):
        self.name = name
        self.bitmap = bitmap
        self.glyphs = glyphs
        self.first = first
        self.last = last
        self.ya = ya

class GFX_FontParser:
    def __init__(self):
        self._cDecls = {}
        pass

    def parse(self, data):
        stripped = self._stripComments(data)
        return self._parseData(stripped)

    def _stripComments(self, data):
        data = re.sub('/\*.*?\*/', '', data, flags=re.S) # C comments
        data = re.sub('//.*', '', data) # C++ comments
        data = re.sub('#.*', '', data) # CPP directives
        data = re.sub('\s+', ' ', data) # merge whitespace runs
        return data

    def _parseData(self, data):
        for s in re.split(' ?; ?', data):
            s = re.sub('^\s*','',s)
            if self._parseBitmap(s):
                continue
            if self._parseGlyphs(s):
                continue
            if self._parseFont(s):
                return self.font
            raise ValueError("cannot parse statement: `{}`".format(s))

    def _parseBitmap(self, stmt):
        m = re.match('const uint8_t (\w*)\[\w*\] PROGMEM = {\s*(.*)\s*}', stmt, re.S)
        if not m:
            return False
        try:
            self._cDecls[m[1]] = [int(x,0)
                    for x in re.split(" ?, ?", m[2]) if len(x)]
        except Exception as e:
            print("failed to parse stmt=`{}`".format(stmt), file=sys.stderr)
            raise e
        return True

    def _parseGlyphs(self, stmt):
        m = re.match('const GFXglyph (\w*)\[\w*\] PROGMEM = {\s*(.*)\s*}', stmt, re.S)
        if not m:
            return False
        arr = [re.split(' ?, ?',im[1])
                for im in re.finditer('{\s*([^}]*)}', m[2])]
        self._cDecls[m[1]] = [GFX_Glyph(*[int(xe,0) for xe in g])
                for g in arr if len(g)]
        return True

    def _parseFont(self, stmt):
        m = re.match('const GFXfont (\w*) PROGMEM = {\s*(.*?)\s*}', stmt, re.S)
        if not m:
            return False
        name = m[1]
        elements = re.split(' ?, ?', m[2])
        elements = [re.sub('^\(.*?\)', '', e) for e in elements] # strip casting
        bitmap = self._cDecls[elements[0]]
        glyphs = self._cDecls[elements[1]]
        (first, last, ya) = [int(ix,0) for ix in elements[2:5]]
        self.font = GFX_Font(name, bitmap, glyphs, first, last, ya)
        return True

class GFX_FontFormatter:
    """ Emit a GFXfont in C++ header format """

    def __init__(self):
        pass

    def format(self, font):
        def fmtGlyphs(glyphs):
            s = '{'
            for i in range(len(glyphs)):
                ch = font.first + i
                chChar = '' if ch < 0x20 or ch > 0x7e else " '{}'".format(chr(ch))
                comment = "0x{:02X}{}".format(ch, chChar)
                sep = '};' if i == len(glyphs)-1 else ','
                g = glyphs[i]
                s += '{{{}, {}, {}, {}, {}, {}}}{} // {}\n'.format(
                   g.bo, g.w, g.h, g.xa, g.xo, g.yo, sep, comment)
            return s

        approximateBytes = len(font.bitmap) + len(font.glyphs) * 7 + 7

        out = \
"""const uint8_t {0}Bitmaps[] PROGMEM = {{{1}}};

const GFXglyph {0}Glyphs[] PROGMEM = {2}

const GFXfont {0} PROGMEM = {{
    (uint8_t *){0}Bitmaps,
    (GFXglyph *){0}Glyphs,
    0x{3:02X}, 0x{4:02X}, {5}
}};

// Approx. {6} bytes
""".format(font.name,
           ', '.join(['0x{:02X}'.format(x)
               for x in font.bitmap]),
           fmtGlyphs(font.glyphs),
           font.first,
           font.last,
           font.ya,
           approximateBytes)
        return out

def main():
    data = ''.join(sys.stdin)
    font = GFX_FontParser().parse(data)
    def encodingExtension(obj):
        if isinstance(obj, GFX_Glyph):
            return {"$GFX_Glyph": obj.__dict__}
        if isinstance(obj, GFX_Font):
            return {"$GFX_Font": obj.__dict__}
        raise TypeError(repr(obj) + " is not JSON serializable")

    if False:
        print(json.dumps(font, default=encodingExtension))

    print("{}".format(GFX_FontFormatter().format(font)))

if __name__ == "__main__":
    main()

