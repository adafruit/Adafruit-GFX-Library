
TOP_DIR:=$(shell pwd)

FONT_CONVERT_PY=./fontconvert.py

FREEFONTS_URL_BASE=http://ftp.gnu.org/gnu/freefont
FREEFONTS_VERSION=20120503
FREEFONTS_ZIP=freefont-ttf-$(FREEFONTS_VERSION).zip
FONT_SRC_DIR=freefont-$(FREEFONT_VERSION)

all: annotate

convert: tag.convert.py

annotate: tag.annotate.orig tag.annotate.py

format: tag.clang-format

clean: 
	rm -rf fontconvert

tag.download: freefonts.shasum
	curl --output "$(FREEFONTS_ZIP)" "$(FREEFONTS_URL_BASE)/$(FREEFONTS_ZIP)"
	shasum -a 512224 -c freefonts.shasum
	touch $@

tag.expand: tag.download
	unzip -o $(FREEFONTS_ZIP)
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

tag.clang-format.orig: tag.annotate.orig
	clang-format -verbose -i Fonts.annotated.orig/*.h
	touch $@

tag.convert.py: tag.expand $(FONT_CONVERT_PY) gfxfont_annotate.py
	mkdir -p Fonts.py
	for f in Mono Sans Serif; do \
	  for weight in "" Bold; do \
	    for slant in "" Italic Oblique; do \
	      face=Free$${f}$${weight}$${slant}; \
	      infile="$(FONT_SRC_DIR)/$${face}.ttf"; \
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

tag.annotate.py: tag.convert.py gfxfont_annotate.py
	mkdir -p Fonts.annotated.py
	for f in $$(basename Fonts.py/*.h); do \
	  in=Fonts.py/$$f; \
	  out=Fonts.annotated.py/$$f; \
	  echo "$$in -> $$out"; \
	  ./gfxfont_annotate.py <$$in >$$out; \
	done
	touch $@

tag.clang-format.py: tag.annotate.py
	clang-format -verbose -i Fonts.annotated.py/*.h
	touch $@

