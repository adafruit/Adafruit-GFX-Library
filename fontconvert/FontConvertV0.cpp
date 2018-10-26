/*
TrueType to Adafruit_GFX font converter.  Derived from Peter Jakobs'
Adafruit_ftGFX fork & makefont tool, and Paul Kourany's Adafruit_mfGFX.

NOT AN ARDUINO SKETCH.  This is a command-line tool for preprocessing
fonts to be used with the Adafruit_GFX Arduino library.

For UNIX-like systems.  Outputs to stdout; redirect to header file, e.g.:
  ./fontconvert ~/Library/Fonts/FreeSans.ttf 18 > FreeSans18pt7b.h

REQUIRES FREETYPE LIBRARY.  www.freetype.org

Currently this only extracts the printable 7-bit ASCII chars of a font.
Will eventually extend with some int'l chars a la ftGFX, not there yet.
Keep 7-bit fonts around as an option in that case, more compact.

See notes at end for glyph nomenclature & other tidbits.
*/

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <ft2build.h>
#include FT_GLYPH_H
#include "../gfxfont.h" // Adafruit_GFX font structures
#include "cArrayPtr.h"
#include <iostream>
#include <fstream>
#include <sstream>
#define pgm_read_byte(a) *(uint8_t*)(a)
#define pgm_read_word(a) *(uint16_t*)(a)
#define pgm_read_pointer(a) (uint8_t*)(*a)

using namespace std;

#define DPI 141 // Approximate res. of Adafruit 2.8" TFT

const char *hexa = "0123456789ABCDEF";

