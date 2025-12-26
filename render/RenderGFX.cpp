#include "RenderGFX.h"

#include <cstdio>

RenderGFX::RenderGFX(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {

  im = gdImageCreate(w, h);
}

void RenderGFX::drawPixel(int16_t x, int16_t y, uint16_t color) {
  gdImageSetPixel(im, x, y, color);
}

uint16_t RenderGFX::color(uint8_t r, uint8_t g, uint8_t b) {
  return gdImageColorAllocate(im, r, g, b);
}

void RenderGFX::save(const char *name) {
  FILE *pngout = fopen(name, "wb");
  gdImagePng(im, pngout);
  fclose(pngout);
}
