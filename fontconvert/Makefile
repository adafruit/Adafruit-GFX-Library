
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

#$(FREEFONTS_ZIP):
#	if [[ -f $(notdir $(FREEFONTS_ZIP)) ]] ; then \
#	  cp $(notdir $(FREEFONTS_ZIP)) $@ ; \
#	else \
#	  curl --output $@ "$(FREEFONTS_URL_BASE)/$(notdir $@)" ; \
#	fi
#
#$(BUILD_DIR)/shasum.passed: $(FREEFONTS_ZIP) freefonts.shasum
#	cd $(BUILD_DIR) ; shasum -a 512224 -c $(abspath freefonts.shasum)
#	touch $@

#$(BUILD_DIR)/freefont-$(FREEFONTS_VERSION): $(BUILD_DIR)/shasum.passed
#	unzip -o $(FREEFONTS_ZIP) -d $(BUILD_DIR) 'freefont-$(FREEFONTS_VERSION)/*.ttf'

fonts:=FreeMono FreeSans FreeSerif
styles:="" "Bold" "Italic" "BoldItalic" "Oblique" "BoldOblique"
sizes:=9 12 18 24

faces:=$(foreach f,$(fonts),$(foreach sty,$(styles),$(shell printf "%s%s" $(f) $(sty))))
outs:=$(foreach face,$(faces),$(foreach sz,$(sizes),$(shell printf "%s/%s%spt7b.h" $(FONT_HDR_DIR) $(face) $(sz))))

breakFont=$(shell echo $(notdir $(1)) | sed -e 's/^\([A-Za-z]*\)\([0-9]*\)pt7b\.h/\1:\2/' | cut -d: -f $(2))
fontSrc=$(BUILD_DIR)/freefont-$(FREEFONTS_VERSION)/$(call breakFont,$1,1).ttf
fontSize=$(call breakFont,$1,2)

#@echo "\$$@:[$@]"
#@echo "[fontSrc:$(call fontSrc,$@)]"
#@echo "[fontSize:$(call fontSize,$@)]"
$(outs):
	if [[ -f "$(call fontSrc,$@)" ]] ; then $(BUILD_DIR)/fontconvert $(call fontSrc,$@) $(call fontSize,$@) > $@ ; fi

# @echo $(BUILD_DIR)/fontconvert $infile $size > $outfile

dbg: $(outs)
