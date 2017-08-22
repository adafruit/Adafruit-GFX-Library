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

convert=./fontconvert
inpath=/usr/share/fonts/truetype/droid
outdir=/home/ji/Arduino/libraries/droidfonts
outpath=${outdir}/fonts
outdoth=${outdir}/droid.h
outdotcpp_decls=${outdir}/droid_decls.cpp
outdotcpp_externs=${outdir}/droid_externs.cpp
outdotcpp_includes=${outdir}/droid_includes.cpp
outdotcpp=${outdir}/droid.cpp
fonts=(DroidSerif-Regular DroidSerif-Italic DroidSerif-Bold DroidSerif-BoldItalic DroidSans DroidSans-Bold DroidSansMono)
sizes=(6 7 8 9 10 11 12 14 16 18 20 24 28 32 36 42 48 56 60 72)


rm -f ${outdoth} ${outdotcpp} ${outdotcpp_externs} ${outdotcpp_includes}
mkdir -p ${outpath}

printf "#include <Adafruit_GFX.h>\n\n" > ${outdoth}
for fontname in ${fonts[*]}; do
  for size in ${sizes[*]}; do
    infile="${inpath}/${fontname}.ttf"
    if [ -f ${infile} ]; then
      outfilebase="${fontname}-${size}-7b.h"
      outfile=${outpath}/${outfilebase}
      # printf "%s %s %s > %s\n" $convert $infile $si $outfile
      ${convert} ${infile} ${size} > ${outfile}
      gfxname="${fontname//-/_}${size}pt7b"
      ptrname="${fontname//-/_}_${size}"
      printf "#include \"fonts/%s\"\n" ${outfilebase} >> ${outdotcpp_includes}
      printf "extern const GFXfont ${gfxname};\n" >>  ${outdotcpp_externs}
      printf "extern const GFXfont* ${ptrname};\n" >> ${outdoth}
      printf "const GFXfont* ${ptrname} = &${gfxname};\n" >> ${outdotcpp_decls}
    fi
  done
done

printf "#include <Adafruit_GFX.h>\n\n" > ${outdotcpp}
cat ${outdotcpp_includes} >> ${outdotcpp}
echo >> ${outdotcpp}
cat ${outdotcpp_externs} >> ${outdotcpp}
echo >> ${outdotcpp}
cat ${outdotcpp_decls} >> ${outdotcpp}
rm -f ${outdotcpp_decls} ${outdotcpp_externs} ${outdotcpp_includes}

cat > ${outdir}/library.properties <<EOF
name=Droid Fonts
version=1.0.0
author=JI
maintainer=JI <ji@tla.org>
sentence=Droid fonts for the Adafruit GFX library
paragraph=Fonts generated from Google's Droid fonts and a modified script from GFX Library
category=Display
url=https://github.com/jayeye/jardino
architectures=*
EOF



