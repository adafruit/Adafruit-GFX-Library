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

#include <assert.h>
#include <ctype.h>
#include <ft2build.h>
#include <stdint.h>
#include <stdio.h>
#include FT_GLYPH_H
#include FT_MODULE_H
#include FT_TRUETYPE_DRIVER_H
#include "../gfxfont.h" // Adafruit_GFX font structures

#define DPI 141 // Approximate res. of Adafruit 2.8" TFT

// Accumulate bits for output, with periodic hexadecimal byte write
void enbit(uint8_t value) {
  static uint8_t row = 0, sum = 0, bit = 0x80, firstCall = 1;
  if (value)
    sum |= bit;          // Set bit if needed
  if (!(bit >>= 1)) {    // Advance to next bit, end of byte reached?
    if (!firstCall) {    // Format output table nicely
      if (++row >= 12) { // Last entry on line?
        printf(",\n  "); //   Newline format output
        row = 0;         //   Reset row counter
      } else {           // Not end of line
        printf(", ");    //   Simple comma delim
      }
    }
    printf("0x%02X", sum); // Write byte value
    sum = 0;               // Clear for next byte
    bit = 0x80;            // Reset bit counter
    firstCall = 0;         // Formatting flag
  }
}

struct Remap {
  uint64_t char_to_replace;
  uint64_t replace_with;
  struct Remap *next;
};

struct Remap *parse_remap(char *input);
uint64_t remap_char(struct Remap *remap, uint64_t in);

char *to_utf8(uint64_t codepoint);
uint64_t read_utf8(char **input_ptr);

