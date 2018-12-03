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
#ifndef ARDUINO

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <ft2build.h>
#include FT_GLYPH_H
#include "../gfxfont.h" // Adafruit_GFX font structures

#define DPI 141 // Approximate res. of Adafruit 2.8" TFT

/* XOR pack support objects begin */
#include <string>

using namespace std;
class BitOperations {
public:
	BitOperations(unsigned int maxSize) {
		m_bits = maxSize;
		m_bitPos = 0;
		m_mask = 0x80;
	}
	operator bool() const {
		return !!(*m_bitsBytePtr & m_mask);
	}
	const BitOperations operator ++() {
		if(m_bitPos >= m_bits) {
			throw "++ out of bit array !";
		}
		if(m_mask == 0x01) {
			m_mask = 0x80;
			m_bitsBytePtr++;
		} else {
			m_mask >>= 1;
		}
		m_bitPos++;
		return *this;
	};
	void resetPos() {
		m_bitsBytePtr = m_blockBits;
		m_bitPos = 0;
		m_mask = 0x80;
	}
	const unsigned int BitPos() const {
		return m_bitPos;
	}
protected:
	unsigned char *m_blockBits, *m_bitsBytePtr, m_mask;
	unsigned int m_bits, m_bitPos;
};
class BitReader : public BitOperations {
public:
	BitReader(unsigned char *arr, unsigned int maxSize)
	: BitOperations(maxSize) {
		m_blockBits = arr;
		m_bitsBytePtr = m_blockBits;
	}
};
class BitWriter : public BitOperations {
public:
	BitWriter(unsigned char *blockBits, unsigned int bits)
	: BitOperations(bits) {
		m_zerosOnly = true;
		unsigned int blockSize = !!(bits & 7) + (bits / 8);
		if(m_parents = !!blockBits) {
			m_blockBits = blockBits;
			m_bitsBytePtr = m_blockBits;
		} else {
			m_blockBits = new unsigned char[blockSize];
			m_bitsBytePtr = m_blockBits;
			memset(m_bitsBytePtr,0 , blockSize);
		}
	}
	void operator +=(bool bit) {
		if(m_bitPos >= m_bits) {
			throw "Assign past bit array boundary !";
		}
		if(bit) {
			*m_bitsBytePtr |= m_mask;
			m_zerosOnly = false;
		} else ; // not implemented !!!
		if(m_mask == 0x01) {
			m_mask = 0x80;
			m_bitsBytePtr++;
		} else {
			m_mask >>= 1;
		}
		m_bitPos++;
	}
	~BitWriter () {
		if(!m_parents) {
			delete[] m_blockBits;
		}
	}
	bool zeroData() {
		return m_zerosOnly;
	}
	void resetData() {
		m_zerosOnly = true;
		int bits = m_bits;
		for(unsigned char *pos = m_blockBits;bits > 0;bits-=8) {
			*pos++ = 0;
		}
	}
private:
	bool m_parents, m_zerosOnly;
};
class CharBox {
public:
	CharBox(FT_Bitmap *bitmap) {
		memcpy(&m_bitmap, bitmap, sizeof(FT_Bitmap));
		int bufSize = m_bitmap.pitch * m_bitmap.rows;
		m_bitmap.buffer = new unsigned char[bufSize];
		memcpy(m_bitmap.buffer, bitmap->buffer, bufSize);
		int destSize = m_bitmap.width * m_bitmap.rows;
		destSize = (destSize / 8) + !!(destSize & 7);
		m_tinyBuf = new unsigned char[destSize];
		clearTiny();
		m_destBuf = new unsigned char[2 * destSize];
	}
	const unsigned char *getTiny() { return m_tinyBuf; }
	void xor() {
		for(unsigned int y=m_bitmap.rows-1; y; y--) { // XOR character before processing last row with previous, etc.
			for(unsigned int x=0;x < (unsigned)m_bitmap.pitch; x++) {
				m_bitmap.buffer[y * m_bitmap.pitch + x] ^= m_bitmap.buffer[(y-1) * m_bitmap.pitch + x];
			}
		}
	}
	void clearTiny() {
		int destSize = m_bitmap.width * m_bitmap.rows;
		destSize = (destSize / 8) + !!(destSize & 7);
		memset(m_tinyBuf, 0, destSize);
	}
	void unXor() {
		for(unsigned int y=1; y < m_bitmap.rows; y++) {
			for(unsigned int x=0;x < (unsigned)m_bitmap.pitch; x++) {
				m_bitmap.buffer[y * m_bitmap.pitch + x] ^= m_bitmap.buffer[(y-1) * m_bitmap.pitch + x];
			}
		}
		clearTiny();
		copy2tiny();
	}
	unsigned int pack(int bitFrom, int bitTo = -1) {
		copy2tiny();
		if(bitTo < 0) bitTo = m_bitmap.rows * m_bitmap.width + 1;
		if(bitTo > 255) bitTo = 255;
		unsigned int minSize = 0;
		unsigned int minBits = 0;
		BitWriter *out = new BitWriter(m_destBuf, m_bitmap.rows * m_bitmap.width * 2);
		for(unsigned int bits = bitFrom; bits < bitTo; bits++) {
			out->resetData();
			unsigned int len = encode(out, bits);
			if(!minSize || (minSize > len)) {
				minSize = len;
				minBits = bits;
			}
			out->resetData();
			out->resetPos();
		}
		delete out;
		return minBits;
	}
	unsigned char *packedData(unsigned int& bits) {
		BitWriter out(m_destBuf, 8 + (2 * m_bitmap.rows * m_bitmap.width));
		for(unsigned char mask = 128;mask;mask >>= 1) {
			out += bits & mask;
		}
		bits = encode(&out, bits);
		return m_destBuf;
	}
	unsigned int encode(BitWriter *out, unsigned int bits) {
		unsigned int bitSize = m_bitmap.rows * m_bitmap.width;
		BitReader src(m_tinyBuf, bitSize);
		BitWriter block(NULL, bits);
		for(unsigned int b = 0; b < bitSize; b += bits) {
			for(unsigned int bl = 0; (bl < bits)
				&& ((b + bl) < bitSize); bl++) {
				block += src;
				++src;
			}
			if(block.zeroData()) {
				*out += false;
			} else {
				try {
					*out += true;
				} catch (...) {
					out->resetPos();
					return -1;
				}
				block.resetPos();
				for(unsigned int bl = 0; bl < bits; bl++) {
					try {
						*out += block;
					} catch (...) {
						out->resetPos();
						return -1;
					}
					++block ;
				}
			}
			block.resetPos();
			block.resetData();
		}
		int x=src.BitPos();
		int y=out->BitPos();
		return out->BitPos();
	}
	~CharBox() {
		delete[] m_bitmap.buffer;
		delete[] m_tinyBuf;
		delete[] m_destBuf;
	}
protected:
	void copy2tiny() {
		unsigned char *destPos = m_tinyBuf, destMask = 0x80;
		for(unsigned int y=0; y < m_bitmap.rows; y++) {
			for(unsigned int x=0;x < m_bitmap.width; x++) {
				if(m_bitmap.buffer[y * m_bitmap.pitch + (x/8)]
				& (0x80 >> (x & 7))) {
					*destPos |= destMask;
				} else ; // dest zeros untouched !
				destMask >>= 1;
				if(!destMask) {
					destMask = 0x80;
					destPos++;
				}
			}
		}
	}
private:
	FT_Bitmap m_bitmap;
	unsigned char *m_tinyBuf;
	unsigned char *m_destBuf;
};
/* XOR pack support objects end */

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

	printf("const uint8_t %sBitmaps[] PROGMEM = {\n  ", fontName);

	// Process glyphs and output huge bitmap data array
	string txt; // ASCII output
	unsigned int packedSize = 0, origSize = 0, lostBits = 0;
	unsigned long bitsCount = 0;
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
		table[j].width        = bitmap->width;
		table[j].height       = bitmap->rows;
		table[j].xAdvance     = (uint8_t)face->glyph->advance.x >> 6;
		table[j].xOffset      = g->left;
		if(table[j].xAdvance <= table[j].width + table[j].xOffset)
			table[j].xAdvance = 1 + table[j].width + table[j].xOffset;
		table[j].yOffset      = 1 - g->top;

		txt += (bitmap->width/10+'0'); // ASCII output
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

		CharBox b(bitmap);
		b.xor();
		unsigned int size = b.pack(2); // find best block size
		const unsigned char *packed = b.packedData(size); // return buffer and data size in bits
		unsigned int byteSize = !!(size & 7) + (size / 8);

		int originalSize = bitmap->width * bitmap->rows;
		originalSize = !!(originalSize & 7) + (originalSize / 8);
		origSize += originalSize;

		bool orig = originalSize <= byteSize;
		if(orig) { // revert pack when size is same or bigger
			b.unXor();
			packed = b.getTiny();
			size = bitsCount = bitmap->width * bitmap->rows;
			byteSize = originalSize;
		}

		lostBits += (byteSize * 8) - size;

		bitsCount += size;

		if(i > first) {
			printf(",\n");
		}
		if(!orig) {
			printf("/* %c %05i (%ib=>%i/%iB) */ %i", i, bitmapOffset, size, byteSize, originalSize,
				*packed);
		} else {
			printf("/* %c %05i (%ib=>%i/%iB) */ 0x%02X", i, bitmapOffset, size, byteSize, originalSize,
				*packed);
		}
		for(int i=1;i<byteSize;i++) {
			printf(",0x%02X", *(packed+i));
		}
		//if(*(packed+byteSize)) printf(",0x%02X", *(packed+byteSize));
