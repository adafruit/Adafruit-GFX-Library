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
  // Rows are byte-aligned. If allocation fails, no special action is
  // performed here. Drawing functions check for valid buffer.
  pixbuf = (uint8_t *)calloc(((w + 7) / 8) * h, 1);
}

/*!
  @brief  GFXcanvas1 destructor. Frees memory associated with canvas.
*/
GFXcanvas1::~GFXcanvas1(void) {
  if (pixbuf) {
    free(pixbuf);
  }
};

/*!
  @brief  Draw a pixel to the canvas framebuffer, applying clipping and/or
          rotation as appropriate.
  @param  x      Pixel column (horizonal pos).
  @param  y      Pixel row (vertical pos).
  @param  color  Pixel draw color. Range depends on canvas depth.
*/
void GFXcanvas1::drawPixel(int16_t x, int16_t y, uint16_t color) {
  // Validate canvas buffer is valid & point is in bounds
  if (pixbuf && (x >= 0) && (x < _width) && (y >= 0) && (y < _height)) {
    // Point is valid. Apply 'soft' rotation if needed.
    int16_t t;
    switch (rotation) { // 0 can be ignored
    case 1: // 90 degrees CW
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
    case 2: // 180 degrees
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3: // 90 degrees CCW
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
    }

    // (x, y) now known in-bounds and in canvas-native coordinate system.
    drawPixelRaw(x, y, color);
  }
}

