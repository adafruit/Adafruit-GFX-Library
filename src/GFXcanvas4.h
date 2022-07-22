/*!
 * @file GFXcanvas4.h
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

#pragma once

#include "Adafruit_GFX.h"

/// A GFX 4-bit canvas context for graphics
class GFXcanvas4 : public Adafruit_GFX {
public:
  GFXcanvas4(uint16_t w, uint16_t h);
  ~GFXcanvas4(void);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void fillScreen(uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  /*!
    @brief    Get the pixel color value at a given coordinate
    @param    x  x coordinate
    @param    y  y coordinate
    @returns  The pixel's color value, 0 to 15
  */
  bool getPixel(int16_t x, int16_t y) const;
  /*!
    @brief    Get a pointer to the internal buffer memory
    @returns  A pointer to the allocated buffer
  */
  uint8_t *getBuffer(void) const { return buffer; }

protected:
  /*!
    @brief    Get the pixel color value at a given, unrotated coordinate.
              This method is intended for hardware drivers to get pixel value
              in native physical coordinates.
    @param    x  x coordinate
    @param    y  y coordinate
    @returns  The pixel's color value, 0 to 15
  */
  bool getRawPixel(int16_t x, int16_t y) const;
  void drawFastRawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastRawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

private:
  uint8_t *buffer;
};
