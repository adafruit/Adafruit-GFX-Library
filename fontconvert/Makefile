FT26_PC=/Users/billy/prog/install/freetype-2.6/lib/pkgconfig
PKG_CONFIG=pkg-config
CFLAGS=-Wall $(shell $(PKG_CONFIG) --cflags freetype2)
LDFLAGS=$(shell $(PKG_CONFIG) --libs freetype2)

all: tag.clang-format

clean: 
	rm -rf fontconvert

tag.download: get_fonts.sh freefont-ttf-20120503.zip freefonts.shasum
	./get_fonts.sh freefont-ttf-20120503.zip
	shasum -a 512224 -c freefonts.shasum
	touch $@

tag.expand: tag.download
	unzip -o freefont-ttf-20120503.zip
	touch $@

tag.convert: tag.expand fontconvert makefonts.sh
	mkdir -p Fonts
	convert=./fontconvert inpath=freefont-20120503 outpath=Fonts ./makefonts.sh
	touch $@

tag.reconstruct: tag.convert font-annotate.py
	mkdir -p Fonts-reconstructed
	for f in Fonts/*.h; do \
	  echo $$f; \
	  ./font-annotate.py <$$f >Fonts-reconstructed/$$(basename $$f) ; \
	done
	clang-format -i Fonts-reconstructed/*.h
	touch $@

tag.clang-format: tag.reconstruct
	clang-format -i Fonts-reconstructed/*.h
	touch $@

fontconvert-freetype-2.6: fontconvert.c
	PKG_CONFIG_PATH=$(FT26_PC)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@
	

#fonts:=FreeMono FreeSans FreeSerif
#styles:="" "Bold" "Italic" "BoldItalic" "Oblique" "BoldOblique"
#sizes:=9 12 18 24
#
#faces=$(foreach f,$(fonts),$(foreach sty,$(styles),$(f)$(sty)))
#sizedFaces=$(foreach f,$(faces),$(foreach sz,$(sizes),Fonts/$(f)$(sz)pt7b.h))
#
#$(sizedFaces): convert
#
#dbg:
#	@echo $(faces)
#	@echo "---"
#	@echo $(sizedFaces)