/*!
  @brief  Draw a pixel to the canvas framebuffer. NO CLIPPING, ROTATION OR
          VALIDATION OF CANVAS BUFFER IS PERFORMED, calling function must be
          AWARE and WELL-BEHAVED. This is a protected function intended for
          related class code, not user-facing.
  @param  x      Pixel column (horizonal pos).
  @param  y      Pixel row (vertical pos).
  @param  color  Pixel draw color; thresholded to 0 or 1 (any nonzero input).
*/
void GFXcanvas1::drawPixelRaw(int16_t x, int16_t y, uint16_t color) {
  uint8_t *ptr = &pixbuf[(x / 8) + y * ((WIDTH + 7) / 8)];
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

/*!
  @brief   Retrieve pixel value from the canvas framebuffer, applying
           clipping and/or rotation as appropriate.
  @param   x  Pixel column (horizonal pos).
  @param   y  Pixel row (vertical pos).
  @return  Pixel value if coordinate is in-bounds. 0 if out-of-bounds
           or if canvas buffer previously failed to allocate.
*/
bool GFXcanvas1::getPixel(int16_t x, int16_t y) const {
  // Validate canvas buffer is valid & point is in bounds
  if (pixbuf && (x >= 0) && (x < _width) && (y >= 0) && (y < _height)) {
    // Point is valid. Apply 'soft' rotation if needed.
    int16_t t;
    switch (rotation) { // 0 can be ignored
    case 1: // 90 degrees CW
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
    case 2: // 180 degrees
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3: // 90 degrees CCW
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
    }
    // (x, y) now known in-bounds and in canvas-native coordinate system.
    return getPixelRaw(x, y);
  }
  return 0; // Coord out of bounds, or canvas buffer is invalid
}

/*!
  @brief   Retrieve pixel value from the canvas framebuffer, where inputs
           are known valid in-bounds and in native unrotated coordinates.
           NO CLIPPING, ROTATION OR VALIDATION OF CANVAS BUFFER IS PERFORMED,
           calling function must be AWARE and WELL-BEHAVED. This is a
           protected function intended for driver code, not user-facing.
  @param   x  Pixel column (horizonal pos).
  @param   y  Pixel row (vertical pos).
  @return  Pixel value (0 or 1).
*/
bool GFXcanvas1::getPixelRaw(int16_t x, int16_t y) const {
  uint8_t *ptr = &pixbuf[(x / 8) + y * ((WIDTH + 7) / 8)];
#ifdef __AVR__
  return ((*ptr) & pgm_read_byte(&GFXsetBit[x & 7])) != 0;
#else
  return ((*ptr) & (0x80 >> (x & 7))) != 0;
#endif
}

/*!
  @brief  Fill canvas completely with one color.
  @param  color  Canvas fill color; thresholded to 0 or 1 (any nonzero input).
*/
void GFXcanvas1::fillScreen(uint16_t color) {
  if (pixbuf) {
    memset(pixbuf, color ? 0xFF : 0x00, ((WIDTH + 7) / 8) * HEIGHT);
  }
}

/*!
  @brief  Draw vertical line. May allow for optimizations compared to the
          generalized drawLine() function.
  @param  x      Column (horizontal position of start and end points)
  @param  y      Row (vertical pos) of start point.
  @param  h      Length of vertical line to be drawn, including first point.
  @param  color  Line draw color; thresholded to 0 or 1 (any nonzero input).
*/
void GFXcanvas1::drawFastVLine(int16_t x, int16_t y, int16_t h,
                               uint16_t color) {
  if (pixbuf) {
    if (h < 0) { // Convert negative heights to positive equivalent
      h = -h;
      y -= h - 1;
      if (y < 0) {
        h += y;
        y = 0;
      }
    }

    // Reject off-canvas and zero-height lines
    if ((h > 0) && (x >= 0) && (x < _width) && (y < _height) &&
        ((y + h - 1) >= 0)) {
      if (y < 0) { // Clip top
        h += y;
        y = 0;
      }
      if ((y + h) > _height) { // Clip bottom
        h = _height - y;
      }

      switch (rotation) {
      case 0: // No rotation
        drawFastVLineRaw(x, y, h, color);
        break;
      case 1: // 90 degrees CW
        drawFastHLineRaw(WIDTH - y - h, x, h, color);
        break;
      case 2: // 180 degrees
        drawFastVLineRaw(WIDTH - 1 - x, HEIGHT - y - h, h, color);
        break;
      case 3: // 90 degrees CCW
        drawFastHLineRaw(y, HEIGHT - 1 - x, h, color);
        break;
      }
    }
  }
}

/*!
  @brief  Draw vertical line into 'raw' canvas buffer. Inputs are assumed
          valid and in native coordinates. NO CLIPPING, ROTATION OR
          VALIDATION OF CANVAS BUFFER IS PERFORMED, calling function must
          be AWARE and WELL-BEHAVED. This is a protected function intended
          for related class code, not user-facing.
  @param  x      Column (horizontal position of start and end points)
  @param  y      Row (vertical pos) of start point.
  @param  h      Length of vertical line to be drawn, including first point.
  @param  color  Line draw color; thresholded to 0 or 1 (any nonzero input).
*/
void GFXcanvas1::drawFastVLineRaw(int16_t x, int16_t y, int16_t h,
                                  uint16_t color) {
  int16_t row_bytes = ((WIDTH + 7) / 8); // Row-to-row increment
  uint8_t *ptr = &pixbuf[(x / 8) + y * row_bytes];

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
          generalized drawLine() function.
  @param  x      Column (horizontal position) of start point.
  @param  y      Row (vertical position of start and end points)
  @param  w      Width of vertical line to be drawn, including first point.
  @param  color  Line draw color; thresholded to 0 or 1 (any nonzero input).
*/
void GFXcanvas1::drawFastHLine(int16_t x, int16_t y, int16_t w,
                               uint16_t color) {
  if (pixbuf) {
    if (w < 0) { // Convert negative widths to positive equivalent
      w = -w;
      x -= w - 1;
      if (x < 0) {
        w += x;
        x = 0;
      }
    }

    // Reject off-canvas and zero-width lines
    if ((w > 0) && (y >= 0) && (y < _height) && (x < _width) &&
        ((x + w - 1) >= 0)) {
      if (x < 0) { // Clip left
        w += x;
        x = 0;
      }
      if ((x + w) >= _width) { // Clip right
        w = _width - x;
      }

      switch (rotation) {
      case 0: // No rotation
        drawFastHLineRaw(x, y, w, color);
        break;
      case 1: // 90 degrees CW
        drawFastVLineRaw(WIDTH - 1 - y, x, w, color);
        break;
      case 2: // 180 degrees
        drawFastHLineRaw(WIDTH - x - w, HEIGHT - 1 - y, w, color);
        break;
      case 3: // 90 degrees CCW
        drawFastVLineRaw(y, HEIGHT - x - w, w, color);
        break;
      }
    }
  }
}

/*!
  @brief  Draw horizontal line into 'raw' canvas buffer. Inputs are assumed
          valid and in native coordinates. NO CLIPPING, ROTATION OR
          VALIDATION OF CANVAS BUFFER IS PERFORMED, calling function must
          be AWARE and WELL-BEHAVED. This is a protected function intended
          for related class code, not user-facing.
  @param  x      Column (horizontal position) of start point.
  @param  y      Row (vertical position of start and end points)
  @param  w      Width of vertical line to be drawn, including first point.
  @param  color  Line draw color; thresholded to 0 or 1 (any nonzero input).
*/
void GFXcanvas1::drawFastHLineRaw(int16_t x, int16_t y, int16_t w,
                                  uint16_t color) {
  uint8_t *ptr = &pixbuf[(x / 8) + y * ((WIDTH + 7) / 8)];

#if 0
// Do partial fill on first byte; may be only byte needed
// If more bytes...
//   Fill the solid ones.
//   If a lingering byte...
//     Do partial fill on that
  if (x & 7) { // Partial fill on first byte?

x & 7 = first bit set in first byte
(w & 7) + 1 ??? = number of bits set in first byte


    mask = (0xFF >> (7 - (w & 7))) << (x & 7);
    if ((color)) {
      *ptr |= mask;
    } else {
      *ptr &= ~mask;
    }
    ptr++;
w -= 

  if (x & 7) {                     // Partial fill on first byte?
    uint8_t mask = 0xFF >> mod;  // Consider bits from mod to end of 1st byte
    foo = 8 - mod;               // Bits to end of 1st byte
    if (w < foo) {               // If line width is less than this,
      mask &= (0xFF << (foo - w));
    }
// wait - wh if I started with mask of w&7 bits?

    *

    if (w < mod) {
        mask &= 0xFF << (mod - w);
    }

    w -= mod;
  }


if width is less than remainder, mask the mask




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
#endif
}