void convert(GFXfont *gfxFont, const char *fontName, uint8_t c, uint8_t size, int rawSize, stringstream& data) {
	uint16_t fontPos = 0;
	cArrHolder bitCollection(rawSize); // size from original
	GFXglyph *glyphs = new GFXglyph[gfxFont->last - gfxFont->first + 1];
	(*bitCollection).set2(0);
    c -= pgm_read_byte(&gfxFont->first);
	int chars = gfxFont->last - gfxFont->first;
    // Character is assumed previously filtered by write() to eliminate
    // newlines, returns, non-printable characters, etc.  Calling drawChar()
    // directly with 'bad' characters of font may cause mayhem!

	uint16_t x=0,y=0;
	string bin;
	string check, check2;
	string newBytes;
	memcpy(&glyphs[0], gfxFont->glyph, sizeof(GFXglyph));
	while(c <= chars) {
		GFXglyph *glyph  = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
		uint8_t  *bitmap = (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);
		memcpy(&glyphs[c], glyph, sizeof(GFXglyph));
		glyphs[c].bitmapOffset = fontPos;

		uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
		uint8_t  w  = pgm_read_byte(&glyph->width),
				h  = pgm_read_byte(&glyph->height);
		int8_t   xo = pgm_read_byte(&glyph->xOffset),
				yo = pgm_read_byte(&glyph->yOffset);
		uint8_t  xx, yy, bits = 0, bit = 0;
		int16_t  xo16 = 0, yo16 = 0;
		cArrHolder destBuf((w/8+!!(w&7))*h);

		if(size > 1) {
			xo16 = xo;
			yo16 = yo;
		}

		// Todo: Add character clipping here

		// NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
		// THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
		// has typically been used with the 'classic' font to overwrite old
		// screen contents with new data.  This ONLY works because the
		// characters are a uniform size; it's not a sensible thing to do with
		// proportionally-spaced fonts with glyphs of varying sizes (and that
		// may overlap).  To replace previously-drawn text when using a custom
		// font, use the getTextBounds() function to determine the smallest
		// rectangle encompassing a string, erase the area with fillRect(),
		// then draw new text.  This WILL infortunately 'blink' the text, but
		// is unavoidable.  Drawing 'background' pixels will NOT fix this,
		// only creates a new set of problems.  Have an idea to work around
		// this (a canvas object type for MCUs that can afford the RAM and
		// displays supporting setAddrWindow() and pushColors()), but haven't
		// implemented this yet.

		(*destBuf).set2(0);
		for(yy=0; yy<h; yy++) {
			for(xx=0; xx<w; xx++) {
				if(!(bit++ & 7)) {
					bits = pgm_read_byte(&bitmap[bo++]);
				}
				if(bits & 0x80) {
					bin += '*';
					(*destBuf).setData(*(*destBuf) | (128>>(xx&7)));
				} else
					bin += ' ';
				if(7 == (xx&7)) ++destBuf;
				bits <<= 1;
			}
			if(w&7) ++destBuf;
			bin += "|\n";
		}
//			bin += "\n";
		for(int i=0;i<glyph->width;i++) {
			bin += '-';
		}
		bin += "|\n";
		unsigned int dataWidth = w/8 + !!(w&7);
		(*destBuf).setPos(0l);
		cArrHolder bufPos(*destBuf, dataWidth*h - dataWidth); // -1 row
		if(h > 1) {
		(*bufPos).moveWindow(dataWidth); // 1 row down
		(*bufPos).setPos((*bufPos).end()); // end
		}
		cArrHolder srcBuf(*destBuf, dataWidth*h - dataWidth); // -1 row
		if(h > 1) {
		(*srcBuf).setPos((*srcBuf).end()); // end -1
		}
		unsigned char mask = 128;
//		string check;
		const unsigned char *destBuf2 = (*destBuf).getPos();
		if(h > 1) {
/*		for(int r=0;r<height;r++) {
			for(int c=0;c<dataWidth;c++) {
				mask = 128;
				while(mask) {
					check += mask&*(destBuf2+r*dataWidth+c)?'*':' ';
					mask >>= 1;
				}
			}
			check += '\n';
		}
		cout << check;
		check = "";*/
		do {
			(*bufPos).setData(**bufPos ^ **srcBuf);
			--bufPos; --srcBuf;
		} while(!(*srcBuf).isInvalid());
/*		for(int r=0;r<height;r++) {
			for(int c=0;c<dataWidth;c++) {
				mask = 128;
				while(mask) {
					check += mask&*(destBuf2+r*dataWidth+c)?'*':' ';
					mask >>= 1;
				}
			}
			check += '\n';
		}
		cout << check;*/
		}
		(*destBuf).setPos(0l);
		destBuf2 = (*destBuf).getPos();
//		check = "";
		uint16_t byte = 1;
		for(int r=0;r<h;r++) {
			mask = 128;
			for(int c=0;mask && c<w;c++) {
				byte <<= 1;
				if(mask&*(destBuf2+r*dataWidth+(c/8))) {
					check += '*';
					if(r) {
						if(check2[check2.length()-w-2] == '*')
							check2 += ' ';
						else
							check2 += '*';
					} else {
						check2 += '*';
					}
					byte++;
				} else {
					check += ' ';
					if(r) {
						if(check2[check2.length()-w-2] == ' ')
							check2 += ' ';
						else
							check2 += '*';
					} else {
						check2 += ' ';
					}
				}
				mask >>= 1;
				if(!mask) {
					mask = 128;
				}
				if(byte > 255) {
					(*bitCollection).addBitMappedByte((unsigned char)byte); // save 1/8 zero flag bit
					fontPos++;
					newBytes += "0x";
					byte &= 255;
					newBytes += hexa[byte >> 4];
					newBytes += hexa[byte & 15];
					newBytes += ", ";
					byte = 1;
				}
			}
			check += "|\n";
			check2 += "|\n";
		}
		if((glyph->width * glyph->height) & 7) {
			while(byte < 255) {
				byte <<= 1;
			}
			(*bitCollection).addBitMappedByte((unsigned char)byte); // save 1/8 zero flag bit
			fontPos++;
			newBytes += "0x";
			while(byte < 256) byte <<= 1;
			byte &= 255;
			newBytes += hexa[byte >> 4];
			newBytes += hexa[byte & 15];
			newBytes += ",/*";
		} else
			newBytes += "/*";
		for(int i=0;i<glyph->width;i++) {
			check += '-';
			check2 += '-';
		}
		check += "|\n";
		check2 += "|\n";
		uint16_t bo2 = pgm_read_word(&glyph->bitmapOffset);
		while(bo2 < bo) {
			byte = bitmap[bo2++];
			newBytes += "0x";
			newBytes += hexa[byte >> 4];
			newBytes += hexa[byte & 15];
			newBytes += ", ";
		}
		newBytes += "\n";
		newBytes += check2;
		newBytes += check;
		newBytes += "*/";
//			check2 += "\n";
		if(bin != check2) 
			throw "BUG in pack logic !";
		bin = "";
		check = "";
		check2 = "";
//			newBytes += "\n";
		c++;
	}
/*	string name = "output\\oAA"; // Test output - XORed & UnXORed bytes & ASCII print
	ofstream file(name.c_str());
	file << newBytes;
	file.close();*/
	unsigned int packedSize = (*bitCollection).getBytesUsed();
	(*bitCollection).setPos(0l);
	if(packedSize > (*bitCollection).Capacity())
		exit(-1);
	for(unsigned int j=0;j<packedSize;j++) {
		data << **bitCollection;
		++bitCollection;
	}
	gfxFont->glyph = glyphs;
}

