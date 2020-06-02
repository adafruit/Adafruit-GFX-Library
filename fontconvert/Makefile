
TOP_DIR:=$(shell pwd)
FREETYPE2_CONFIG=pkg-config $(TOP_DIR)/freetype2/local/lib/pkgconfig/freetype2.pc
#FREETYPE2_CONFIG=freetype-config
CFLAGS=-Wall $(shell $(FREETYPE2_CONFIG) --cflags )
LDFLAGS=$(shell $(FREETYPE2_CONFIG) --libs)

FONT_CONVERT=./fontconvert
FONT_CONVERT_PY=./fontconvert.py

#FREETYPE_FORCE=env FREETYPE_PROPERTIES="truetype:interpreter-version=35"
#FREETYPE_FORCE=env FREETYPE_PROPERTIES="truetype:interpreter-version=40"


all: annotate

convert: tag.convert tag.convert.py

annotate: tag.annotate.orig tag.annotate.new tag.annotate.py

# expensive ~5minutes, so only really do this before checkin.
format: tag.clang-format

clean: 
	rm -rf fontconvert

FREEFONTS_URL_BASE=http://ftp.gnu.org/gnu/freefont
FREEFONTS_VERSION=20120503
FREEFONTS_ZIP=freefont-ttf-$(FREEFONTS_VERSION).zip

tag.download: freefonts.shasum
	curl --output "$(FREEFONTS_ZIP)" "$(FREEFONTS_URL_BASE)/$(FREEFONTS_ZIP)"
	shasum -a 512224 -c freefonts.shasum
	touch $@

tag.expand: tag.download
	unzip -o freefont-ttf-20120503.zip
	touch $@

tag.convert: tag.expand $(FONT_CONVERT)
	mkdir -p Fonts.new
	for f in Mono Sans Serif; do \
	  for weight in "" Bold; do \
	    for slant in "" Italic Oblique; do \
	      face=Free$${f}$${weight}$${slant}; \
	      infile="freefont-20120503/$${face}.ttf"; \
	      [[ ! -f $$infile ]] && continue; \
	      for sz in 9 12 18 24; do \
		outfile="Fonts.new/$${face}$${sz}pt7b.h"; \
		echo "$$infile $$sz -> $$outfile"; \
		$(FREETYPE_FORCE) $(FONT_CONVERT) $$infile $$sz > $$outfile; \
	      done; \
	    done; \
	  done; \
	done
	touch $@

tag.convert.py: tag.expand $(FONT_CONVERT_PY) gfxfont_annotate.py
	mkdir -p Fonts.py
	for f in Mono Sans Serif; do \
	  for weight in "" Bold; do \
	    for slant in "" Italic Oblique; do \
	      face=Free$${f}$${weight}$${slant}; \
	      infile="freefont-20120503/$${face}.ttf"; \
	      [[ ! -f $$infile ]] && continue; \
	      for sz in 9 12 18 24; do \
		outfile="Fonts.py/$${face}$${sz}pt7b.h"; \
		echo "$$infile $$sz -> $$outfile"; \
		$(FREETYPE_FORCE) $(FONT_CONVERT_PY) $$infile $$sz > $$outfile; \
	      done; \
	    done; \
	  done; \
	done
	touch $@

tag.annotate.new: tag.convert gfxfont_annotate.py
	mkdir -p Fonts.annotated.new
	for f in $$(basename Fonts.new/*.h); do \
	  in=Fonts.new/$$f; \
	  out=Fonts.annotated.new/$$f; \
	  echo "$$in -> $$out"; \
	  ./gfxfont_annotate.py <$$in >$$out; \
	done
	touch $@

tag.annotate.orig: gfxfont_annotate.py
	mkdir -p Fonts.annotated.orig
	for f in $$(basename ../Fonts/*.h); do \
	  in=../Fonts/$$f; \
	  out=Fonts.annotated.orig/$$f; \
	  echo "$$in -> $$out"; \
	  ./gfxfont_annotate.py <$$in >$$out; \
	done
	touch $@

tag.annotate.py: tag.convert.py gfxfont_annotate.py
	mkdir -p Fonts.annotated.py
	for f in $$(basename Fonts.py/*.h); do \
	  in=Fonts.py/$$f; \
	  out=Fonts.annotated.py/$$f; \
	  echo "$$in -> $$out"; \
	  ./gfxfont_annotate.py <$$in >$$out; \
	done
	touch $@

tag.clang-format: tag.annotate.new
	clang-format -verbose -i Fonts.annotated.new/*.h
	touch $@

$(FONT_CONVERT): fontconvert.c tag.freetype2.install
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

FT2_GIT_URL:=git://git.sv.nongnu.org/freetype/freetype2.git

tag.freetype2.clone:
	rm -rf freetype2/src
	mkdir -p freetype2/src
	git clone $(FT2_GIT_URL) --branch=VER-2-6 freetype2/src
	touch $@

tag.freetype2.install: tag.freetype2.clone
	(cd freetype2/src && sh autogen.sh)
	mkdir -p freetype2/build
	(cd freetype2/build && ../src/configure --prefix=$(TOP_DIR)/freetype2/local)
	(cd freetype2/build && make)
	(cd freetype2/build && make install)
	touch $@
