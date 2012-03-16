/***********************************
This is a our graphics core library, for all our displays. 
We'll be adapting all the
existing libaries to use this core to make updating, support 
and upgrading easier!

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above must be included in any redistribution
****************************************/

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
  void constructor(uint16_t w, uint16_t h);

  // this must be defined by the subclass
  virtual void drawPixel(uint16_t x, uint16_t y, uint16_t color);
  virtual void invertDisplay(boolean i);

  // these are 'generic' drawing functions, so we can share them!
  virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, 
		uint16_t color);
  virtual void drawFastVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
  virtual void drawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
  virtual void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
		uint16_t color);
  virtual void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
		uint16_t color);
  virtual void fillScreen(uint16_t color);

  void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, 
		  uint16_t color);
  void drawCircleHelper(uint16_t x0, uint16_t y0,
			uint16_t r, uint8_t cornername, uint16_t color);
  void fillCircle(uint16_t x0, uint16_t y0, uint16_t r, 
		  uint16_t color);
  void fillCircleHelper(uint16_t x0, uint16_t y0, uint16_t r,
		      uint8_t cornername, uint16_t delta, uint16_t color);

  void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		    uint16_t x2, uint16_t y2, uint16_t color);
  void fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		    uint16_t x2, uint16_t y2, uint16_t color);
  void drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
		     uint16_t radius, uint16_t color);
  void fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
		     uint16_t radius, uint16_t color);

  void drawBitmap(uint16_t x, uint16_t y, 
		  const uint8_t *bitmap, uint16_t w, uint16_t h,
		  uint16_t color);
  void drawChar(uint16_t x, uint16_t y, char c,
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

  uint16_t height(void);
  uint16_t width(void);

  void setRotation(uint8_t r);
  uint8_t getRotation(void);

 protected:
  uint16_t WIDTH, HEIGHT;       // this is the 'raw' display w/h - never changes
  uint16_t _width, _height;     // dependant on rotation
  uint16_t cursor_x, cursor_y, textcolor, textbgcolor;
  uint8_t textsize;
  uint8_t rotation;
};

#endif
