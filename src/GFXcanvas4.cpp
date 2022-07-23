/*!
 * @file GFXcanvas4.cpp
 *
 * Part of Adafruit's GFX graphics library. Provides a 4-bit in-RAM
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

#include "GFXcanvas4.h"

/**************************************************************************/
/*!
   @brief  Instatiate a 4-bit graphics canvas.
   @param  w  Canvas width, in pixels
   @param  h  Canvas height, in pixels
*/
/**************************************************************************/
GFXcanvas4::GFXcanvas4(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {
  // Rows are byte-aligned. If allocation fails, no special action is
  // performed here. Drawing functions check for valid buffer.
  pixbuf = (uint8_t *)calloc(((w + 1) / 2) * h, 1);
}

/*!
  @brief  GFXcanvas4 destructor. Frees memory associated with canvas.
*/
GFXcanvas4::~GFXcanvas4(void) {
  if (pixbuf) {
    free(pixbuf);
  }
};

/**************************************************************************/
/*!
    @brief  Draw a pixel to the canvas framebuffer
    @param  x      x coordinate
    @param  y      y coordinate
    @param  color  4-bit color to set pixel
*/
/**************************************************************************/
void GFXcanvas4::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (pixbuf) {
    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
      return;

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
void GFXcanvas4::drawPixelRaw(int16_t x, int16_t y, uint16_t color) {
  uint8_t *ptr = &pixbuf[(x / 2) + y * ((WIDTH + 1) / 2)];
  if (x & 1) { // Odd column - lower nybble
    *ptr = (*ptr & 0xF0) | (color & 0xF);
  } else { // Even column - upper nybble
    *ptr = (*ptr & 0x0F) | (color << 4);
  }
}

uint8_t GFXcanvas4::getPixel(int16_t x, int16_t y) const {
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
  return getPixelRaw(x, y);
}

uint8_t GFXcanvas4::getPixelRaw(int16_t x, int16_t y) const {
  if ((x < 0) || (y < 0) || (x >= WIDTH) || (y >= HEIGHT))
    return 0;
  if (pixbuf) {
    uint8_t *ptr = &pixbuf[(x / 2) + y * ((WIDTH + 1) / 2)];
    if (x & 1) { // Odd column - lower nybble
      return *ptr & 0xF;
    } else { // Even column - upper nybble
      return *ptr >> 4;
    }
  }
  return 0;
}

/**************************************************************************/
/*!
    @brief  Fill the framebuffer completely with one color
    @param  color Binary (on or off) color to fill with
*/
/**************************************************************************/
void GFXcanvas4::fillScreen(uint16_t color) {
  if (pixbuf) {
    memset(pixbuf, (color & 0xF) * 0x11, ((WIDTH + 1) / 2) * HEIGHT);
  }
}

/**************************************************************************/
/*!
   @brief  Speed optimized vertical line drawing
   @param  x      Line horizontal start point
   @param  y      Line vertical start point
   @param  h      Height of vertical line to draw, including first point
   @param  color  Color to fill with
*/
/**************************************************************************/
void GFXcanvas4::drawFastVLine(int16_t x, int16_t y, int16_t h,
                               uint16_t color) {

  if (h < 0) { // Convert negative heights to positive equivalent
    h *= -1;
    y -= h - 1;
    if (y < 0) {
      h += y;
      y = 0;
    }
  }

  // Edge rejection (no-draw if totally off canvas)
  if ((x < 0) || (x >= width()) || (y >= height()) || ((y + h - 1) < 0)) {
    return;
  }

  if (y < 0) { // Clip top
    h += y;
    y = 0;
  }
  if (y + h > height()) { // Clip bottom
    h = height() - y;
  }

  if (getRotation() == 0) {
    drawFastVLineRaw(x, y, h, color);
  } else if (getRotation() == 1) {
    int16_t t = x;
    x = WIDTH - 1 - y;
    y = t;
    x -= h - 1;
    drawFastHLineRaw(x, y, h, color);
  } else if (getRotation() == 2) {
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;

    y -= h - 1;
    drawFastVLineRaw(x, y, h, color);
  } else if (getRotation() == 3) {
    int16_t t = x;
    x = y;
    y = HEIGHT - 1 - t;
    drawFastHLineRaw(x, y, h, color);
  }
}

/**************************************************************************/
/*!
   @brief  Speed optimized horizontal line drawing
   @param  x      Line horizontal start point
   @param  y      Line vertical start point
   @param  w      Width of horizontal line to be drawn, including first point
   @param  color  Color to fill with
*/
/**************************************************************************/
void GFXcanvas4::drawFastHLine(int16_t x, int16_t y, int16_t w,
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
  if ((y < 0) || (y >= height()) || (x >= width()) || ((x + w - 1) < 0)) {
    return;
  }

  if (x < 0) { // Clip left
    w += x;
    x = 0;
  }
  if (x + w >= width()) { // Clip right
    w = width() - x;
  }

  if (getRotation() == 0) {
    drawFastHLineRaw(x, y, w, color);
  } else if (getRotation() == 1) {
    int16_t t = x;
    x = WIDTH - 1 - y;
    y = t;
    drawFastVLineRaw(x, y, w, color);
  } else if (getRotation() == 2) {
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;

    x -= w - 1;
    drawFastHLineRaw(x, y, w, color);
  } else if (getRotation() == 3) {
    int16_t t = x;
    x = y;
    y = HEIGHT - 1 - t;
    y -= w - 1;
    drawFastVLineRaw(x, y, w, color);
  }
}

/**************************************************************************/
/*!
   @brief  Speed optimized vertical line drawing into the raw canvas buffer
   @param  x      Line horizontal start point
   @param  y      Line vertical start point
   @param  h      length of vertical line to be drawn, including first point
   @param  color  Color to fill with
*/
/**************************************************************************/
void GFXcanvas4::drawFastVLineRaw(int16_t x, int16_t y, int16_t h,
                                  uint16_t color) {
  // x & y already in raw (rotation 0) coordinates, no need to transform.
  int16_t row_bytes = ((WIDTH + 1) / 2);
  uint8_t *ptr = &pixbuf[(x / 2) + y * row_bytes];
  uint8_t mask;

  if (x & 1) { // Odd column - lower nybble
    color &= 0xF;
    mask = 0xF0;
  } else { // Even column - upper nybble
    color = (color & 0xF) << 4;
    mask = 0x0F;
  }
  for (int16_t i = 0; i < h; i++) {
    *ptr = (*ptr & mask) | color;
    ptr += row_bytes;
  }
}

/**************************************************************************/
/*!
   @brief  Speed optimized horizontal line drawing into the raw canvas buffer
   @param  x      Line horizontal start point
   @param  y      Line vertical start point
   @param  w      length of horizontal line to be drawn, including first point
   @param  color  Binary (on or off) color to fill with
*/
/**************************************************************************/
void GFXcanvas4::drawFastHLineRaw(int16_t x, int16_t y, int16_t w,
                                  uint16_t color) {
  // x & y already in raw (rotation 0) coordinates, no need to transform.
  int16_t rowBytes = ((WIDTH + 1) / 2);
  uint8_t *ptr = &pixbuf[(x / 2) + y * rowBytes];
  color &= 0xF;

  // check to see if first byte needs to be partially filled
  if ((x & 1) > 0) { // If starting in odd column...
    // Set lower nybble of first byte
    *ptr = (*ptr & 0xF0) | color;
    w--;   // Subtract 1 pixel from width
    ptr++; // Advance pixel pointer to next byte
  }

  if (w > 0) {
    // Fill remaining whole bytes...
    memset(ptr, color * 0x11, w / 2);
    // And upper nybble of last byte, if required...
    if (w & 1) {
      ptr += w / 2;
      *ptr = (*ptr * 0x0F) | (color << 4);
    }
  }
}
