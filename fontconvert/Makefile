
CFLAGS=-Wall $(shell pkg-config --cflags freetype2)
LDFLAGS=$(shell pkg-config --libs freetype2)

#FT26_PKG_CONFIG_DIR=/Users/billy/prog/install/freetype-2.6/lib/pkgconfig
PKG_CONFIG_PATH=$(FT26_PKG_CONFIG_DIR)

all: reconstruct

clean: 
	rm -rf fontconvert

download: tag.verified
expand: download tag.expanded
convert: expand tag.converted
reconstruct: convert tag.reconstructed

tag.verified: get_fonts.sh freefont-ttf-20120503.zip freefonts.shasum
	./get_fonts.sh freefont-ttf-20120503.zip
	shasum -a 512224 -c freefonts.shasum
	touch $@

tag.expanded: tag.verified
	unzip -o freefont-ttf-20120503.zip
	touch $@

tag.converted: fontconvert makefonts.sh tag.expanded
	mkdir -p Fonts
	convert=./fontconvert inpath=freefont-20120503 outpath=Fonts ./makefonts.sh
	touch $@

tag.reconstructed: fontview.py
	mkdir -p Fonts-reconstructed
	for f in Fonts/*.h ; do echo $$f; ./fontview.py <$$f | clang-format >Fonts-reconstructed/$$(basename $$f) ; done
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
