/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
 */

#include "Adafruit_GFX.h"
#include "glcdfont.c"
#ifdef __AVR__
  #include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
  #include <pgmspace.h>
#endif

// Many (but maybe not all) non-AVR board installs define macros
// for compatibility with existing PROGMEM-reading AVR code.
// Do our own checks and defines here for good measure...

#ifndef pgm_read_byte
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
 #define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
 #define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

// Pointers are a peculiar case...typically 16-bit on AVR boards,
// 32 bits elsewhere.  Try to accommodate both...

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
 #define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
 #define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

Adafruit_GFX::Adafruit_GFX(int16_t w, int16_t h):
WIDTH(w), HEIGHT(h)
{
    _width    = WIDTH;
    _height   = HEIGHT;
    rotation  = 0;
    cursor_y  = cursor_x    = 0;
    textsize  = 1;
    textcolor = textbgcolor = 0xFFFF;
    wrap      = true;
    _cp437    = false;
    gfxFont   = NULL;
}

// Bresenham's algorithm - thx wikpedia
void Adafruit_GFX::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            writePixel(y0, x0, color);
        } else {
            writePixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void Adafruit_GFX::startWrite(){
    // Overwrite in subclasses if desired!
}

void Adafruit_GFX::writePixel(int16_t x, int16_t y, uint16_t color){
    // Overwrite in subclasses if startWrite is defined!
    drawPixel(x, y, color);
}

// (x,y) is topmost point; if unsure, calling function
// should sort endpoints or call writeLine() instead
void Adafruit_GFX::writeFastVLine(int16_t x, int16_t y,
        int16_t h, uint16_t color) {
    // Overwrite in subclasses if startWrite is defined!
    // Can be just writeLine(x, y, x, y+h-1, color);
    // or writeFillRect(x, y, 1, h, color);
    drawFastVLine(x, y, h, color);
}

// (x,y) is leftmost point; if unsure, calling function
// should sort endpoints or call writeLine() instead
void Adafruit_GFX::writeFastHLine(int16_t x, int16_t y,
        int16_t w, uint16_t color) {
    // Overwrite in subclasses if startWrite is defined!
    // Example: writeLine(x, y, x+w-1, y, color);
    // or writeFillRect(x, y, w, 1, color);
    drawFastHLine(x, y, w, color);
}

void Adafruit_GFX::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color) {
    // Overwrite in subclasses if desired!
    fillRect(x,y,w,h,color);
}

void Adafruit_GFX::endWrite(){
    // Overwrite in subclasses if startWrite is defined!
}

// (x,y) is topmost point; if unsure, calling function
// should sort endpoints or call drawLine() instead
void Adafruit_GFX::drawFastVLine(int16_t x, int16_t y,
        int16_t h, uint16_t color) {
    // Update in subclasses if desired!
    startWrite();
    writeLine(x, y, x, y+h-1, color);
    endWrite();
}

// (x,y) is leftmost point; if unsure, calling function
// should sort endpoints or call drawLine() instead
void Adafruit_GFX::drawFastHLine(int16_t x, int16_t y,
        int16_t w, uint16_t color) {
    // Update in subclasses if desired!
    startWrite();
    writeLine(x, y, x+w-1, y, color);
    endWrite();
}

void Adafruit_GFX::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color) {
    // Update in subclasses if desired!
    startWrite();
    for (int16_t i=x; i<x+w; i++) {
        writeFastVLine(i, y, h, color);
    }
    endWrite();
}

void Adafruit_GFX::fillScreen(uint16_t color) {
    // Update in subclasses if desired!
    fillRect(0, 0, _width, _height, color);
}

void Adafruit_GFX::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        uint16_t color) {
    // Update in subclasses if desired!
    if(x0 == x1){
        if(y0 > y1) _swap_int16_t(y0, y1);
        drawFastVLine(x0, y0, y1 - y0 + 1, color);
    } else if(y0 == y1){
        if(x0 > x1) _swap_int16_t(x0, x1);
        drawFastHLine(x0, y0, x1 - x0 + 1, color);
    } else {
        startWrite();
        writeLine(x0, y0, x1, y1, color);
        endWrite();
    }
}


