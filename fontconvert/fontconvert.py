#!/usr/bin/env python3

"""

TrueType to Adafruit_GFX font converter.  Derived from Peter Jakobs'
Adafruit_ftGFX fork & makefont tool, and Paul Kourany's Adafruit_mfGFX.

For UNIX-like systems.  Outputs to stdout; redirect to header file, e.g.:
  ./fontconvert ~/Library/Fonts/FreeSans.ttf 18 > FreeSans18pt7b.h

REQUIRES FREETYPE LIBRARY.  www.freetype.org
Install Freetype Python bindings with `python3 -m pip install freetype-py`

Currently this only extracts the printable 7-bit ASCII chars of a font.
Will eventually extend with some int'l chars a la ftGFX, not there yet.
Keep 7-bit fonts around as an option in that case, more compact.

-------------------------------------------------------------------------

Character Metrics
=================

Character metrics are slightly different from classic GFX & ftGFX.
In classic GFX: cursor position is the upper-left pixel of each 5x7
character; lower extent of most glyphs (except those w/descenders)
is +6 pixels in Y direction.
W/new GFX fonts: cursor position is on baseline, where baseline is
'inclusive' (containing the bottom-most row of pixels in most symbols,
except those with descenders; ftGFX is one pixel lower).

Cursor Y will be moved automatically when switching between classic
and new fonts.  If you switch fonts, any print() calls will continue
along the same baseline.

                    ...........#####.. -- yOffset
                    ..........######..
                    ..........######..
                    .........#######..
                    ........#########.
   * = Cursor pos.  ........#########.
                    .......##########.
                    ......#####..####.
                    ......#####..####.
       *.#..        .....#####...####.
       .#.#.        ....##############
       #...#        ...###############
       #...#        ...###############
       #####        ..#####......#####
       #...#        .#####.......#####
====== #...# ====== #*###.........#### ======= Baseline
                    || xOffset

glyph->xOffset and yOffset are pixel offsets, in GFX coordinate space
(+Y is down), from the cursor position to the top-left pixel of the
glyph bitmap.  i.e. yOffset is typically negative, xOffset is typically
zero but a few glyphs will have other values (even negative xOffsets
sometimes, totally normal).  glyph->xAdvance is the distance to move
the cursor on the X axis after drawing the corresponding symbol.

There's also some changes with regard to 'background' color and new GFX
fonts (classic fonts unchanged).  See Adafruit_GFX.cpp for explanation.

"""

import argparse
import json
import sys
import os
import re
import math

from gfxfont import Font as GFX_Font, Glyph as GFX_Glyph
from gfxfont_annotate import GFX_FontFormatter as Formatter

# Adafruit 2.8" TFT (https://www.adafruit.com/product/1770)
#    sqrt(320^2 + 240^2) / 2.8 = 142.86 dpi
_DPI_DEFAULT = 141


def parseOptions():
    parser = argparse.ArgumentParser(
        prog="fontconvert.py",
        description="Convert TTF to adafruit GFXfont header format",
    )
    parser.add_argument("--dpi", type=int, default=_DPI_DEFAULT, help="dots per inch")
    parser.add_argument(
        "infile", type=str, help="TrueType (.ttf) input file",
    )
    parser.add_argument("size", type=int, help="font point size")
    parser.add_argument("--first", type=int, default=0x20, help="first glyph character")
    parser.add_argument("--last", type=int, default=0x7E, help="last glyph character")
    # parser.add_argument(
    #    "outfile",
    #    type=argparse.FileType(mode="w"),
    #    help="GFXfont (.h) output file",
    # )
    class Opt:
        pass

    opt = Opt()
    parser.parse_args(namespace=opt)
    return opt


def ftbmpToBitSeq(bitmap):
    bitSeq = []
    bitPos = 0
    for y in range(0, bitmap.rows):
        rowIdx = y * bitmap.pitch
        bitPos = 0
        for x in range(bitmap.width):
            bitSeq.append((bitmap.buffer[rowIdx] >> (7 - bitPos)) & 1)
            bitPos += 1
            if bitPos == 8:
                rowIdx += 1
                bitPos = 0
    return bitSeq


