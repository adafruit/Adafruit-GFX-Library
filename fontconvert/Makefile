FT26_PC=/Users/billy/prog/install/freetype-2.6/lib/pkgconfig
PKG_CONFIG=pkg-config
CFLAGS=-Wall $(shell $(PKG_CONFIG) --cflags freetype2)
LDFLAGS=$(shell $(PKG_CONFIG) --libs freetype2)

all: tag.annotate

# (optional)
# all: tag.clang-format

clean: 
	rm -rf fontconvert

tag.download: get_fonts.sh freefonts.shasum
	./get_fonts.sh freefont-ttf-20120503.zip
	shasum -a 512224 -c freefonts.shasum
	touch $@

tag.expand: tag.download
	unzip -o freefont-ttf-20120503.zip
	touch $@

tag.convert: tag.expand fontconvert makefonts.sh
	mkdir -p Fonts
	for f in FreeMono FreeSans FreeSerif; do \
	  for wt in "" Bold; do \
	    for sty in "" Italic Oblique; do \
	      face=$${f}$${wt}$${sty}; \
	      infile="freefont-20120503/$${face}.ttf"; \
	      [[ ! -f $$infile ]] && continue; \
	      for sz in 9 12 18 24; do \
		outfile="Fonts/$${face}$${sz}pt7b.h"; \
		echo "$$infile $$sz -> $$outfile"; \
		./fontconvert $$infile $$sz > $$outfile; \
	      done; \
	    done; \
	  done; \
	done
	touch $@

tag.annotate: tag.convert font-annotate.py
	mkdir -p Fonts-annotated
	for f in $$(basename Fonts/*.h); do \
	  in=Fonts/$$f; \
	  out=Fonts-annotated/$$f; \
	  echo "$$in -> $$out"; \
	  ./font-annotate.py <$$in >$$out; \
	done
	touch $@

tag.clang-format: tag.annotate
	clang-format -i Fonts-annotated/*.h
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