int main(int argc, char *argv[]) {
  int i, j, err, size, first = ' ', last = '~', bitmapOffset = 0, x, y, byte;
  char *fontName, c, *ptr;
  FT_Library library;
  FT_Face face;
  FT_Glyph glyph;
  FT_Bitmap *bitmap;
  FT_BitmapGlyphRec *g;
  GFXglyph *table;
  uint8_t bit;
  struct Remap *remap = NULL;

  // Parse command line.  Valid syntaxes are:
  //   fontconvert [filename] [size]
  //   fontconvert [filename] [size] [last char]
  //   fontconvert [filename] [size] [first char] [last char]
  //   fontconvert [filename] [size] [first char] [last char] [remap]
  // Unless overridden, default first and last chars are
  // ' ' (space) and '~', respectively
  //
  // Remap is a string containing pairs of characters to remap.
  // Example:
  //   fontconvert myfont.ttf 16 48 58 :°
  // creates a font which contains digits '0' to '9' and the degree symbol,
  // with the degree symbol glyph is mapped to ASCII character ':' (58).
  // Thus when drawing text "42:", one would get "42°".
  //
  // If remap is given, "Custom" is appended to the font name.

  if (argc < 3) {
    fprintf(stderr, "Usage: %s fontfile size [first] [last] [remap]\n",
            argv[0]);
    return 1;
  }

  size = atoi(argv[2]);

  if (argc == 4) {
    last = atoi(argv[3]);
  } else if (argc == 5) {
    first = atoi(argv[3]);
    last = atoi(argv[4]);
  } else if (argc == 6) {
    first = atoi(argv[3]);
    last = atoi(argv[4]);
    remap = parse_remap(argv[5]);
  }

  if (last < first) {
    i = first;
    first = last;
    last = i;
  }

  ptr = strrchr(argv[1], '/'); // Find last slash in filename
  if (ptr)
    ptr++; // First character of filename (path stripped)
  else
    ptr = argv[1]; // No path; font in local dir.

  // Allocate space for font name and glyph table
  if ((!(fontName = malloc(strlen(ptr) + 20 + (remap ? 6 : 0)))) ||
      (!(table = (GFXglyph *)malloc((last - first + 1) * sizeof(GFXglyph))))) {
    fprintf(stderr, "Malloc error\n");
    return 1;
  }

  // Derive font table names from filename.  Period (filename
  // extension) is truncated and replaced with the font size & bits.
  strcpy(fontName, ptr);
  ptr = strrchr(fontName, '.'); // Find last period (file ext)
  if (!ptr)
    ptr = &fontName[strlen(fontName)]; // If none, append
  // Insert font size and 7/8 bit.  fontName was alloc'd w/extra
  // space to allow this, we're not sprintfing into Forbidden Zone.
  sprintf(ptr, "%dpt%db", size, (last > 127) ? 8 : 7);
  // Space and punctuation chars in name replaced w/ underscores.
  for (i = 0; (c = fontName[i]); i++) {
    if (isspace(c) || ispunct(c))
      fontName[i] = '_';
  }

  if (remap) {
    strcat(fontName, "Custom");
  }

  // Init FreeType lib, load font
  if ((err = FT_Init_FreeType(&library))) {
    fprintf(stderr, "FreeType init error: %d", err);
    return err;
  }

  // Use TrueType engine version 35, without subpixel rendering.
  // This improves clarity of fonts since this library does not
  // support rendering multiple levels of gray in a glyph.
  // See https://github.com/adafruit/Adafruit-GFX-Library/issues/103
  FT_UInt interpreter_version = TT_INTERPRETER_VERSION_35;
  FT_Property_Set(library, "truetype", "interpreter-version",
                  &interpreter_version);

  if ((err = FT_New_Face(library, argv[1], 0, &face))) {
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
  for (i = first, j = 0; i <= last; i++, j++) {
    // MONO renderer provides clean image with perfect crop
    // (no wasted pixels) via bitmap struct.
    if ((err = FT_Load_Char(face, remap_char(remap, i), FT_LOAD_TARGET_MONO))) {
      fprintf(stderr, "Error %d loading char '%c'\n", err, i);
      continue;
    }

    if ((err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO))) {
      fprintf(stderr, "Error %d rendering char '%c'\n", err, i);
      continue;
    }

    if ((err = FT_Get_Glyph(face->glyph, &glyph))) {
      fprintf(stderr, "Error %d getting glyph '%c'\n", err, i);
      continue;
    }

    bitmap = &face->glyph->bitmap;
    g = (FT_BitmapGlyphRec *)glyph;

    // Minimal font and per-glyph information is stored to
    // reduce flash space requirements.  Glyph bitmaps are
    // fully bit-packed; no per-scanline pad, though end of
    // each character may be padded to next byte boundary
    // when needed.  16-bit offset means 64K max for bitmaps,
    // code currently doesn't check for overflow.  (Doesn't
    // check that size & offsets are within bounds either for
    // that matter...please convert fonts responsibly.)
    table[j].bitmapOffset = bitmapOffset;
    table[j].width = bitmap->width;
    table[j].height = bitmap->rows;
    table[j].xAdvance = face->glyph->advance.x >> 6;
    table[j].xOffset = g->left;
    table[j].yOffset = 1 - g->top;

    for (y = 0; y < bitmap->rows; y++) {
      for (x = 0; x < bitmap->width; x++) {
        byte = x / 8;
        bit = 0x80 >> (x & 7);
        enbit(bitmap->buffer[y * bitmap->pitch + byte] & bit);
      }
    }

    // Pad end of char bitmap to next byte boundary if needed
    int n = (bitmap->width * bitmap->rows) & 7;
    if (n) {     // Pixel count not an even multiple of 8?
      n = 8 - n; // # bits to next multiple
      while (n--)
        enbit(0);
    }
    bitmapOffset += (bitmap->width * bitmap->rows + 7) / 8;

    FT_Done_Glyph(glyph);
  }

  printf(" };\n\n"); // End bitmap array

  // Output glyph attributes table (one per character)
  printf("const GFXglyph %sGlyphs[] PROGMEM = {\n", fontName);
  for (i = first, j = 0; i <= last; i++, j++) {
    printf("  { %5d, %3d, %3d, %3d, %4d, %4d }", table[j].bitmapOffset,
           table[j].width, table[j].height, table[j].xAdvance, table[j].xOffset,
           table[j].yOffset);
    if (i < last) {
      printf(",   // 0x%02X", i);
      if ((i >= ' ') && (i <= '~')) {
        printf(" '%s'", to_utf8(remap_char(remap, i)));
      }
      putchar('\n');
    }
  }
  printf(" }; // 0x%02X", last);
  if ((last >= ' ') && (last <= '~'))
    printf(" '%s'", to_utf8(remap_char(remap, last)));
  printf("\n\n");

  // Output font structure
  printf("const GFXfont %s PROGMEM = {\n", fontName);
  printf("  (uint8_t  *)%sBitmaps,\n", fontName);
  printf("  (GFXglyph *)%sGlyphs,\n", fontName);
  if (face->size->metrics.height == 0) {
    // No face height info, assume fixed width and get from a glyph.
    printf("  0x%02X, 0x%02X, %d };\n\n", first, last, table[0].height);
  } else {
    printf("  0x%02X, 0x%02X, %ld };\n\n", first, last,
           face->size->metrics.height >> 6);
  }
  printf("// Approx. %d bytes\n", bitmapOffset + (last - first + 1) * 7 + 7);
  // Size estimate is based on AVR struct and pointer sizes;
  // actual size may vary.

  FT_Done_FreeType(library);

  return 0;
}

