#ifndef __RENDERGFX_H
#define __RENDERGFX_H

#include "../Adafruit_GFX.h"
#include "Arduino.h"
#include <gd.h>

class RenderGFX : public Adafruit_GFX {
public:
  RenderGFX(uint16_t w, uint16_t h);

  virtual void drawPixel(int16_t x, int16_t y, uint16_t color);

  virtual uint16_t color(uint8_t r, uint8_t g, uint8_t b);

  virtual void save(const char *name);

protected:
  gdImagePtr im;
};

#endif
