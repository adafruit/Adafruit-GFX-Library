#!/usr/bin/env python3

import sys
import gfxfont

def main():
    data = "".join(sys.stdin)
    font = gfxfont.Parser().parse(data)
    formatter = gfxfont.Formatter(unicode_names=True, break_bmp=True, draw=True)
    print("{}".format(formatter.format(font)))


if __name__ == "__main__":
    main()