//static uint8_t encoding[8] = {0}, encPos = 0, ba = 0;
// Accumulate bits for output, with periodic hexadecimal byte write
stringstream bin;
void enbit(uint8_t value) {
	static uint8_t row = 0, sum = 0, bit = 0x80, firstCall = 1;
	if(value) sum |= bit;    // Set bit if needed
	if(!(bit >>= 1)) {       // Advance to next bit, end of byte reached?
//		printf("0x%02X", sum); // Write byte value
		bin.write((char*)&sum, 1); // to binary stream only...
		sum       = 0;         // Clear for next byte
		bit       = 0x80;      // Reset bit counter
		firstCall = 0;         // Formatting flag
	}
}

int main(int argc, char *argv[]) {
	unsigned int                i, j, err, size, first=' ', last='~',
	                   bitmapOffset = 0, x, y, byte;
	char              *fontName, c, *ptr;
	FT_Library         library;
	FT_Face            face;
	FT_Glyph           glyph;
	FT_Bitmap         *bitmap;
	FT_BitmapGlyphRec *g;
	GFXglyph          *table;
	uint8_t            bit;
	
	// Parse command line.  Valid syntaxes are:
	//   fontconvert [filename] [size]
	//   fontconvert [filename] [size] [last char]
	//   fontconvert [filename] [size] [first char] [last char]
	// Unless overridden, default first and last chars are
	// ' ' (space) and '~', respectively

	if(argc < 3) {
		fprintf(stderr, "Usage: %s fontfile size [first] [last]\n",
		  argv[0]);
		return 1;
	}

	size = atoi(argv[2]);

	if(argc == 4) {
		last  = atoi(argv[3]);
	} else if(argc == 5) {
		first = atoi(argv[3]);
		last  = atoi(argv[4]);
	}

	if(last < first) {
		i     = first;
		first = last;
		last  = i;
	}

	ptr = strrchr(argv[1], '/'); // Find last slash in filename
	if(ptr) ptr++;         // First character of filename (path stripped)
	else    ptr = argv[1]; // No path; font in local dir.

	// Allocate space for font name and glyph table
	if((!(fontName = (char*)malloc(strlen(ptr) + 20))) ||
	   (!(table = (GFXglyph *)malloc((last - first + 1) *
	    sizeof(GFXglyph))))) {
		fprintf(stderr, "Malloc error\n");
		return 1;
	}

	// Derive font table names from filename.  Period (filename
	// extension) is truncated and replaced with the font size & bits.
	strcpy(fontName, ptr);
	ptr = strrchr(fontName, '.'); // Find last period (file ext)
	if(!ptr) ptr = &fontName[strlen(fontName)]; // If none, append
	// Insert font size and 7/8 bit.  fontName was alloc'd w/extra
	// space to allow this, we're not sprintfing into Forbidden Zone.
	sprintf(ptr, "%dpt%db", size, (last > 127) ? 8 : 7);
	// Space and punctuation chars in name replaced w/ underscores.  
	for(i=0; (c=fontName[i]); i++) {
		if(isspace(c) || ispunct(c)) fontName[i] = '_';
	}

	// Init FreeType lib, load font
	if((err = FT_Init_FreeType(&library))) {
		fprintf(stderr, "FreeType init error: %d", err);
		return err;
	}
	if((err = FT_New_Face(library, argv[1], 0, &face))) {
		fprintf(stderr, "Font load error: %d", err);
		FT_Done_FreeType(library);
		return err;
	}

	// << 6 because '26dot6' fixed-point format
	FT_Set_Char_Size(face, size << 6, 0, DPI, 0);

	// Currently all symbols from 'first' to 'last' are processed.
	// Fonts may contain WAY more glyphs than that, but this code
	// will need to handle encoding stuff to deal with extracting
	// the right symbols, and that's not done yet.
	// fprintf(stderr, "%ld glyphs\n", face->num_glyphs);

//	printf("const uint8_t %sBitmaps[] PROGMEM = {\n  ", fontName);

	// Process glyphs and output huge bitmap data array
	string txt;
	for(i=first, j=0; i<=last; i++, j++) {
		// MONO renderer provides clean image with perfect crop
		// (no wasted pixels) via bitmap struct.
		if((err = FT_Load_Char(face, i, FT_LOAD_TARGET_MONO))) {
			fprintf(stderr, "Error %d loading char '%c'\n",
			  err, i);
			continue;
		}

		if((err = FT_Render_Glyph(face->glyph,
		  FT_RENDER_MODE_MONO))) {
			fprintf(stderr, "Error %d rendering char '%c'\n",
			  err, i);
			continue;
		}

		if((err = FT_Get_Glyph(face->glyph, &glyph))) {
			fprintf(stderr, "Error %d getting glyph '%c'\n",
			  err, i);
			continue;
		}

		bitmap = &face->glyph->bitmap;
		g      = (FT_BitmapGlyphRec *)glyph;

		// Minimal font and per-glyph information is stored to
		// reduce flash space requirements.  Glyph bitmaps are
		// fully bit-packed; no per-scanline pad, though end of
		// each character may be padded to next byte boundary
		// when needed.  16-bit offset means 64K max for bitmaps,
		// code currently doesn't check for overflow.  (Doesn't
		// check that size & offsets are within bounds either for
		// that matter...please convert fonts responsibly.)
		table[j].bitmapOffset = bitmapOffset;
		table[j].width        = bitmap->width;
		table[j].height       = bitmap->rows;
		table[j].xAdvance     = (uint8_t)face->glyph->advance.x >> 6;
		table[j].xOffset      = g->left;
		table[j].yOffset      = 1 - g->top;

		for(y=0; y < bitmap->rows; y++) {
			for(x=0;x < bitmap->width; x++) {
				byte = x / 8;
				bit  = 0x80 >> (x & 7);
				enbit(bitmap->buffer[
				  y * bitmap->pitch + byte] & bit);
			}
		}

		txt += (bitmap->width/10+'0');
		txt += (bitmap->width%10+'0');
		txt += ' ';
		txt += (bitmap->rows/10+'0');
		txt += (bitmap->rows%10+'0');
		txt += ' ';
		txt += i;
		txt += '\n';
		for(y=0; y < bitmap->rows; y++) {
			for(x=0;x < bitmap->width; x++) {
				byte = x / 8;
				bit  = 0x80 >> (x & 7);
				if(bitmap->buffer[
				  y * bitmap->pitch + byte] & bit)
					  txt += '*';
				else txt += ' ';
			}
			txt += '\n';
		}
		txt += '\n';

		// Pad end of char bitmap to next byte boundary if needed
		int n = (bitmap->width * bitmap->rows) & 7;
		if(n) { // Pixel count not an even multiple of 8?
			n = 8 - n; // # bits to next multiple
			while(n--) enbit(0);
		}
		bitmapOffset += (bitmap->width * bitmap->rows + 7) / 8;

		FT_Done_Glyph(glyph);
	}
	GFXfont newOne;
	bin.seekg(0, bin.end);
	unsigned int rawSize = bin.tellg();
	bin.seekg(0, bin.beg);
	newOne.bitmap = new uint8_t[rawSize];
	bin.read((char*)&newOne.bitmap[0], rawSize);
	newOne.glyph = &table[0];
	newOne.first = first;
	newOne.last = last;
	newOne.yAdvance = face->size->metrics.height >> 6;
	bin.str("");
	convert(&newOne, fontName, '!', size, rawSize, bin);
	bin.seekg(0, bin.end);
	unsigned int packedSize = bin.tellg();
	bin.seekg(0, bin.beg);
	/*1070/1431 74% */

	FILE *f = fopen(fontName,"wb");
	fprintf(f,"/*%i/%i %i%s */\nconst uint8_t %sBitmaps[] PROGMEM = {\n", packedSize, rawSize, packedSize*100/rawSize, "%", fontName);
	int remainNZ = 9;
	char bitComment[9] = "12345678";
	for(unsigned int j=0;j<packedSize;j++) {
		bin.read((char*)&byte, 1);
		if(j < packedSize - 1) fprintf(f,"0x%02X, ", byte);
		else fprintf(f,"0x%02X", byte);
		if(remainNZ == 9) {
			int bitsBin = byte, pos = 0;
			remainNZ = 0;
			memset(&bitComment[0], '0', 8);
			while(pos < 8) {
				if(byte&(128>>pos)) bitComment[pos] = '1';
				pos++;
				if(bitsBin&1)
					remainNZ++;
				bitsBin >>= 1;
			}
			fprintf(f,"/*%s*/ ", &bitComment[0]);
		}
//		if(!(j%12)) fprintf(f,"\n");
		if(remainNZ) {
			remainNZ--;
		} else {
			fprintf(f,"\n");
			remainNZ = 9;
		}
	}

	fprintf(f," };\n\n"); // End bitmap array
//	gfxPack(&newOne);

	// Output glyph attributes table (one per character)
	fprintf(f,"const GFXglyph %sGlyphs[] PROGMEM = {\n", fontName);
	for(i=first, j=0; i<=last; i++, j++) {
		fprintf(f,"  { %5d, %3d, %3d, %3d, %4d, %4d }",
		  table[j].bitmapOffset,
		  table[j].width,
		  table[j].height,
		  table[j].xAdvance,
		  table[j].xOffset,
		  table[j].yOffset);
		if(i < last) {
			fprintf(f,",   // 0x%02X", i);
			if((i >= ' ') && (i <= '~')) {
				fprintf(f," '%c'", i);
			}
			fprintf(f,"\n");
		}
	}
	fprintf(f," }; // 0x%02X", last);
	if((last >= ' ') && (last <= '~')) fprintf(f," '%c'", last);
	fprintf(f,"\n\n");

	// Output font structure
	fprintf(f,"const GFXfont %s PROGMEM = {\n", fontName);
	fprintf(f,"  (uint8_t  *)%sBitmaps,\n", fontName);
	fprintf(f,"  (GFXglyph *)%sGlyphs,\n", fontName);
	fprintf(f,"  0x%02X, 0x%02X, %ld };\n\n",
	  first, last, face->size->metrics.height >> 6);
	fprintf(f,"// Approx. %d bytes\n",
	  bitmapOffset + (last - first + 1) * 7 + 7);
	// Size estimate is based on AVR struct and pointer sizes;
	// actual size may vary.
	fprintf(f,"Txt font data:\n%s", txt.c_str());
	fclose(f);

	FT_Done_FreeType(library);

	return 0;
}

