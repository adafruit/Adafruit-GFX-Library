
CFLAGS  = -Wall $(shell pkg-config --cflags freetype2)
LDFLAGS  = $(shell pkg-config --libs freetype2)

all: build/fontconvert

build:
	mkdir $@


build/fontconvert: build fontconvert.c
	$(CC) fontconvert.c $(CFLAGS) $(LDFLAGS) -o $@
