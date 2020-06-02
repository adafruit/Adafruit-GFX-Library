#!/usr/bin/env python3

import sys
import re
import json
import unicodedata

from gfxfont import Font as GFX_Font, Glyph as GFX_Glyph

class GFX_FontParser:
    def __init__(self):
        self._cDecls = {}
        pass

    def parse(self, data):
        # self.headerComment = self._extractHeaderComments(data)
        stripped = self._stripComments(data)
        return self._parseCxxFile(stripped)

    # def _extractHeaderComments(self, data):
    #    pass

    def _stripComments(self, data):
        data = re.sub("/\*.*?\*/", "", data, flags=re.S)  # C comments
        data = re.sub("//.*", "", data)  # C++ comments
        data = re.sub("#.*", "", data)  # CPP directives
        data = re.sub("\s+", " ", data)  # merge whitespace runs
        return data

    def _parseCxxFile(self, data):
        for s in re.split(" *; *", data):
            s = re.sub("^\s*", "", s)
            # print("_parseCxxFile: s:`{}`".format(s), file=sys.stderr)
            if not len(s):
                continue
            if self._parseBitmap(s):
                continue
            if self._parseGlyphs(s):
                continue
            if self._parseFont(s):
                return self.font
            raise ValueError("Cannot parse statement: `{}`".format(s))

    def _parseBitmap(self, stmt):
        m = re.match("const uint8_t (\w*)\[\w*\] PROGMEM = {\s*(.*)\s*}", stmt, re.S)
        if not m:
            return False
        try:
            self._cDecls[m[1]] = [int(x, 0) for x in re.split(" ?, ?", m[2]) if len(x)]
        except Exception as e:
            print("failed to parse stmt=`{}`".format(stmt), file=sys.stderr)
            raise e
        return True

    def _parseGlyphs(self, stmt):
        m = re.match("const GFXglyph (\w*)\[\w*\] PROGMEM = {\s*(.*)\s*}", stmt, re.S)
        if not m:
            return False
        arr = [re.split(" ?, ?", im[1]) for im in re.finditer("{\s*([^}]*)}", m[2])]
        self._cDecls[m[1]] = [
            GFX_Glyph(*[int(xe, 0) for xe in g]) for g in arr if len(g)
        ]
        return True

    def _parseFont(self, stmt):
        m = re.match("const GFXfont (\w*) PROGMEM = {\s*(.*?)\s*}", stmt, re.S)
        if not m:
            return False
        name = m[1]
        elements = re.split(" ?, ?", m[2])
        elements = [re.sub("^\(.*?\)", "", e) for e in elements]  # strip casting
        bitmap = self._cDecls[elements[0]]
        glyphs = self._cDecls[elements[1]]
        (first, last, ya) = [int(ix, 0) for ix in elements[2:5]]
        self.font = GFX_Font(name, bitmap, glyphs, first, last, ya)
        return True


