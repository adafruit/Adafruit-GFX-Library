/*!
 * @file GFXcanvas1.cpp
 *
 * Part of Adafruit's GFX graphics library. Provides a 1-bit in-RAM
 * offscreen drawing canvas.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Limor "ladyada" Fried for Adafruit Industries,
 * with contributions from the open source community.
 *
 * BSD license, all text here must be included in any redistribution.
 */

#include "GFXcanvas1.h"

#ifdef __AVR__
// Bitmask tables of 0x80>>X and ~(0x80>>X), because X>>Y is slow on AVR
const uint8_t PROGMEM GFXcanvas1::GFXsetBit[] = {0x80, 0x40, 0x20, 0x10,
                                                 0x08, 0x04, 0x02, 0x01};
const uint8_t PROGMEM GFXcanvas1::GFXclrBit[] = {0x7F, 0xBF, 0xDF, 0xEF,
                                                 0xF7, 0xFB, 0xFD, 0xFE};
#endif

/*!
  @brief  Instatiate a GFX 1-bit canvas context for graphics. All pixels
          are initialized to 0.
  @param  w  Canvas width in pixels.
  @param  h  Canvas height in pixels.
*/
GFXcanvas1::GFXcanvas1(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {
  buffer = (uint8_t *)calloc(((w + 7) / 8) * h, 1); // Rows are byte aligned
}

/*!
  @brief  Delete canvas, free memory.
*/
GFXcanvas1::~GFXcanvas1(void) {
  if (buffer)
    free(buffer);
}

/*!
  @brief  Draw a pixel to the canvas framebuffer, applying clipping and/or
          rotation as appropriate.
  @param  x      Pixel column (horizonal pos).
  @param  y      Pixel row (vertical pos).
  @param  color  Binary (on or off) color to set pixel.
*/
void GFXcanvas1::drawPixel(int16_t x, int16_t y, uint16_t color) {
  // Validate canvas buffer is valid & point is in bounds
  if (buffer && (x >= 0) && (x < _width) && (y >= 0) && (y < _height)) {
    // Apply 'soft' rotation if needed
    int16_t t;
    switch (rotation) {
    case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
    case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
    }

    // (x, y) now known in-bounds and in native coordinate system.
    uint8_t *ptr = &buffer[(x / 8) + y * ((WIDTH + 7) / 8)];
#ifdef __AVR__
    if (color)
      *ptr |= pgm_read_byte(&GFXsetBit[x & 7]);
    else
      *ptr &= pgm_read_byte(&GFXclrBit[x & 7]);
#else
    if (color)
      *ptr |= 0x80 >> (x & 7);
    else
      *ptr &= ~(0x80 >> (x & 7));
#endif
  }
}

/*!

  @brief   Retrieve pixel value from the canvas framebuffer, applying
           clipping and/or rotation as appropriate.
  @param   x  Pixel column (horizonal pos).
  @param   y  Pixel row (vertical pos).
  @return  Pixel value (0 or 1) if coordinate is in-bounds. 0 if out-of-
           bounds or if canvas buffer previously failed to allocate.
*/
bool GFXcanvas1::getPixel(int16_t x, int16_t y) const {
  if (buffer && (x >= 0) && (x < _width) && (y >= 0) && (y < _height)) {
    int16_t t;
    switch (rotation) {
    case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
    case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
    }
    // (x, y) now known in-bounds and in native coordinate system.
    return getRawPixel(x, y);
  }
  return 0; // Coord out of bounds, or canvas buffer is invalid
}

/*!
  @brief   Retrieve pixel value from the canvas framebuffer, where inputs
           are known valid in-bounds and in native unrotated coordinates.
           NO CLIPPING OR VALIDATION OF CANVAS BUFFER IS PERFORMED,
           calling function must be AWARE and WELL-BEHAVED.
  @param   x  Pixel column (horizonal pos).
  @param   y  Pixel row (vertical pos).
  @return  Pixel value (0 or 1).
*/
bool GFXcanvas1::getRawPixel(int16_t x, int16_t y) const {
  uint8_t *ptr = &buffer[(x / 8) + y * ((WIDTH + 7) / 8)];
#ifdef __AVR__
  return ((*ptr) & pgm_read_byte(&GFXsetBit[x & 7])) != 0;
#else
  return ((*ptr) & (0x80 >> (x & 7))) != 0;
#endif
}

/*!
  @brief  Fill canvas completely with one color.
  @param  color  Binary (on or off) color to fill with.
*/
void GFXcanvas1::fillScreen(uint16_t color) {
  if (buffer) {
    memset(buffer, color ? 0xFF : 0x00, ((WIDTH + 7) / 8) * HEIGHT);
  }
}

/*!
  @brief  Draw vertical line. May allow for optimizations compared to the
          generalized drawLine().
  @param  x      Column (horizontal position of start and end points)
  @param  y      Row (vertical pos) of start point.
  @param  h      Length of vertical line to be drawn, including first point.
  @param  color  Binary (on or off) color to draw.
*/
void GFXcanvas1::drawFastVLine(int16_t x, int16_t y, int16_t h,
                               uint16_t color) {

  if (h < 0) { // Convert negative heights to positive equivalent
    h = -h;
    y -= h - 1;
    if (y < 0) {
      h += y;
      y = 0;
    }
  }

  // Edge rejection (no-draw if totally off canvas)
  if ((x >= 0) && (x < _width) && (y < _height) && ((y + h - 1) >= 0)) {
    if (y < 0) { // Clip top
      h += y;
      y = 0;
    }
    if (y + h > height()) { // Clip bottom
      h = height() - y;
    }

    int16_t t;
    switch (rotation) {
    case 0:
      drawFastRawVLine(x, y, h, color);
      break;
    case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      x -= h - 1;
      drawFastRawHLine(x, y, h, color);
      break;
    case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      y -= h - 1;
      drawFastRawVLine(x, y, h, color);
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      drawFastRawHLine(x, y, h, color);
      break;
    }
  }
}

/*!
  @brief  Draw horizontal line. May allow for optimizations compared to the
          generalized drawLine().
  @param  x      Column (horizontal position) of start point.
  @param  y      Row (vertical position of start and end points)
  @param  w      Width of vertical line to be drawn, including first point.
  @param  color  Binary (on or off) color to draw.
*/
void GFXcanvas1::drawFastHLine(int16_t x, int16_t y, int16_t w,
                               uint16_t color) {
  if (w < 0) { // Convert negative widths to positive equivalent
    w *= -1;
    x -= w - 1;
    if (x < 0) {
      w += x;
      x = 0;
    }
  }

  // Edge rejection (no-draw if totally off canvas)
  if ((y >= 0) && (y < _height) && (x < _width) && ((x + w - 1) >= 0)) {
    if (x < 0) { // Clip left
      w += x;
      x = 0;
    }
    if (x + w >= width()) { // Clip right
      w = width() - x;
    }

    int16_t t;
    switch (rotation) {
    case 0:
      drawFastRawHLine(x, y, w, color);
      break;
    case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      drawFastRawVLine(x, y, w, color);
      break;
    case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      x -= w - 1;
      drawFastRawHLine(x, y, w, color);
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      y -= w - 1;
      drawFastRawVLine(x, y, w, color);
      break;
    }
  }
}

/*!
  @brief  Draw vertical line into 'raw' canvas buffer. Inputs are assumed
          valid and in native coordinates; NO CLIPPING, ROTATION OR BUFFER
          VALIDATION IS PERFORMED.
  @param  x      Column (horizontal position of start and end points)
  @param  y      Row (vertical pos) of start point.
  @param  h      Length of vertical line to be drawn, including first point.
  @param  color  Binary (on or off) color to draw.
*/
void GFXcanvas1::drawFastRawVLine(int16_t x, int16_t y, int16_t h,
                                  uint16_t color) {
  int16_t row_bytes = ((WIDTH + 7) / 8); // Row-to-row increment
  uint8_t *ptr = &buffer[(x / 8) + y * row_bytes];

  if (color > 0) {
#ifdef __AVR__
    uint8_t bit_mask = pgm_read_byte(&GFXsetBit[x & 7]);
#else
    uint8_t bit_mask = (0x80 >> (x & 7));
#endif
    while (h--) {
      *ptr |= bit_mask;
      ptr += row_bytes;
    }
  } else {
#ifdef __AVR__
    uint8_t bit_mask = pgm_read_byte(&GFXclrBit[x & 7]);
#else
    uint8_t bit_mask = ~(0x80 >> (x & 7));
#endif
    while (h--) {
      *ptr &= bit_mask;
      ptr += row_bytes;
    }
  }
}

/*!
  @brief  Draw horizontal line. May allow for optimizations compared to the
          generalized drawLine().
  @brief  Draw horizontal line into 'raw' canvas buffer. Inputs are assumed
          valid and in native coordinates; NO CLIPPING, ROTATION OR BUFFER
          VALIDATION IS PERFORMED.
  @param  x      Column (horizontal position) of start point.
  @param  y      Row (vertical position of start and end points)
  @param  w      Width of vertical line to be drawn, including first point.
  @param  color  Binary (on or off) color to draw.
*/
void GFXcanvas1::drawFastRawHLine(int16_t x, int16_t y, int16_t w,
                                  uint16_t color) {
  int16_t rowBytes = ((WIDTH + 7) / 8);
  uint8_t *ptr = &buffer[(x / 8) + y * rowBytes];
  int16_t remainingWidthBits = w;

  if ((x & 7) > 0) { // Partial fill on first byte?
    // create bit mask for first byte
    uint8_t startByteBitMask = 0x00;
    for (int8_t i = (x & 7); ((i < 8) && (w > 0)); i++) {
#ifdef __AVR__
      startByteBitMask |= pgm_read_byte(&GFXsetBit[i]);
#else
      startByteBitMask |= (0x80 >> i);
#endif
      w--;
    }
    if (color > 0) {
      *ptr |= startByteBitMask;
    } else {
      *ptr &= ~startByteBitMask;
    }
    ptr++;
  }

  if (w > 0) { // Handle any remaining pixels, first is now known byte-aligned
    int16_t remainingWholeBytes = w / 8;
    int16_t lastByteBits = w & 7;

    memset(ptr, (color > 0) ? 0xFF : 0x00, remainingWholeBytes);

    if (lastByteBits > 0) {
      uint8_t lastByteBitMask = 0x00;
      for (size_t i = 0; i < lastByteBits; i++) {
#ifdef __AVR__
        lastByteBitMask |= pgm_read_byte(&GFXsetBit[i]);
#else
        lastByteBitMask |= (0x80 >> i);
#endif
      }
      ptr += remainingWholeBytes;

      if (color > 0) {
        *ptr |= lastByteBitMask;
      } else {
        *ptr &= ~lastByteBitMask;
      }
    }
  }
}
