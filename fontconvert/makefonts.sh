#!/bin/bash

# Ugly little Bash script, generates a set of .h files for GFX using
# GNU FreeFont sources.  There are three fonts: 'Mono' (Courier-like),
# 'Sans' (Helvetica-like) and 'Serif' (Times-like); four styles: regular,
# bold, oblique or italic, and bold+oblique or bold+italic; and four
# sizes: 9, 12, 18 and 24 point.  No real error checking or anything,
# this just powers through all the combinations, calling the fontconvert
# utility and redirecting the output to a .h file for each combo.

# Adafruit_GFX repository does not include the source outline fonts
# (huge zipfile, different license) but they're easily acquired:
# http://savannah.gnu.org/projects/freefont/


convert=${convert:-./fontconvert}
inpath=${inpath:-~/Desktop/freefont}
outpath=${outpath:-../Fonts}

fonts=(FreeMono FreeSans FreeSerif)
styles=("" Bold Italic BoldItalic Oblique BoldOblique)
sizes=(9 12 18 24)

for f in "${fonts[@]}"; do
    for st in "${styles[@]}"; do
        infile="${inpath}/${f}${st}.ttf"
        if [[ ! -f $infile ]] ; then
          echo "no input file: \"$infile\"" >&2
          continue
        fi
        for si in "${sizes[@]}"; do
          outfile="${outpath}/${f}${st}${si}pt7b.h"
          ( set -x ; "$convert" "$infile" "$si" > "$outfile" )
        done
    done
done