class GFX_FontFormatter:
    """ Emit a GFXfont in C++ header format """

    def __init__(self, **kwargs):
        getIf = lambda key: kwargs[key] if key in kwargs else False
        self._unicode_names = getIf("unicode_names")
        self._break_bmp = getIf("break_bmp")
        self._draw = getIf("draw")

    def _fmtBitmap(self, font):
        s = "{\n"
        if self._break_bmp:
            glyphEnds = {}
            ch = font.first
            for g in font.glyphs:
                glyphEnds[int(g.bo + ((g.h * g.w) + 7) / 8)] = (g, ch)
                ch += 1

            # Place a clang-format resistant comment after each glyph in the bmp
            def annotation(i):
                (g, ch) = glyphEnds[i]
                s = " // 0x{:02X} '{}' ({} x {}) @({},{}) +{}\n\n".format(
                    ch, chr(ch), g.w, g.h, g.xo, g.yo, g.xa
                )
                if self._draw:
                    box_t = min(g.yo, 0)
                    box_b = max(g.yo + g.h - 1, 0)
                    box_l = min(g.xo, 0)
                    box_r = max(g.xo + g.w - 1, g.xa, 0)
                    grid = [
                        [" " for x in range(box_l, box_r + 1)]
                        for y in range(box_t, box_b + 1)
                    ]

                    def pt(x, y, c, alt=None):
                        if grid[y - box_t][x - box_l] == " ":
                            grid[y - box_t][x - box_l] = c
                        elif alt:
                            grid[y - box_t][x - box_l] = alt
                        return ""

                    s += pt(0, 0, "0", "@")
                    s += pt(g.xa, 0, ">", "}")
                    pos = 8 * g.bo
                    for y in range(g.h):
                        # s += '// '
                        for x in range(g.w):
                            bit = font.bitmap[int(pos / 8)]
                            bit = (bit >> (7 - (pos % 8))) & 1
                            # s += '*' if bit else '.'
                            if bit:
                                s += pt(g.xo + x, g.yo + y, "*", "X")
                            else:
                                s += pt(g.xo + x, g.yo + y, ".")
                            pos += 1
                        # s += '\n'
                    for row in grid:
                        s += "// [" + "".join(row) + "]\n"
                s += "\n"
                return s

            i = 0
            for i in range(len(font.bitmap)):
                if i in glyphEnds:
                    s += annotation(i)
                sep = "" if i == len(font.bitmap) - 1 else ","
                s += "0x{:02X}{}".format(font.bitmap[i], sep)
                i += 1
            if i in glyphEnds:
                s += annotation(i)
        else:
            s += ", ".join(["0x{:02X}".format(x) for x in font.bitmap])
        s += "};"
        return s

    def _fmtGlyphs(self, font):
        s = "{"
        for i in range(len(font.glyphs)):
            ch = font.first + i
            chChr = chr(ch)

            if self._unicode_names:
                chName = ""
                try:
                    uName = unicodedata.name(chChr)
                    # If it has a uName we can print the char and its name
                    chName += " {}:{}".format(chChr, uName)
                except ValueError:
                    pass
                chComment = "{}".format(chName)
            else:
                prn = ch >= 0x20 and ch <= 0x7E
                chComment = " '{}'".format(chChr) if prn else ""
            comment = "0x{:02X}{}".format(ch, chComment)
            sep = "};" if i == len(font.glyphs) - 1 else ","
            g = font.glyphs[i]
            s += "{{{}, {}, {}, {}, {}, {}}}{} // {}\n".format(
                g.bo, g.w, g.h, g.xa, g.xo, g.yo, sep, comment
            )
        return s

    def format(self, font):
        fsz = 7  # sizeof(GFXfont)
        gsz = 7  # sizeof(GFXglyph)
        approximateBytes = len(font.bitmap) + len(font.glyphs) * gsz + fsz

        headerComment = "// Processed by `{}`\n".format(" ".join(sys.argv))

        out = """{header_comment}
const uint8_t {name}Bitmaps[] PROGMEM = {bitmap}

const GFXglyph {name}Glyphs[] PROGMEM = {glyphs}

const GFXfont {name} PROGMEM = {{
    (uint8_t *){name}Bitmaps,
    (GFXglyph *){name}Glyphs,
    0x{first:02X}, 0x{last:02X}, {ya}
}};

// Approx. {approximateBytes} bytes
""".format(
            header_comment=headerComment,
            name=font.name,
            bitmap=self._fmtBitmap(font),
            glyphs=self._fmtGlyphs(font),
            first=font.first,
            last=font.last,
            ya=font.ya,
            approximateBytes=approximateBytes,
        )
        return out


def main():
    data = "".join(sys.stdin)
    font = GFX_FontParser().parse(data)

    def encodingExtension(obj):
        if isinstance(obj, GFX_Glyph):
            return {"$GFX_Glyph": obj.__dict__}
        if isinstance(obj, GFX_Font):
            return {"$GFX_Font": obj.__dict__}
        raise TypeError(repr(obj) + " is not JSON serializable")

    if False:
        print(json.dumps(font, default=encodingExtension))

    fmtArgs = {
        "unicode_names": True,
        "break_bmp": True,
        "draw": True,
    }
    formatter = GFX_FontFormatter(**fmtArgs)
    print("{}".format(formatter.format(font)))


if __name__ == "__main__":
    main()
