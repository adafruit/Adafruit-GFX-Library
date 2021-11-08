#!/usr/bin/env python3

"""
Adds comments to an Adafruit GFX font .h file helpful for visualizing the data.

Read an Adafruit GFXFont .h file on stdin, and print the same header on stdout,
but with comments that show what each Glyph looks like in ASCII art, as well
as the Unicode name of each glyph.
"""

import sys
import gfxfont

def main():
    data = "".join(sys.stdin)
    font = gfxfont.Parser().parse(data)
    formatter = gfxfont.Formatter(unicode_names=True, break_bmp=True, draw=True)
    print("{}".format(formatter.format(font)))


if __name__ == "__main__":
    main()
