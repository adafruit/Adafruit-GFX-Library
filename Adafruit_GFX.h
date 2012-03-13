#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

#define swap(a, b) { int16_t t = a; a = b; b = t; }

class Adafruit_GFX : public Print{
 public:
  // this must be defined by the subclass
  virtual void drawPixel(uint8_t x, uint8_t y, uint8_t color);

  // these are 'generic' drawing functions, so we can share them!
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, 
		uint8_t color);
  virtual void drawFastVLine(uint8_t x, uint8_t y, uint8_t h, uint8_t color);
  void drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		uint8_t color);
  void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		uint8_t color);

  void drawCircle(uint8_t x0, uint8_t y0, uint8_t r, 
		  uint8_t color);
  void fillCircle(uint8_t x0, uint8_t y0, uint8_t r, 
		  uint8_t color);

  void drawBitmap(uint8_t x, uint8_t y, 
		  const uint8_t *bitmap, uint8_t w, uint8_t h,
		  uint8_t color);
  void drawChar(uint8_t x, uint8_t y, char c,
		uint16_t color, uint16_t bg, uint8_t size);
#if ARDUINO >= 100
  virtual size_t write(uint8_t);
#else
  virtual void   write(uint8_t);
#endif
  void setCursor(uint16_t x, uint16_t y);
  void setTextColor(uint16_t c);
  void setTextColor(uint16_t c, uint16_t bg);
  void setTextSize(uint8_t s);


  // return the size of the display
  uint16_t width() { return WIDTH; }
  uint16_t height() { return HEIGHT; }

 protected:
  uint16_t WIDTH, HEIGHT;
  uint16_t cursor_x, cursor_y, textcolor, textbgcolor;
  uint8_t textsize;
};

#endif