// Draw a circle outline
void Adafruit_GFX::drawCircle(int16_t x0, int16_t y0, int16_t r,
        uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    startWrite();
    writePixel(x0  , y0+r, color);
    writePixel(x0  , y0-r, color);
    writePixel(x0+r, y0  , color);
    writePixel(x0-r, y0  , color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        writePixel(x0 + x, y0 + y, color);
        writePixel(x0 - x, y0 + y, color);
        writePixel(x0 + x, y0 - y, color);
        writePixel(x0 - x, y0 - y, color);
        writePixel(x0 + y, y0 + x, color);
        writePixel(x0 - y, y0 + x, color);
        writePixel(x0 + y, y0 - x, color);
        writePixel(x0 - y, y0 - x, color);
    }
    endWrite();
}

void Adafruit_GFX::drawCircleHelper( int16_t x0, int16_t y0,
        int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) {
            writePixel(x0 + x, y0 + y, color);
            writePixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            writePixel(x0 + x, y0 - y, color);
            writePixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            writePixel(x0 - y, y0 + x, color);
            writePixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            writePixel(x0 - y, y0 - x, color);
            writePixel(x0 - x, y0 - y, color);
        }
    }
}

void Adafruit_GFX::fillCircle(int16_t x0, int16_t y0, int16_t r,
        uint16_t color) {
    startWrite();
    writeFastVLine(x0, y0-r, 2*r+1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
    endWrite();
}

// Used to do circles and roundrects
void Adafruit_GFX::fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
        uint8_t cornername, int16_t delta, uint16_t color) {

    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;

        if (cornername & 0x1) {
            writeFastVLine(x0+x, y0-y, 2*y+1+delta, color);
            writeFastVLine(x0+y, y0-x, 2*x+1+delta, color);
        }
        if (cornername & 0x2) {
            writeFastVLine(x0-x, y0-y, 2*y+1+delta, color);
            writeFastVLine(x0-y, y0-x, 2*x+1+delta, color);
        }
    }
}

// Draw a rectangle
void Adafruit_GFX::drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color) {
    startWrite();
    writeFastHLine(x, y, w, color);
    writeFastHLine(x, y+h-1, w, color);
    writeFastVLine(x, y, h, color);
    writeFastVLine(x+w-1, y, h, color);
    endWrite();
}

// Draw a rounded rectangle
void Adafruit_GFX::drawRoundRect(int16_t x, int16_t y, int16_t w,
        int16_t h, int16_t r, uint16_t color) {
    // smarter version
    startWrite();
    writeFastHLine(x+r  , y    , w-2*r, color); // Top
    writeFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
    writeFastVLine(x    , y+r  , h-2*r, color); // Left
    writeFastVLine(x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    drawCircleHelper(x+r    , y+r    , r, 1, color);
    drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
    drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
    drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
    endWrite();
}

// Fill a rounded rectangle
void Adafruit_GFX::fillRoundRect(int16_t x, int16_t y, int16_t w,
        int16_t h, int16_t r, uint16_t color) {
    // smarter version
    startWrite();
    writeFillRect(x+r, y, w-2*r, h, color);

    // draw four corners
    fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
    fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
    endWrite();
}

// Draw a triangle
void Adafruit_GFX::drawTriangle(int16_t x0, int16_t y0,
        int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
void Adafruit_GFX::fillTriangle(int16_t x0, int16_t y0,
        int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }

    startWrite();
    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        writeFastHLine(a, y0, b-a+1, color);
        endWrite();
        return;
    }

    int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
    int32_t
    sa   = 0,
    sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it

    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if(a > b) _swap_int16_t(a,b);
        writeFastHLine(a, y, b-a+1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if(a > b) _swap_int16_t(a,b);
        writeFastHLine(a, y, b-a+1, color);
    }
    endWrite();
}

