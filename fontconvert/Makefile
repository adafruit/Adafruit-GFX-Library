all: fontconvert

CC     = gcc
CFLAGS = -Wall $(shell pkg-config --cflags freetype2)
LIBS   = $(shell pkg-config --libs freetype2)

fontconvert: fontconvert.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@
	strip $@

clean:
	rm -f fontconvert
