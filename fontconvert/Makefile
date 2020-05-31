
FREETYPE_CONFIG=freetype2-install/bin/freetype-config
CFLAGS=-Wall $(shell $(FREETYPE_CONFIG) --cflags )
LDFLAGS=$(shell $(FREETYPE_CONFIG) --libs)

FONT_CONVERT=./fontconvert
TOP_DIR:=$(shell pwd)

all: annotate

convert: tag.convert

annotate: tag.annotate.orig tag.annotate.new

# expensive ~5minutes, so only really do this before checkin.
format: tag.clang-format

clean: 
	rm -rf fontconvert

tag.download: get_fonts.sh freefonts.shasum
	./get_fonts.sh freefont-ttf-20120503.zip
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
		$(FONT_CONVERT) $$infile $$sz > $$outfile; \
	      done; \
	    done; \
	  done; \
	done
	touch $@

tag.annotate.new: tag.convert font-annotate.py
	mkdir -p Fonts.annotated.new
	for f in $$(basename Fonts.new/*.h); do \
	  in=Fonts.new/$$f; \
	  out=Fonts.annotated.new/$$f; \
	  echo "$$in -> $$out"; \
	  ./font-annotate.py <$$in >$$out; \
	done
	touch $@

tag.annotate.orig: font-annotate.py
	mkdir -p Fonts.annotated.orig
	for f in $$(basename ../Fonts/*.h); do \
	  in=../Fonts/$$f; \
	  out=Fonts.annotated.orig/$$f; \
	  echo "$$in -> $$out"; \
	  ./font-annotate.py <$$in >$$out; \
	done
	touch $@

tag.clang-format: tag.annotate.new
	clang-format -verbose -i Fonts.annotated.new/*.h
	touch $@

# FREETYPE_CONFIG=freetype2-install/bin/freetype-config

$(FONT_CONVERT): fontconvert.c freetype2-install
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

FT2_GIT_URL:=git://git.sv.nongnu.org/freetype/freetype2.git

freetype2-git:
	rm -rf $@
	git clone $(FT2_GIT_URL) --branch=VER-2-6 $@

freetype2-autoconf:
	(cd freetype2-git; sh autogen.sh)

freetype2-configure: freetype2-autoconf
	mkdir -p freetype2-build
	(cd freetype2-build; ../freetype2-git/configure --prefix=$(TOP_DIR)/freetype2-install)

freetype2-make: freetype2-configure
	(cd freetype2-build; make)

freetype2-install: freetype2-make
	(cd freetype2-build; make install)
