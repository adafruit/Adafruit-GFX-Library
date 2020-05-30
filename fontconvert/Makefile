
CFLAGS:=-Wall $(shell pkg-config --cflags freetype2)
LDFLAGS:=$(shell pkg-config --libs freetype2)
BUILD_DIR:=build

SRCS:=fontconvert.c

FREEFONTS_VERSION:=20120503
FREEFONTS_ZIP:=$(BUILD_DIR)/freefont-ttf-$(FREEFONTS_VERSION).zip
FREEFONTS_URL_BASE:=http://ftp.gnu.org/gnu/freefont

FONT_HDR_DIR:=$(BUILD_DIR)/Fonts

all: $(BUILD_DIR) $(BUILD_DIR)/fontconvert $(BUILD_DIR)/freefont-$(FREEFONTS_VERSION)

clean: 
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/fontconvert: fontconvert.c
	$(CC) $(SRCS) $(CFLAGS) $(LDFLAGS) -o $@

$(FREEFONTS_ZIP):
	if [[ -f $(notdir $(FREEFONTS_ZIP)) ]] ; then \
	  cp $(notdir $(FREEFONTS_ZIP)) $@ ; \
	else \
	  curl --output $@ "$(FREEFONTS_URL_BASE)/$(notdir $@)" ; \
	fi

$(BUILD_DIR)/shasum.passed: $(FREEFONTS_ZIP) freefonts.shasum
	cd $(BUILD_DIR) ; shasum -a 512224 -c $(abspath freefonts.shasum)
	touch $@

$(BUILD_DIR)/freefont-$(FREEFONTS_VERSION): $(BUILD_DIR)/shasum.passed
	unzip -o $(FREEFONTS_ZIP) -d $(BUILD_DIR) 'freefont-$(FREEFONTS_VERSION)/*.ttf'

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
