/******************************************************************
 This is the core graphics library for all our displays, providing
 basic graphics primitives (points, lines, circles, etc.). It needs
 to be paired with a hardware-specific library for each display
 device we carry (handling the lower-level functions).
 
 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source hardware
 by purchasing products from Adafruit!
 
 Written by Limor Fried/Ladyada for Adafruit Industries.
 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.
 ******************************************************************/

#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

#define swap(a, b) { int16_t t = a; a = b; b = t; }

class Adafruit_GFX : public Print {
 public:

  //Adafruit_GFX();
  // i have no idea why we have to formally call the constructor. kinda sux
  void constructor(int16_t w, int16_t h);

  // this must be defined by the subclass
  virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
  virtual void invertDisplay(boolean i);

  // these are 'generic' drawing functions, so we can share them!
  virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, 
		uint16_t color);
  virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, 
		uint16_t color);
  virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, 
		uint16_t color);
  virtual void fillScreen(uint16_t color);

  void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
  void drawCircleHelper(int16_t x0, int16_t y0,
			int16_t r, uint8_t cornername, uint16_t color);
  void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
  void fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
		      uint8_t cornername, int16_t delta, uint16_t color);

  void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
		    int16_t x2, int16_t y2, uint16_t color);
  void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
		    int16_t x2, int16_t y2, uint16_t color);
  void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
		     int16_t radius, uint16_t color);
  void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
		     int16_t radius, uint16_t color);

  void drawBitmap(int16_t x, int16_t y, 
		  const uint8_t *bitmap, int16_t w, int16_t h,
		  uint16_t color);
  void drawChar(int16_t x, int16_t y, unsigned char c,
		uint16_t color, uint16_t bg, uint8_t size);
#if ARDUINO >= 100
  virtual size_t write(uint8_t);
#else
  virtual void   write(uint8_t);
#endif
  void setCursor(int16_t x, int16_t y);
  void setTextColor(uint16_t c);
  void setTextColor(uint16_t c, uint16_t bg);
  void setTextSize(uint8_t s);
  void setTextWrap(boolean w);

  int16_t height(void);
  int16_t width(void);

  void setRotation(uint8_t r);
  uint8_t getRotation(void);

 protected:
  int16_t  WIDTH, HEIGHT;   // this is the 'raw' display w/h - never changes
  int16_t  _width, _height; // dependent on rotation
  int16_t  cursor_x, cursor_y;
  uint16_t textcolor, textbgcolor;
  uint8_t  textsize;
  uint8_t  rotation;
  boolean  wrap; // If set, 'wrap' text at right edge of display
};

#endif