//		printf("\n");
		bitmapOffset += byteSize;
		table[j].bitmapOffset = size; // bit size
		FT_Done_Glyph(glyph);
	}
	packedSize = bitmapOffset;

	printf("\n};\n/* Lost rounded bits: %ub of %u => %f%%\n   Compression result %uB of %u => %f%% */\n\n", lostBits, bitsCount, 100.0*lostBits/bitsCount,
		packedSize, origSize, -100.0*(origSize-packedSize)/origSize); // End bitmap array

	// Output glyph attributes table (one per character)
	printf("const GFXglyph %sGlyphs[] PROGMEM = {\n", fontName);
	for(i=first, j=0; i<=last; i++, j++) {
		printf("  { %5d, %3d, %3d, %3d, %4d, %4d }",
		  table[j].bitmapOffset,
		  table[j].width,
		  table[j].height,
		  table[j].xAdvance,
		  table[j].xOffset,
		  table[j].yOffset);
		if(i < last) {
			printf(",   // 0x%02X", i);
			if((i >= ' ') && (i <= '~')) {
				printf(" '%c'", i);
			}
			printf("\n");
		}
	}
	printf(" }; // 0x%02X", last);
	if((last >= ' ') && (last <= '~')) printf(" '%c'", last);
	printf("\n\n");

	// Output font structure
	printf("const GFXfont %s PROGMEM = {\n", fontName);
	printf("  (uint8_t  *)%sBitmaps,\n", fontName);
	printf("  (GFXglyph *)%sGlyphs,\n", fontName);
	if (face->size->metrics.height == 0) {
      // No face height info, assume fixed width and get from a glyph.
		printf("  0x%02X, 0x%02X, %d };\n\n",
			first, last, table[0].height);
	} else {
		printf("  0x%02X, 0x%02X, %ld };\n\n",
			first, last, face->size->metrics.height >> 6);
	}
	printf("// Approx. %d bytes\n",
	  bitmapOffset + (last - first + 1) * 7 + 7);
	// Size estimate is based on AVR struct and pointer sizes;
	// actual size may vary.
	printf("/* Txt font data:\n%s */", txt.c_str()); // ASCII raw font info

	FT_Done_FreeType(library);
	free(fontName);

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

#endif /* !ARDUINO */