def bitSeqToGfxBitmap(bitSeq):
    arr = []
    acc = 0
    bitPos = 7
    for b in bitSeq:
        acc = acc | (b << bitPos)
        if bitPos == 0:
            arr.append(acc)
            acc = 0
            bitPos = 8
        bitPos -= 1
    if bitPos != 7:
        arr.append(acc)
    return arr


class Gly:
    pass


class Fnt:
    pass


try:
    import freetype
except ImportError as e:
    print("Missing freetype. Try `python3 -m pip install freetype-py`", file=sys.stderr)
    raise e


def getGlyph(face, ch):
    char = chr(ch)
    face.load_char(char, freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_MONO)
    slot = face.glyph
    gly = Gly()
    gly.ch = ch
    gly.w = slot.bitmap.width
    gly.h = slot.bitmap.rows
    gly.xa = slot.advance.x >> 6
    gly.xo = slot.bitmap_left
    gly.yo = 1 - slot.bitmap_top
    gly.bmp = bitSeqToGfxBitmap(ftbmpToBitSeq(slot.bitmap))
    return gly


def render(infile, size, first, last, dpi, **kwargs):
    if 'freetype_tt_ver' in kwargs :
        if "FREETYPE_PROPERTIES" not in os.environ:
            os.environ["FREETYPE_PROPERTIES"] = ""
        os.environ["FREETYPE_PROPERTIES"] = "{} truetype:interpreter-version={}".format(
            os.environ["FREETYPE_PROPERTIES"], kwargs['freetype_tt_ver']
        )

    face = freetype.Face(infile)
    face.set_char_size(size << 6, 0, dpi, 0)

    font = Fnt()
    font.name = "{}{}pt7b".format(re.match(".*/(\w*).ttf", infile)[1], size)
    font.glyphs = [getGlyph(face, ch) for ch in range(first, last + 1)]
    font.first = first
    font.last = last
    font.ya = face.size.height >> 6

    print("const uint8_t {}Bitmaps[] PROGMEM = {{".format(font.name))
    for gi, g in enumerate(font.glyphs):
        sep = " " if gi == len(font.glyphs) - 1 else ", "
        print(
            "{}{}// ch=0x{:02X}\n".format(
                ",".join(["0x{:02X}".format(x) for x in g.bmp]), sep, g.ch
            )
        )
    print("};\n")

    # Output glyph attributes table (one per character)
    print("const GFXglyph {}Glyphs[] PROGMEM = {{".format(font.name))

    bo = 0
    for gi, g in enumerate(font.glyphs):
        sep = " " if gi == len(font.glyphs) - 1 else ", "
        print(
            "    {{{}, {}, {}, {}, {}, {}}}{}// ch=0x{:02X}".format(
                bo, g.w, g.h, g.xa, g.xo, g.yo, sep, g.ch,
            )
        )
        bo += len(g.bmp)
    print("};\n")

    # Output glyph attributes table (one per character)
    print("const GFXfont {} PROGMEM = {{".format(font.name))
    print("  (uint8_t  *){}Bitmaps,".format(font.name))
    print("  (GFXglyph *){}Glyphs,".format(font.name))
    print("  0x{:02X}, 0x{:02X}, {}}};\n\n".format(font.first, font.last, font.ya))


def main():
    parser = argparse.ArgumentParser(description="Convert TTF to Adafruit-GFX font .h")
    parser.add_argument("--debug", type=bool, default=False, help="enable verbose debug info")
    parser.add_argument("--first", type=int, default=0x20, help="first character")
    parser.add_argument("--last", type=int, default=0x7E, help="last character")
    parser.add_argument("--dpi", type=int, default=_DPI_DEFAULT, help="dots per inch")
    parser.add_argument(
        "--freetype_tt_ver",
        type=int,
        choices=[35, 38, 40],
        default=None,
        help="Explicitly configure Freetype2 truetype:interpreter-version",
    )
    parser.add_argument("infile", type=str, help="TrueType (.ttf) input file")
    parser.add_argument("size", type=int, help="font point size")

    opts = vars(parser.parse_args())
    if opts['debug']:
        print("FreeType v{}".format('.'.join([str(x) for x in freetype.version()])), file=sys.stderr)
    render(**opts)


if __name__ == "__main__":
    main()
