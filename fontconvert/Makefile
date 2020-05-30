
CFLAGS:=-Wall $(shell pkg-config --cflags freetype2)
LDFLAGS:=$(shell pkg-config --libs freetype2)

all: convert

clean: 
	rm -rf fontconvert

download: tag.freefont-ttf-20120503.zip.verified
expand: tag.freefont-ttf-20120503.zip.expanded
convert: tag.freefont-ttf-20120503.zip.converted

tag.freefont-ttf-20120503.zip.verified: get_fonts.sh freefont-ttf-20120503.zip freefonts.shasum
	./get_fonts.sh freefont-ttf-20120503.zip
	shasum -a 512224 -c freefonts.shasum
	touch $@

tag.freefont-ttf-20120503.zip.expanded: tag.freefont-ttf-20120503.zip.verified
	unzip -o freefont-ttf-20120503.zip
	touch $@

tag.freefont-ttf-20120503.zip.converted: fontconvert makefonts.sh tag.freefont-ttf-20120503.zip.expanded
	mkdir -p Fonts
	convert=./fontconvert inpath=freefont-20120503 outpath=Fonts ./makefonts.sh
	touch $@

freefont-ttf-20120503.zip: get_fonts.sh
	./get_fonts.sh $@

#fonts:=FreeMono FreeSans FreeSerif
#styles:="" "Bold" "Italic" "BoldItalic" "Oblique" "BoldOblique"
#sizes:=9 12 18 24
#
#faces=$(foreach f,$(fonts),$(foreach sty,$(styles),$(f)$(sty)))
#sizedFaces=$(foreach f,$(faces),$(foreach sz,$(sizes),Fonts/$(f)$(sz)pt7b.h))

$(sizedFaces): convert

dbg:
	@echo $(faces)
	@echo "---"
	@echo $(sizedFaces)