/* -------------------------------------------------------------------------

Character metrics are slightly different from classic GFX & ftGFX.
In classic GFX: cursor position is the upper-left pixel of each 5x7
character; lower extent of most glyphs (except those w/descenders)
is +6 pixels in Y direction.
W/new GFX fonts: cursor position is on baseline, where baseline is
'inclusive' (containing the bottom-most row of pixels in most symbols,
except those with descenders; ftGFX is one pixel lower).

Cursor Y will be moved automatically when switching between classic
and new fonts.  If you switch fonts, any print() calls will continue
along the same baseline.

                    ...........#####.. -- yOffset
                    ..........######..
                    ..........######..
                    .........#######..
                    ........#########.
   * = Cursor pos.  ........#########.
                    .......##########.
                    ......#####..####.
                    ......#####..####.
       *.#..        .....#####...####.
       .#.#.        ....##############
       #...#        ...###############
       #...#        ...###############
       #####        ..#####......#####
       #...#        .#####.......#####
====== #...# ====== #*###.........#### ======= Baseline
                    || xOffset

glyph->xOffset and yOffset are pixel offsets, in GFX coordinate space
(+Y is down), from the cursor position to the top-left pixel of the
glyph bitmap.  i.e. yOffset is typically negative, xOffset is typically
zero but a few glyphs will have other values (even negative xOffsets
sometimes, totally normal).  glyph->xAdvance is the distance to move
the cursor on the X axis after drawing the corresponding symbol.

There's also some changes with regard to 'background' color and new GFX
fonts (classic fonts unchanged).  See Adafruit_GFX.cpp for explanation.
*/