// Draw a 1-bit image (bitmap) at the specified (x,y) position from the
// provided bitmap buffer (must be PROGMEM memory) using the specified
// foreground color (unset bits are transparent).
void Adafruit_GFX::drawBitmap(int16_t x, int16_t y,
        const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

    int16_t i, j, byteWidth = (w + 7) / 8;
    uint8_t byte = 0;

    startWrite();
    for(j=0; j<h; j++) {
        for(i=0; i<w; i++) {
            if(i & 7) byte <<= 1;
            else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
            if(byte & 0x80) writePixel(x+i, y+j, color);
        }
    }
    endWrite();
}

// Draw a 1-bit image (bitmap) at the specified (x,y) position from the
// provided bitmap buffer (must be PROGMEM memory) using the specified
// foreground (for set bits) and background (for clear bits) colors.
void Adafruit_GFX::drawBitmap(int16_t x, int16_t y,
        const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {

    int16_t i, j, byteWidth = (w + 7) / 8;
    uint8_t byte = 0;

    startWrite();
    for(j=0; j<h; j++) {
        for(i=0; i<w; i++ ) {
            if(i & 7) byte <<= 1;
            else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
            if(byte & 0x80) writePixel(x+i, y+j, color);
            else            writePixel(x+i, y+j, bg);
        }
    }
    endWrite();
}

// drawBitmap() variant for RAM-resident (not PROGMEM) bitmaps.
void Adafruit_GFX::drawBitmap(int16_t x, int16_t y,
        uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

    int16_t i, j, byteWidth = (w + 7) / 8;
    uint8_t byte = 0;

    startWrite();
    for(j=0; j<h; j++) {
        for(i=0; i<w; i++ ) {
            if(i & 7) byte <<= 1;
            else      byte   = bitmap[j * byteWidth + i / 8];
            if(byte & 0x80) writePixel(x+i, y+j, color);
        }
    }
    endWrite();
}

// drawBitmap() variant w/background for RAM-resident (not PROGMEM) bitmaps.
void Adafruit_GFX::drawBitmap(int16_t x, int16_t y,
        uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {

    int16_t i, j, byteWidth = (w + 7) / 8;
    uint8_t byte = 0;

    startWrite();
    for(j=0; j<h; j++) {
        for(i=0; i<w; i++ ) {
            if(i & 7) byte <<= 1;
            else      byte   = bitmap[j * byteWidth + i / 8];
            if(byte & 0x80) writePixel(x+i, y+j, color);
            else            writePixel(x+i, y+j, bg);
        }
    }
    endWrite();
}

//Draw XBitMap Files (*.xbm), exported from GIMP,
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
void Adafruit_GFX::drawXBitmap(int16_t x, int16_t y,
        const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

    int16_t i, j, byteWidth = (w + 7) / 8;
    uint8_t byte = 0;

    startWrite();
    for(j=0; j<h; j++) {
        for(i=0; i<w; i++ ) {
            if(i & 7) byte >>= 1;
            else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
            if(byte & 0x01) writePixel(x+i, y+j, color);
        }
    }
    endWrite();
}

// Draw a character
void Adafruit_GFX::drawChar(int16_t x, int16_t y, unsigned char c,
        uint16_t color, uint16_t bg, uint8_t size) {

    if(!gfxFont) { // 'Classic' built-in font

        if((x >= _width)            || // Clip right
                (y >= _height)           || // Clip bottom
                ((x + 6 * size - 1) < 0) || // Clip left
                ((y + 8 * size - 1) < 0))   // Clip top
            return;

        if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

        startWrite();
        for(int8_t i=0; i<6; i++ ) {
            uint8_t line;
            if(i < 5) line = pgm_read_byte(font+(c*5)+i);
            else      line = 0x0;
            for(int8_t j=0; j<8; j++, line >>= 1) {
                if(line & 0x1) {
                    if(size == 1) writePixel(x+i, y+j, color);
                    else          writeFillRect(x+(i*size), y+(j*size), size, size, color);
                } else if(bg != color) {
                    if(size == 1) writePixel(x+i, y+j, bg);
                    else          writeFillRect(x+i*size, y+j*size, size, size, bg);
                }
            }
        }
        endWrite();

    } else { // Custom font

        // Character is assumed previously filtered by write() to eliminate
        // newlines, returns, non-printable characters, etc.  Calling drawChar()
        // directly with 'bad' characters of font may cause mayhem!

        c -= pgm_read_byte(&gfxFont->first);
        GFXglyph *glyph  = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
        uint8_t  *bitmap = (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);

        uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
        uint8_t  w  = pgm_read_byte(&glyph->width),
                h  = pgm_read_byte(&glyph->height);
        int8_t   xo = pgm_read_byte(&glyph->xOffset),
                yo = pgm_read_byte(&glyph->yOffset);
        uint8_t  xx, yy, bits = 0, bit = 0;
        int16_t  xo16 = 0, yo16 = 0;

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

        startWrite();
        for(yy=0; yy<h; yy++) {
            for(xx=0; xx<w; xx++) {
                if(!(bit++ & 7)) {
                    bits = pgm_read_byte(&bitmap[bo++]);
                }
                if(bits & 0x80) {
                    if(size == 1) {
                        writePixel(x+xo+xx, y+yo+yy, color);
                    } else {
                        writeFillRect(x+(xo16+xx)*size, y+(yo16+yy)*size, size, size, color);
                    }
                }
                bits <<= 1;
            }
        }
        endWrite();

    } // End classic vs custom font
}

#if ARDUINO >= 100
size_t Adafruit_GFX::write(uint8_t c) {
#else
void Adafruit_GFX::write(uint8_t c) {
#endif
    if(!gfxFont) { // 'Classic' built-in font

        if(c == '\n') {
            cursor_y += textsize*8;
            cursor_x  = 0;
        } else if(c == '\r') {
            // skip em
        } else {
            if(wrap && ((cursor_x + textsize * 6) >= _width)) { // Heading off edge?
                cursor_x  = 0;            // Reset x to zero
                cursor_y += textsize * 8; // Advance y one line
            }
            drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
            cursor_x += textsize * 6;
        }

    } else { // Custom font

        if(c == '\n') {
            cursor_x  = 0;
            cursor_y += (int16_t)textsize *
                    (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        } else if(c != '\r') {
            uint8_t first = pgm_read_byte(&gfxFont->first);
            if((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
                uint8_t   c2    = c - pgm_read_byte(&gfxFont->first);
                GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c2]);
                uint8_t   w     = pgm_read_byte(&glyph->width),
                        h     = pgm_read_byte(&glyph->height);
                if((w > 0) && (h > 0)) { // Is there an associated bitmap?
                    int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
                    if(wrap && ((cursor_x + textsize * (xo + w)) >= _width)) {
                        // Drawing character would go off right edge; wrap to new line
                        cursor_x  = 0;
                        cursor_y += (int16_t)textsize *
                                (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
                    }
                    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
                }
                cursor_x += pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize;
            }
        }

    }
#if ARDUINO >= 100
    return 1;
#endif
}

void Adafruit_GFX::setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
}

int16_t Adafruit_GFX::getCursorX(void) const {
    return cursor_x;
}

int16_t Adafruit_GFX::getCursorY(void) const {
    return cursor_y;
}

void Adafruit_GFX::setTextSize(uint8_t s) {
    textsize = (s > 0) ? s : 1;
}

void Adafruit_GFX::setTextColor(uint16_t c) {
    // For 'transparent' background, we'll set the bg
    // to the same as fg instead of using a flag
    textcolor = textbgcolor = c;
}

void Adafruit_GFX::setTextColor(uint16_t c, uint16_t b) {
    textcolor   = c;
    textbgcolor = b;
}

void Adafruit_GFX::setTextWrap(boolean w) {
    wrap = w;
}

uint8_t Adafruit_GFX::getRotation(void) const {
    return rotation;
}

void Adafruit_GFX::setRotation(uint8_t x) {
    rotation = (x & 3);
    switch(rotation) {
        case 0:
        case 2:
            _width  = WIDTH;
            _height = HEIGHT;
            break;
        case 1:
        case 3:
            _width  = HEIGHT;
            _height = WIDTH;
            break;
    }
}

// Enable (or disable) Code Page 437-compatible charset.
// There was an error in glcdfont.c for the longest time -- one character
// (#176, the 'light shade' block) was missing -- this threw off the index
// of every character that followed it.  But a TON of code has been written
// with the erroneous character indices.  By default, the library uses the
// original 'wrong' behavior and old sketches will still work.  Pass 'true'
// to this function to use correct CP437 character values in your code.
void Adafruit_GFX::cp437(boolean x) {
    _cp437 = x;
}

void Adafruit_GFX::setFont(const GFXfont *f) {
    if(f) {          // Font struct pointer passed in?
        if(!gfxFont) { // And no current font struct?
            // Switching from classic to new font behavior.
            // Move cursor pos down 6 pixels so it's on baseline.
            cursor_y += 6;
        }
    } else if(gfxFont) { // NULL passed.  Current font struct defined?
        // Switching from new to classic font behavior.
        // Move cursor pos up 6 pixels so it's at top-left of char.
        cursor_y -= 6;
    }
    gfxFont = (GFXfont *)f;
}

// Pass string and a cursor position, returns UL corner and W,H.
void Adafruit_GFX::getTextBounds(char *str, int16_t x, int16_t y,
        int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
    uint8_t c; // Current character

    *x1 = x;
    *y1 = y;
    *w  = *h = 0;

    if(gfxFont) {

        GFXglyph *glyph;
        uint8_t   first = pgm_read_byte(&gfxFont->first),
                last  = pgm_read_byte(&gfxFont->last),
                gw, gh, xa;
        int8_t    xo, yo;
        int16_t   minx = _width, miny = _height, maxx = -1, maxy = -1,
                gx1, gy1, gx2, gy2, ts = (int16_t)textsize,
                ya = ts * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);

        while((c = *str++)) {
            if(c != '\n') { // Not a newline
                if(c != '\r') { // Not a carriage return, is normal char
                    if((c >= first) && (c <= last)) { // Char present in current font
                        c    -= first;
                        glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
                        gw    = pgm_read_byte(&glyph->width);
                        gh    = pgm_read_byte(&glyph->height);
                        xa    = pgm_read_byte(&glyph->xAdvance);
                        xo    = pgm_read_byte(&glyph->xOffset);
                        yo    = pgm_read_byte(&glyph->yOffset);
                        if(wrap && ((x + (((int16_t)xo + gw) * ts)) >= _width)) {
                            // Line wrap
                            x  = 0;  // Reset x to 0
                            y += ya; // Advance y by 1 line
                        }
                        gx1 = x   + xo * ts;
                        gy1 = y   + yo * ts;
                        gx2 = gx1 + gw * ts - 1;
                        gy2 = gy1 + gh * ts - 1;
                        if(gx1 < minx) minx = gx1;
                        if(gy1 < miny) miny = gy1;
                        if(gx2 > maxx) maxx = gx2;
                        if(gy2 > maxy) maxy = gy2;
                        x += xa * ts;
                    }
                } // Carriage return = do nothing
            } else { // Newline
                x  = 0;  // Reset x
                y += ya; // Advance y by 1 line
            }
        }
        // End of string
        *x1 = minx;
        *y1 = miny;
        if(maxx >= minx) *w  = maxx - minx + 1;
        if(maxy >= miny) *h  = maxy - miny + 1;

    } else { // Default font

        uint16_t lineWidth = 0, maxWidth = 0; // Width of current, all lines

        while((c = *str++)) {
            if(c != '\n') { // Not a newline
                if(c != '\r') { // Not a carriage return, is normal char
                    if(wrap && ((x + textsize * 6) >= _width)) {
                        x  = 0;            // Reset x to 0
                        y += textsize * 8; // Advance y by 1 line
                        if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
                        lineWidth  = textsize * 6; // First char on new line
                    } else { // No line wrap, just keep incrementing X
                        lineWidth += textsize * 6; // Includes interchar x gap
                        if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
                    }
                } // Carriage return = do nothing
            } else { // Newline
                x  = 0;            // Reset x to 0
                y += textsize * 8; // Advance y by 1 line
                if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
                lineWidth = 0;     // Reset lineWidth for new line
            }
        }
        // End of string
        if(lineWidth) y += textsize * 8; // Add height of last (or only) line
        if(lineWidth > maxWidth) maxWidth = lineWidth; // Is the last or only line the widest?
        *w = maxWidth - 1;               // Don't include last interchar x gap
        *h = y - *y1;

    } // End classic vs custom font
}

// Same as above, but for PROGMEM strings
void Adafruit_GFX::getTextBounds(const __FlashStringHelper *str,
        int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
    uint8_t *s = (uint8_t *)str, c;

    *x1 = x;
    *y1 = y;
    *w  = *h = 0;

    if(gfxFont) {

        GFXglyph *glyph;
        uint8_t   first = pgm_read_byte(&gfxFont->first),
                last  = pgm_read_byte(&gfxFont->last),
                gw, gh, xa;
        int8_t    xo, yo;
        int16_t   minx = _width, miny = _height, maxx = -1, maxy = -1,
                gx1, gy1, gx2, gy2, ts = (int16_t)textsize,
                ya = ts * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);

        while((c = pgm_read_byte(s++))) {
            if(c != '\n') { // Not a newline
                if(c != '\r') { // Not a carriage return, is normal char
                    if((c >= first) && (c <= last)) { // Char present in current font
                        c    -= first;
                        glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
                        gw    = pgm_read_byte(&glyph->width);
                        gh    = pgm_read_byte(&glyph->height);
                        xa    = pgm_read_byte(&glyph->xAdvance);
                        xo    = pgm_read_byte(&glyph->xOffset);
                        yo    = pgm_read_byte(&glyph->yOffset);
                        if(wrap && ((x + (((int16_t)xo + gw) * ts)) >= _width)) {
                            // Line wrap
                            x  = 0;  // Reset x to 0
                            y += ya; // Advance y by 1 line
                        }
                        gx1 = x   + xo * ts;
                        gy1 = y   + yo * ts;
                        gx2 = gx1 + gw * ts - 1;
                        gy2 = gy1 + gh * ts - 1;
                        if(gx1 < minx) minx = gx1;
                        if(gy1 < miny) miny = gy1;
                        if(gx2 > maxx) maxx = gx2;
                        if(gy2 > maxy) maxy = gy2;
                        x += xa * ts;
                    }
                } // Carriage return = do nothing
            } else { // Newline
                x  = 0;  // Reset x
                y += ya; // Advance y by 1 line
            }
        }
        // End of string
        *x1 = minx;
        *y1 = miny;
        if(maxx >= minx) *w  = maxx - minx + 1;
        if(maxy >= miny) *h  = maxy - miny + 1;

    } else { // Default font

        uint16_t lineWidth = 0, maxWidth = 0; // Width of current, all lines

        while((c = pgm_read_byte(s++))) {
            if(c != '\n') { // Not a newline
                if(c != '\r') { // Not a carriage return, is normal char
                    if(wrap && ((x + textsize * 6) >= _width)) {
                        x  = 0;            // Reset x to 0
                        y += textsize * 8; // Advance y by 1 line
                        if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
                        lineWidth  = textsize * 6; // First char on new line
                    } else { // No line wrap, just keep incrementing X
                        lineWidth += textsize * 6; // Includes interchar x gap
                    }
                } // Carriage return = do nothing
            } else { // Newline
                x  = 0;            // Reset x to 0
                y += textsize * 8; // Advance y by 1 line
                if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
                lineWidth = 0;     // Reset lineWidth for new line
            }
        }
        // End of string
        if(lineWidth) y += textsize * 8; // Add height of last (or only) line
        if(lineWidth > maxWidth) maxWidth = lineWidth; // Is the last or only line the widest?
        *w = maxWidth - 1;               // Don't include last interchar x gap
        *h = y - *y1;

    } // End classic vs custom font
}

// Return the size of the display (per current rotation)
int16_t Adafruit_GFX::width(void) const {
    return _width;
}

int16_t Adafruit_GFX::height(void) const {
    return _height;
}

void Adafruit_GFX::invertDisplay(boolean i) {
    // Do nothing, must be subclassed if supported by hardware
}

/***************************************************************************/
// code for the GFX button UI element

Adafruit_GFX_Button::Adafruit_GFX_Button(void) {
  _gfx = 0;
}

// Classic initButton() function: pass center & size
void Adafruit_GFX_Button::initButton(
 Adafruit_GFX *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h,
 uint16_t outline, uint16_t fill, uint16_t textcolor,
 char *label, uint8_t textsize)
{
  // Tweak arguments and pass to the newer initButtonUL() function...
  initButtonUL(gfx, x - (w / 2), y - (h / 2), w, h, outline, fill,
    textcolor, label, textsize);
}

// Newer function instead accepts upper-left corner & size
void Adafruit_GFX_Button::initButtonUL(
 Adafruit_GFX *gfx, int16_t x1, int16_t y1, uint16_t w, uint16_t h,
 uint16_t outline, uint16_t fill, uint16_t textcolor,
 char *label, uint8_t textsize)
{
  _x1           = x1;
  _y1           = y1;
  _w            = w;
  _h            = h;
  _outlinecolor = outline;
  _fillcolor    = fill;
  _textcolor    = textcolor;
  _textsize     = textsize;
  _gfx          = gfx;
  strncpy(_label, label, 9);
}

void Adafruit_GFX_Button::drawButton(boolean inverted) {
  uint16_t fill, outline, text;

  if(!inverted) {
    fill    = _fillcolor;
    outline = _outlinecolor;
    text    = _textcolor;
  } else {
    fill    = _textcolor;
    outline = _outlinecolor;
    text    = _fillcolor;
  }

  uint8_t r = min(_w, _h) / 4; // Corner radius
  _gfx->fillRoundRect(_x1, _y1, _w, _h, r, fill);
  _gfx->drawRoundRect(_x1, _y1, _w, _h, r, outline);

  _gfx->setCursor(_x1 + (_w/2) - (strlen(_label) * 3 * _textsize),
    _y1 + (_h/2) - (4 * _textsize));
  _gfx->setTextColor(text);
  _gfx->setTextSize(_textsize);
  _gfx->print(_label);
}

boolean Adafruit_GFX_Button::contains(int16_t x, int16_t y) {
  return ((x >= _x1) && (x < (_x1 + _w)) &&
          (y >= _y1) && (y < (_y1 + _h)));
}

void Adafruit_GFX_Button::press(boolean p) {
  laststate = currstate;
  currstate = p;
}

boolean Adafruit_GFX_Button::isPressed() { return currstate; }
boolean Adafruit_GFX_Button::justPressed() { return (currstate && !laststate); }
boolean Adafruit_GFX_Button::justReleased() { return (!currstate && laststate); }

// -------------------------------------------------------------------------

// GFXcanvas1 and GFXcanvas16 (currently a WIP, don't get too comfy with the
// implementation) provide 1- and 16-bit offscreen canvases, the address of
// which can be passed to drawBitmap() or pushColors() (the latter appears
// to only be in Adafruit_TFTLCD at this time).  This is here mostly to
// help with the recently-added proportionally-spaced fonts; adds a way to
// refresh a section of the screen without a massive flickering clear-and-
// redraw...but maybe you'll find other uses too.  VERY RAM-intensive, since
// the buffer is in MCU memory and not the display driver...GXFcanvas1 might
// be minimally useful on an Uno-class board, but this and GFXcanvas16 are
// much more likely to require at least a Mega or various recent ARM-type
// boards (recommended, as the text+bitmap draw can be pokey).  GFXcanvas1
// requires 1 bit per pixel (rounded up to nearest byte per scanline),
// GFXcanvas16 requires 2 bytes per pixel (no scanline pad).
// NOT EXTENSIVELY TESTED YET.  MAY CONTAIN WORST BUGS KNOWN TO HUMANKIND.

GFXcanvas1::GFXcanvas1(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {
    uint16_t bytes = ((w + 7) / 8) * h;
    if((buffer = (uint8_t *)malloc(bytes))) {
        memset(buffer, 0, bytes);
    }
}

GFXcanvas1::~GFXcanvas1(void) {
    if(buffer) free(buffer);
}

uint8_t* GFXcanvas1::getBuffer(void) {
    return buffer;
}

void GFXcanvas1::drawPixel(int16_t x, int16_t y, uint16_t color) {
    // Bitmask tables of 0x80>>X and ~(0x80>>X), because X>>Y is slow on AVR
    static const uint8_t PROGMEM
    GFXsetBit[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 },
    GFXclrBit[] = { 0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0xFE };

    if(buffer) {
        if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;

        int16_t t;
        switch(rotation) {
            case 1:
                t = x;
                x = WIDTH  - 1 - y;
                y = t;
                break;
            case 2:
                x = WIDTH  - 1 - x;
                y = HEIGHT - 1 - y;
                break;
            case 3:
                t = x;
                x = y;
                y = HEIGHT - 1 - t;
                break;
        }

        uint8_t *ptr = &buffer[(x / 8) + y * ((WIDTH + 7) / 8)];
        if(color) *ptr |= pgm_read_byte(&GFXsetBit[x & 7]);
        else      *ptr &= pgm_read_byte(&GFXclrBit[x & 7]);
    }
}

void GFXcanvas1::fillScreen(uint16_t color) {
    if(buffer) {
        uint16_t bytes = ((WIDTH + 7) / 8) * HEIGHT;
        memset(buffer, color ? 0xFF : 0x00, bytes);
    }
}

GFXcanvas16::GFXcanvas16(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {
    uint16_t bytes = w * h * 2;
    if((buffer = (uint16_t *)malloc(bytes))) {
        memset(buffer, 0, bytes);
    }
}

GFXcanvas16::~GFXcanvas16(void) {
    if(buffer) free(buffer);
}

uint16_t* GFXcanvas16::getBuffer(void) {
    return buffer;
}

void GFXcanvas16::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if(buffer) {
        if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;

        int16_t t;
        switch(rotation) {
            case 1:
                t = x;
                x = WIDTH  - 1 - y;
                y = t;
                break;
            case 2:
                x = WIDTH  - 1 - x;
                y = HEIGHT - 1 - y;
                break;
            case 3:
                t = x;
                x = y;
                y = HEIGHT - 1 - t;
                break;
        }

        buffer[x + y * WIDTH] = color;
    }
}

void GFXcanvas16::fillScreen(uint16_t color) {
    if(buffer) {
        uint8_t hi = color >> 8, lo = color & 0xFF;
        if(hi == lo) {
            memset(buffer, lo, WIDTH * HEIGHT * 2);
        } else {
            uint16_t i, pixels = WIDTH * HEIGHT;
            for(i=0; i<pixels; i++) buffer[i] = color;
        }
    }
}

