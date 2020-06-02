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
import re
import math

from gfxfont import Font as GFX_Font, Glyph as GFX_Glyph
from gfxfont_annotate import GFX_FontFormatter as Formatter

try:
    import freetype
except ImportError as e:
    print("missing freetype. Try `python3 -m pip install freetype-py`", file=sys.stderr)
    raise e

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
        arr.append(acc << (7 - bitPos))
    return arr


def render(infile, size, first, last, dpi):
    chRange = range(first, last + 1)
    face = freetype.Face(infile)
    face.set_char_size(size << 6, 0, dpi, 0)
    fontName = "{}{}".format(re.match(".*/(\w*).ttf", infile)[1], size)

    # First collect all the Glyphs in a table
    table = []
    for ch in chRange:
        char = chr(ch)
        face.load_char(char, freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_MONO)
        slot = face.glyph

        # Reshape the Freetype2 bitmap as an Adafruit_GFXfont bitmap.
        gfxBytes = bitSeqToGfxBitmap(ftbmpToBitSeq(slot.bitmap))

        # print("gfxBytes={}".format(gfxBytes))

        table.append(
            {
                "ch": ch,
                "props": {
                    "w": slot.bitmap.width,
                    "h": slot.bitmap.rows,
                    "xa": slot.advance.x >> 6,
                    "xo": slot.bitmap_left,
                    "yo": 1 - slot.bitmap_top,
                },
                "bmp": gfxBytes,
            }
        )

    print("const uint8_t {}Bitmaps[] PROGMEM = {{".format(fontName))
    for gi in range(len(table)):
        g = table[gi]
        sep = " " if gi == len(table) - 1 else ", "
        print(
            "{}{}// ch=0x{:02X}\n".format(
                ",".join(["0x{:02X}".format(x) for x in g["bmp"]]), sep, g["ch"]
            )
        )
    print("};\n")  # End bitmap array

    # Output glyph attributes table (one per character)
    print("const GFXglyph {}Glyphs[] PROGMEM = {{".format(fontName))

    bo = 0
    for gi in range(len(table)):
        g = table[gi]
        sep = " " if gi == len(table) - 1 else ", "
        print(
            "    {{{}, {}, {}, {}, {}, {}}}{}// ch=0x{:02X}".format(
                bo,
                g["props"]["w"],
                g["props"]["h"],
                g["props"]["xa"],
                g["props"]["xo"],
                g["props"]["yo"],
                sep,
                g["ch"],
            )
        )
        bo += len(g["bmp"])
    print("};\n")

    # No face height info, assume fixed width and get from a glyph.
    #    if (face->size->metrics.height == 0) {
    #        printf("  0x%02X, 0x%02X, %d };\n\n", first, last, table[0].height);
    #    } else {
    #        printf("  0x%02X, 0x%02X, %ld };\n\n", first, last,
    #            face->size->metrics.height >> 6);
    #    }
    ya = face.height >> 6

    # Output glyph attributes table (one per character)
    print("const GFXfont {} PROGMEM = {{".format(fontName))
    print("  (uint8_t  *){}Bitmaps,".format(fontName))
    print("  (GFXglyph *){}Glyphs,".format(fontName))
    print("  0x{:02X}, 0x{:02X}, {}}};\n\n".format(first, last, ya))


def main():
    parser = argparse.ArgumentParser(description="Convert TTF to Adafruit-GFX font .h")
    parser.add_argument("--first", type=int, default=0x20, help="first character")
    parser.add_argument("--last", type=int, default=0x7E, help="last character")
    parser.add_argument("--dpi", type=int, default=_DPI_DEFAULT, help="dots per inch")
    parser.add_argument("infile", type=str, help="TrueType (.ttf) input file")
    parser.add_argument("size", type=int, help="font point size")
    render(**vars(parser.parse_args()))


if __name__ == "__main__":
    main()
