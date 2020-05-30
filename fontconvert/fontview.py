#!/usr/bin/env python3

import sys
import re

class ParseFont:
    def __init__(self):
        self.data = {}

    def parse(self, data):
        stmts = re.split('\s*;\s*', data)
        #for s in stmts:
        #    print("s={}".format(s))

        if True:
            #print("stmts[0]={}".format(stmts[0]))
            m = re.match('const uint8_t (.*)\[\] PROGMEM = {\s*(.*)\s*}', stmts[0])
            if not m:
                raise ValueError("invalid Bitmaps decl")
            self.bmpName = m[1]
            self.bmp = [int(x,0) for x in re.split("\s*,\s*", m[2])]

        if True:
            #print("stmts[1]={}".format(stmts[1]))
            m = re.match('const GFXglyph (.*)\[\] PROGMEM = {\s*(.*)\s*}', stmts[1])
            if not m:
                raise ValueError("invalid glyphs decl")
            self.glyphsName = m[1]  # should be same as self.name
            arr = [re.split('\s*,\s*',im[1]) for im in re.finditer('{\s*([^}]*)}', m[2])]
            gg = [[int(xe,0) for xe in g] for g in arr]
            self.glyphs = gg

        if True:
            #print("stmts[2]={}".format(stmts[2]))
            m = re.match('const GFXfont (\w*) PROGMEM = {\s*(.*?)\s*}', stmts[2])
            if not m:
                raise ValueError("invalid GFXfont decl")

            self.name = m[1]

            ls = [int(ix,0) for ix in re.split(',\s*', m[2])[2:5]]
            (self.first, self.last, self.yAdvance) = ls
        print("gfx:{{name:{}, first:{}, last:{}, yAdvance:{},glyphs:{},bmp:{}}}".format(
            self.name, self.first, self.last, self.yAdvance,
            self.glyphs, self.bmp))

def main():
    data = ""

    #print("args[1] = `{}`".format(sys.argv[1]))
    fh = sys.stdin
    i = 0
    for line in fh:
        line = re.sub('/\*.*\*/', '', line)
        line = re.sub('//.*', '', line)
        line = re.sub('^\s*', '', line)
        line = re.sub('\s{1,}', ' ', line)
        #line = re.sub('\n', '', line)
        if not len(line):
            #print("[skipping blank line]");
            continue
        if re.match('^#.*$', line):
            #print("[skipping: {}]".format(line))
            continue
        # print("[{}]{}".format(i, line))
        data = data + line
        i = i + 1

    pf = ParseFont()
    pf.parse(data)

    #print("name: {}".format(pf.name))
    #print("bmp[{}]: {}".format(len(pf.bmp), pf.bmp))
    #print("glyphs[{}]: {}".format(len(pf.glyphs), pf.glyphs))

if __name__ == "__main__":
    main()

