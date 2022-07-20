#pragma once

#include "Adafruit_GFX.h"

/// A GFX 8-bit canvas context for graphics
class GFXcanvas8 : public Adafruit_GFX {
public:
  GFXcanvas8(uint16_t w, uint16_t h);
  ~GFXcanvas8(void);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void fillScreen(uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  uint8_t getPixel(int16_t x, int16_t y) const;
  /**********************************************************************/
  /*!
   @brief    Get a pointer to the internal buffer memory
   @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  uint8_t *getBuffer(void) const { return buffer; }

protected:
  uint8_t getRawPixel(int16_t x, int16_t y) const;
  void drawFastRawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastRawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

private:
  uint8_t *buffer;
};