char *to_utf8(uint64_t codepoint) {
  static char buf[7];
  memset(buf, 0, 7);
  if (codepoint <= 0x7F) {
    buf[0] = codepoint & 0xFF;
  } else if (codepoint <= 0x7FF) {
    buf[0] = 0b11000000 + (codepoint >> 6 & 0b11111);
    buf[1] = 0b10000000 + (codepoint & 0b111111);
  } else if (codepoint <= 0xFFFF) {
    buf[0] = 0b11100000 + (codepoint >> 12 & 0b1111);
    buf[1] = 0b10000000 + (codepoint >> 6 & 0b111111);
    buf[2] = 0b10000000 + (codepoint & 0b111111);
  } else if (codepoint <= 0x1FFFFF) {
    buf[0] = 0b11110000 + (codepoint >> 18 & 0b111);
    buf[1] = 0b10000000 + (codepoint >> 12 & 0b111111);
    buf[2] = 0b10000000 + (codepoint >> 6 & 0b111111);
    buf[3] = 0b10000000 + (codepoint & 0b111111);
  } else if (codepoint <= 0x3FFFFFF) {
    buf[0] = 0b11111000 + (codepoint >> 24 & 0b11);
    buf[1] = 0b10000000 + (codepoint >> 18 & 0b111111);
    buf[2] = 0b10000000 + (codepoint >> 12 & 0b111111);
    buf[3] = 0b10000000 + (codepoint >> 6 & 0b111111);
    buf[4] = 0b10000000 + (codepoint & 0b111111);
  } else {
    buf[0] = 0b11111100 + (codepoint >> 30 & 0b1);
    buf[1] = 0b10000000 + (codepoint >> 24 & 0b111111);
    buf[2] = 0b10000000 + (codepoint >> 18 & 0b111111);
    buf[3] = 0b10000000 + (codepoint >> 12 & 0b111111);
    buf[4] = 0b10000000 + (codepoint >> 6 & 0b111111);
    buf[5] = 0b10000000 + (codepoint & 0b111111);
  }
  return buf;
}

uint64_t read_utf8(char **input_ptr) {
  char *input = *input_ptr;
  uint64_t codepoint = 0;
  int n_bytes = 0;
  if ((*input & 0b11111100) == 0b11111100) {
    n_bytes = 5;
    codepoint = *input & 1;
  } else if ((*input & 0b11111000) == 0b11111000) {
    n_bytes = 4;
    codepoint = *input & 0b11;
  } else if ((*input & 0b11110000) == 0b11110000) {
    n_bytes = 3;
    codepoint = *input & 0b111;
  } else if ((*input & 0b11100000) == 0b11100000) {
    n_bytes = 2;
    codepoint = *input & 0b1111;
  } else if ((*input & 0b11000000) == 0b11000000) {
    n_bytes = 1;
    codepoint = *input & 0b11111;
  } else {
    n_bytes = 0;
    codepoint = *input;
  }
  input++;
  while (n_bytes > 0) {
    assert(*input);
    codepoint = (codepoint << 6) + (*input & 0b111111);
    input++;
    n_bytes--;
  }

  *input_ptr = input;

  return codepoint;
}

struct Remap *parse_remap(char *input) {
  if (!*input) { // end of string.
    return NULL;
  }
  uint64_t char_to_replace = read_utf8(&input);
  if (!*input) {
    fprintf(stderr, "Unexpected end of remap after: %s\n",
            to_utf8(char_to_replace));
    exit(1);
  }
  uint64_t replace_with = read_utf8(&input);

  struct Remap *result = malloc(sizeof(struct Remap));
  result->char_to_replace = char_to_replace;
  result->replace_with = replace_with;
  result->next = parse_remap(input);
  return result;
}

uint64_t remap_char(struct Remap *remap, uint64_t in) {
  if (!remap) {
    return in;
  }
  if (remap->char_to_replace == in) {
    return remap->replace_with;
  }
  return remap_char(remap->next, in);
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
