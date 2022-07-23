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

// A 4-bit canvas for offscreen graphics in RAM.
class GFXcanvas4 : public Adafruit_GFX {
public:
  GFXcanvas4(uint16_t w, uint16_t h);
  ~GFXcanvas4(void);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  uint8_t getPixel(int16_t x, int16_t y) const;
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void fillScreen(uint16_t color);
  /*!
    @brief   Get pointer to internal canvas memory.
    @return  Pointer (uint8_t *) to start of canvas buffer.
  */
  uint8_t *getBuffer(void) const { return pixbuf; };
protected:
  uint8_t *pixbuf = NULL;
  void drawPixelRaw(int16_t x, int16_t y, uint16_t color);
  uint8_t getPixelRaw(int16_t x, int16_t y) const;
  void drawFastVLineRaw(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastHLineRaw(int16_t x, int16_t y, int16_t w, uint16_t color);
};
