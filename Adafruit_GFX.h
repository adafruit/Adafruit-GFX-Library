#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

#define GFX_COLOR_DEFAULT 0
#define GFX_COLOR_16BIT 0
#define GFX_COLOR_8BIT 1
#define GFX_COLOR_RGB24BIT 2
// Don't forget to define GFX_Color_t if you use the CUSTOM mode
#define GFX_COLOR_CUSTOM 255

#define GFX_MODE_DEFAULT 0
#define GFX_MODE_16BIT 0
#define GFX_MODE_8BIT 1

/*
 * Begin config section
 */
#define GFX_COLOR_MODE GFX_COLOR_DEFAULT
#define GFX_MODE GFX_MODE_DEFAULT
/*
 * End config section
 */

#if GFX_COLOR_MODE == GFX_COLOR_16BIT
typedef uint16_t GFX_Color_t;
#elif GFX_COLOR_MODE == GFX_COLOR_8BIT
typedef uint8_t GFX_Color_t;
#elif GFX_COLOR_MODE == GFX_COLOR_RGB24BIT
typedef struct {uint8_t r; uint8_t g; uint8_t b;} GFX_Color_t;
#elif GFX_COLOR_MODE == GFX_COLOR_CUSTOM

#else
#warning "Unknown color mode specified"
#endif

#if GFX_MODE == GFX_MODE_16BIT
typedef int16_t GFX_Coord_t;
#elif GFX_MODE == GFX_MODE_8BIT
typedef int8_t GFX_Coord_t;
#else
#warning "Unknown mode specified"
#endif


#define swap(a, b) { GFX_Coord_t t = a; a = b; b = t; }

class Adafruit_GFX : public Print {

 public:

  Adafruit_GFX(GFX_Coord_t w, GFX_Coord_t h); // Constructor

  // This MUST be defined by the subclass:
  virtual void drawPixel(GFX_Coord_t x, GFX_Coord_t y, GFX_Color_t color) = 0;

  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void
    drawLine(GFX_Coord_t x0, GFX_Coord_t y0, GFX_Coord_t x1, GFX_Coord_t y1, GFX_Color_t color),
    drawFastVLine(GFX_Coord_t x, GFX_Coord_t y, GFX_Coord_t h, GFX_Color_t color),
    drawFastHLine(GFX_Coord_t x, GFX_Coord_t y, GFX_Coord_t w, GFX_Color_t color),
    drawRect(GFX_Coord_t x, GFX_Coord_t y, GFX_Coord_t w, GFX_Coord_t h, GFX_Color_t color),
    fillRect(GFX_Coord_t x, GFX_Coord_t y, GFX_Coord_t w, GFX_Coord_t h, GFX_Color_t color),
    fillScreen(GFX_Color_t color),
    invertDisplay(boolean i);

  // These exist only with Adafruit_GFX (no subclass overrides)
  void
    drawCircle(GFX_Coord_t x0, GFX_Coord_t y0, GFX_Coord_t r, GFX_Color_t color),
    drawCircleHelper(GFX_Coord_t x0, GFX_Coord_t y0, GFX_Coord_t r, uint8_t cornername,
      GFX_Color_t color),
    fillCircle(GFX_Coord_t x0, GFX_Coord_t y0, GFX_Coord_t r, GFX_Color_t color),
    fillCircleHelper(GFX_Coord_t x0, GFX_Coord_t y0, GFX_Coord_t r, uint8_t cornername,
      GFX_Coord_t delta, GFX_Color_t color),
    drawTriangle(GFX_Coord_t x0, GFX_Coord_t y0, GFX_Coord_t x1, GFX_Coord_t y1,
      GFX_Coord_t x2, GFX_Coord_t y2, GFX_Color_t color),
    fillTriangle(GFX_Coord_t x0, GFX_Coord_t y0, GFX_Coord_t x1, GFX_Coord_t y1,
      GFX_Coord_t x2, GFX_Coord_t y2, GFX_Color_t color),
    drawRoundRect(GFX_Coord_t x0, GFX_Coord_t y0, GFX_Coord_t w, GFX_Coord_t h,
      GFX_Coord_t radius, GFX_Color_t color),
    fillRoundRect(GFX_Coord_t x0, GFX_Coord_t y0, GFX_Coord_t w, GFX_Coord_t h,
      GFX_Coord_t radius, GFX_Color_t color),
    drawBitmap(GFX_Coord_t x, GFX_Coord_t y, const uint8_t *bitmap,
      GFX_Coord_t w, GFX_Coord_t h, GFX_Color_t color),
    drawChar(GFX_Coord_t x, GFX_Coord_t y, unsigned char c, GFX_Color_t color,
      GFX_Color_t bg, uint8_t size),
    setCursor(GFX_Coord_t x, GFX_Coord_t y),
    setTextColor(GFX_Color_t c),
    setTextColor(GFX_Color_t c, GFX_Color_t bg),
    setTextSize(uint8_t s),
    setTextWrap(boolean w),
    setRotation(uint8_t r);

#if ARDUINO >= 100
  virtual size_t write(uint8_t);
#else
  virtual void   write(uint8_t);
#endif

  GFX_Coord_t
    height(void),
    width(void);

  uint8_t getRotation(void);

 protected:
  const GFX_Coord_t
    WIDTH, HEIGHT;   // This is the 'raw' display w/h - never changes
  GFX_Coord_t
    _width, _height, // Display w/h as modified by current rotation
    cursor_x, cursor_y;
  GFX_Color_t
    textcolor, textbgcolor;
  uint8_t
    textsize,
    rotation;
  boolean
    wrap; // If set, 'wrap' text at right edge of display
};

#endif // _ADAFRUIT_GFX_H
