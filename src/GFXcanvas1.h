#pragma once

#include "Adafruit_GFX.h"

/// A GFX 1-bit canvas context for graphics
class GFXcanvas1 : public Adafruit_GFX {
public:
  GFXcanvas1(uint16_t w, uint16_t h);
  ~GFXcanvas1(void);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void fillScreen(uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  /*!
    @brief    Get the pixel color value at a given coordinate
    @param    x  x coordinate
    @param    y  y coordinate
    @returns  The pixel's binary color value, either 0x1 (on) or 0x0 (off)
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
    @returns  The pixel's binary color value, either 0x1 (on) or 0x0 (off)
  */
  bool getRawPixel(int16_t x, int16_t y) const;
  void drawFastRawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastRawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

private:
  uint8_t *buffer;

#ifdef __AVR__
  // Bitmask tables of 0x80>>X and ~(0x80>>X), because X>>Y is slow on AVR
  static const uint8_t PROGMEM GFXsetBit[], GFXclrBit[];
#endif
};
