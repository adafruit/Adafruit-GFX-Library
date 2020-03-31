#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#if ARDUINO >= 100
#include "Arduino.h"
#include "Print.h"
#else
#include "WProgram.h"
#endif
#include "gfxfont.h"

/// A generic graphics superclass that can handle all sorts of drawing. At a
/// minimum you can subclass and provide drawPixel(). At a maximum you can do a
/// ton of overriding to optimize. Used for any/all Adafruit displays!
class Adafruit_GFX : public Print {

public:
  /**************************************************************************/
  /*!
     @brief    Instatiate a GFX context for graphics! Can only be done by a
     superclass
     @param    w   Display width, in pixels
     @param    h   Display height, in pixels
  */
  /**************************************************************************/
  Adafruit_GFX(int16_t w, int16_t h); // Constructor

  /**************************************************************************/
  /*!
     @brief Virtual drawPixel() function to draw to the screen/framebuffer/etc.
     Must be overridden in subclass.
     @param x X coordinate.
     @param y Y coordinate.
     @param color 16-bit pixel color.
  */
  /**************************************************************************/
  virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;

  // TRANSACTION API / CORE DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.

  /**************************************************************************/
  /*!
     @brief Start a display-writing routine.
     Override in subclasses.
  */
  /**************************************************************************/
  virtual void startWrite();

  /**************************************************************************/
  /*!
     @brief Write a pixel.
     Override in subclasses if startWrite is defined!
     @param x     x coordinate
     @param y     y coordinate
     @param color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  virtual void writePixel(int16_t x, int16_t y, uint16_t color);

  /**************************************************************************/
  /*!
     @brief Write a rectangle completely with one color.
     Override in subclasses if startWrite is defined!
     @param x     Top left corner x coordinate
     @param y     Top left corner y coordinate
     @param w     Width in pixels
     @param h     Height in pixels
     @param color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                             uint16_t color);

  /**************************************************************************/
  /*!
     @brief Write a perfectly vertical line
     Override in subclasses if startWrite is defined!
     @param x   Top-most x coordinate
     @param y   Top-most y coordinate
     @param h   Height in pixels
     @param color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

  /**************************************************************************/
  /*!
     @brief Write a perfectly horizontal line.
     Override in subclasses if startWrite is defined!
     @param x     Left-most x coordinate
     @param y     Left-most y coordinate
     @param w     Width in pixels
     @param color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

  /**************************************************************************/
  /*!
      @brief Write a line.
      Bresenham's algorithm - thx wikpedia
      @param x0    Start point x coordinate
      @param y0    Start point y coordinate
      @param x1    End point x coordinate
      @param y1    End point y coordinate
      @param color 16-bit 5-6-5 Color to draw with
  */
  /**************************************************************************/
  virtual void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                         uint16_t color);

  /**************************************************************************/
  /*!
     @brief    End a display-writing routine.
     Override in subclasses if startWrite is defined!
  */
  /**************************************************************************/
  virtual void endWrite();

  // CONTROL API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.

  /**************************************************************************/
  /*!
      @brief Set rotation setting for display
      @param r 0 thru 3 corresponding to 4 cardinal rotations
  */
  /**************************************************************************/
  virtual void setRotation(uint8_t r);

  /**************************************************************************/
  /*!
      @brief Invert the display (ideally using built-in hardware command)
      @param i True if you want to invert, false to make 'normal'
  */
  /**************************************************************************/
  virtual void invertDisplay(boolean i);

  // BASIC DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  // It's good to implement those, even if using transaction API

  /**************************************************************************/
  /*!
     @brief Draw a vertical line.
     often optimized in a subclass.
     @param x     Top-most x coordinate
     @param y     Top-most y coordinate
     @param h     Height in pixels
     @param color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

  /**************************************************************************/
  /*!
     @brief Draw a horizontal line.
     Often optimized in a subclass.
     @param x     Left-most x coordinate
     @param y     Left-most y coordinate
     @param w     Width in pixels
     @param color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

  /**************************************************************************/
  /*!
     @brief Fill a rectangle completely with one color.
     Optimize in subclasses if desired!
     @param x     Top left corner x coordinate
     @param y     Top left corner y coordinate
     @param w     Width in pixels
     @param h     Height in pixels
     @param color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                        uint16_t color);

  /**************************************************************************/
  /*!
     @brief Fill the screen completely with one color.
     Override in subclasses if desired!
     @param color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  virtual void fillScreen(uint16_t color);

  // Optional and probably not necessary to change
  /**************************************************************************/
  /*!
     @brief Draw a line
     Optimize in subclasses if desired!
     @param x0    Start point x coordinate
     @param y0    Start point y coordinate
     @param x1    End point x coordinate
     @param y1    End point y coordinate
     @param color 16-bit 5-6-5 Color to draw with
  */
  /**************************************************************************/
  virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                        uint16_t color);

  /**************************************************************************/
  /*!
     @brief Draw a rectangle with no fill color
     @param x     Top left corner x coordinate
     @param y     Top left corner y coordinate
     @param w     Width in pixels
     @param h     Height in pixels
     @param color 16-bit 5-6-5 Color to draw with
  */
  /**************************************************************************/
  virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
                        uint16_t color);

  // These exist only with Adafruit_GFX (not virtual, no subclass overrides)

  /**************************************************************************/
  /*!
     @brief Draw a circle outline
     @param x0    Center x coordinate
     @param y0    Center y coordinate
     @param r     Radius of circle
     @param color 16-bit 5-6-5 Color to draw with
  */
  /**************************************************************************/
  void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

  /**************************************************************************/
  /*!
     @brief Draw selected quarter-circles, used in drawing roundrects
     @param x0         Center-point x coordinate
     @param y0         Center-point y coordinate
     @param r          Radius of circle
     @param cornername Bits 0..3 indicate which quadrant(s) to draw
     @param color      16-bit 5-6-5 Color to draw with
  */
  /**************************************************************************/
  void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
                        uint16_t color);

  /**************************************************************************/
  /*!
     @brief Draw a circle with filled color
     @param x0    Center-point x coordinate
     @param y0    Center-point y coordinate
     @param r     Radius of circle
     @param color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

  /**************************************************************************/
  /*!
     @brief Fill quarter-circle, used for circles and roundrects
     @param x0       Center-point x coordinate
     @param y0       Center-point y coordinate
     @param r        Radius of circle
     @param corners  bits 0..1 indicate which quadrants to draw
     @param delta    Offset from center-point, used for round-rects
     @param color    16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t corners,
                        int16_t delta, uint16_t color);

  /**************************************************************************/
  /*!
     @brief Draw a triangle with no fill color
     @param x0    Vertex #0 x coordinate
     @param y0    Vertex #0 y coordinate
     @param x1    Vertex #1 x coordinate
     @param y1    Vertex #1 y coordinate
     @param x2    Vertex #2 x coordinate
     @param y2    Vertex #2 y coordinate
     @param color 16-bit 5-6-5 Color to draw with
  */
  /**************************************************************************/
  void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                    int16_t y2, uint16_t color);

  /**************************************************************************/
  /*!
     @brief Draw a triangle with color-fill
     @param x0    Vertex #0 x coordinate
     @param y0    Vertex #0 y coordinate
     @param x1    Vertex #1 x coordinate
     @param y1    Vertex #1 y coordinate
     @param x2    Vertex #2 x coordinate
     @param y2    Vertex #2 y coordinate
     @param color 16-bit 5-6-5 Color to fill/draw with
  */
  /**************************************************************************/
  void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                    int16_t y2, uint16_t color);

  /**************************************************************************/
  /*!
     @brief Draw a rounded rectangle with no fill color
     @param x     Top left corner x coordinate
     @param y     Top left corner y coordinate
     @param w     Width in pixels
     @param h     Height in pixels
     @param r     Radius of corner rounding
     @param color 16-bit 5-6-5 Color to draw with
  */
  /**************************************************************************/
  void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,
                     uint16_t color);

  /**************************************************************************/
  /*!
     @brief Draw a rounded rectangle with fill color
     @param x     Top left corner x coordinate
     @param y     Top left corner y coordinate
     @param w     Width in pixels
     @param h     Height in pixels
     @param r     Radius of corner rounding
     @param color 16-bit 5-6-5 Color to draw/fill with
  */
  /**************************************************************************/
  void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,
                     uint16_t color);

  // BITMAP / XBITMAP / GRAYSCALE / RGB BITMAP FUNCTIONS ---------------------

  /**************************************************************************/
  /*!
     @brief Draw a PROGMEM-resident 1-bit image.
     Draws at the specified (x,y) position, using the specified foreground color
     (unset bits are transparent).
     @param x       Top left corner x coordinate
     @param y       Top left corner y coordinate
     @param bitmap  byte array with monochrome bitmap
     @param w       Width of bitmap in pixels
     @param h       Height of bitmap in pixels
     @param color  16-bit 5-6-5 Color to draw with
  */
  /**************************************************************************/
  void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w,
                  int16_t h, uint16_t color);

  /**************************************************************************/
  /*!
     @brief Draw a PROGMEM-resident 1-bit image.
     Draws at the specified (x,y) position, using the specified foreground
     (for set bits) and background (unset bits) colors.
     @param x       Top left corner x coordinate
     @param y       Top left corner y coordinate
     @param bitmap  byte array with monochrome bitmap
     @param w       Width of bitmap in pixels
     @param h       Height of bitmap in pixels
     @param color   16-bit 5-6-5 Color to draw pixels with
     @param bg      16-bit 5-6-5 Color to draw background with
  */
  /**************************************************************************/
  void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w,
                  int16_t h, uint16_t color, uint16_t bg);

  /**************************************************************************/
  /*!
     @brief Draw a RAM-resident 1-bit image.
     Draws at the specified (x,y) position, using the specified foreground color
     (unset bits are transparent).
     @param x       Top left corner x coordinate
     @param y       Top left corner y coordinate
     @param bitmap  byte array with monochrome bitmap
     @param w       Width of bitmap in pixels
     @param h       Height of bitmap in pixels
     @param color   16-bit 5-6-5 Color to draw with
  */
  /**************************************************************************/
  void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h,
                  uint16_t color);

  /**************************************************************************/
  /*!
     @brief Draw a RAM-resident 1-bit image.
     Draws at the specified (x,y) position, using the specified foreground
     (for set bits) and background (unset bits).
     colors.
     @param x       Top left corner x coordinate
     @param y       Top left corner y coordinate
     @param bitmap  byte array with monochrome bitmap
     @param w       Width of bitmap in pixels
     @param h       Height of bitmap in pixels
     @param color   16-bit 5-6-5 Color to draw pixels with
     @param bg      16-bit 5-6-5 Color to draw background with
  */
  /**************************************************************************/
  void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h,
                  uint16_t color, uint16_t bg);

  /**************************************************************************/
  /*!
     @brief Draw PROGMEM-resident XBitMap Files (*.xbm), exported from GIMP.
     Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
     C Array can be directly used with this function.
     There is no RAM-resident version of this function; if generating bitmaps
     in RAM, use the format defined by drawBitmap() and call that instead.
     @param x       Top left corner x coordinate
     @param y       Top left corner y coordinate
     @param bitmap  byte array with monochrome bitmap
     @param w       Width of bitmap in pixels
     @param h       Height of bitmap in pixels
     @param color   16-bit 5-6-5 Color to draw pixels with
  */
  /**************************************************************************/
  void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w,
                   int16_t h, uint16_t color);

  /**************************************************************************/
  /*!
     @brief Draw a PROGMEM-resident 8-bit image (grayscale).
     Draws at the specified (x,y) pos. Specifically for 8-bit display devices
     such as IS31FL3731; no color reduction/expansion is performed.
     @param x       Top left corner x coordinate
     @param y       Top left corner y coordinate
     @param bitmap  byte array with grayscale bitmap
     @param w       Width of bitmap in pixels
     @param h       Height of bitmap in pixels
  */
  /**************************************************************************/
  void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
                           int16_t w, int16_t h);

  /**************************************************************************/
  /*!
     @brief Draw a RAM-resident 8-bit image (grayscale).
     Draws at the specified (x,y) pos. Specifically for 8-bit display devices
     such as IS31FL3731; no color reduction/expansion is performed.
     @param x       Top left corner x coordinate
     @param y       Top left corner y coordinate
     @param bitmap  byte array with grayscale bitmap
     @param w       Width of bitmap in pixels
     @param h       Height of bitmap in pixels
  */
  /**************************************************************************/
  void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w,
                           int16_t h);

  /**************************************************************************/
  /*!
     @brief Draw a PROGMEM-resident 8-bit image (grayscale) with a 1-bit mask.
     Draw (set bits = opaque, unset bits = clear) at the specified (x,y)
     position. BOTH buffers (grayscale and mask) must be PROGMEM-resident.
     Specifically for 8-bit display devices such as IS31FL3731; no color
     reduction/expansion is performed.
     @param x       Top left corner x coordinate
     @param y       Top left corner y coordinate
     @param bitmap  byte array with grayscale bitmap
     @param mask    byte array with mask bitmap
     @param w       Width of bitmap in pixels
     @param h       Height of bitmap in pixels
  */
  /**************************************************************************/
  void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
                           const uint8_t *mask, int16_t w, int16_t h);

  /**************************************************************************/
  /*!
     @brief Draw a RAM-resident 8-bit image (grayscale) with a 1-bit mask.
     Draw (set bits = opaque, unset bits = clear) at the specified (x,y)
     position. BOTH buffers (grayscale and mask) must be RAM-residentt, no
     mix-and-match Specifically for 8-bit display devices such as IS31FL3731; no
     color reduction/expansion is performed.
     @param x       Top left corner x coordinate
     @param y       Top left corner y coordinate
     @param bitmap  byte array with grayscale bitmap
     @param mask    byte array with mask bitmap
     @param w       Width of bitmap in pixels
     @param h       Height of bitmap in pixels
  */
  /**************************************************************************/
  void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint8_t *mask,
                           int16_t w, int16_t h);

  /**************************************************************************/
  /*!
     @brief Draw a PROGMEM-resident 16-bit image (RGB 5/6/5).
     Draws at the specified (x,y) position.
     For 16-bit display devices; no color reduction performed.
     @param x      Top left corner x coordinate
     @param y      Top left corner y coordinate
     @param bitmap byte array with 16-bit color bitmap
     @param w      Width of bitmap in pixels
     @param h      Height of bitmap in pixels
  */
  /**************************************************************************/
  void drawRGBBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w,
                     int16_t h);

  /**************************************************************************/
  /*!
     @brief Draw a RAM-resident 16-bit image (RGB 5/6/5).
     Draws at the specified (x,y) position.
     For 16-bit display devices; no color reduction performed.
     @param x       Top left corner x coordinate
     @param y       Top left corner y coordinate
     @param bitmap  byte array with 16-bit color bitmap
     @param w       Width of bitmap in pixels
     @param h       Height of bitmap in pixels
  */
  /**************************************************************************/
  void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w,
                     int16_t h);

  /**************************************************************************/
  /*!
     @brief Draw a PROGMEM-resident 16-bit image (RGB 5/6/5) with a 1-bit mask.
     Draws (set bits = opaque, unset bits = clear) at the specified (x,y)
     position. BOTH buffers (color and mask) must be PROGMEM-resident. For
     16-bit display devices; no color reduction performed.
     @param x      Top left corner x coordinate
     @param y      Top left corner y coordinate
     @param bitmap byte array with 16-bit color bitmap
     @param mask   byte array with monochrome mask bitmap
     @param w      Width of bitmap in pixels
     @param h      Height of bitmap in pixels
  */
  /**************************************************************************/
  void drawRGBBitmap(int16_t x, int16_t y, const uint16_t *bitmap,
                     const uint8_t *mask, int16_t w, int16_t h);

  /**************************************************************************/
  /*!
     @brief Draw a RAM-resident 16-bit image (RGB 5/6/5) with a 1-bit mask.
     Draws (set bits = opaque, unset bits = clear) at the specified (x,y)
     position. BOTH buffers (color and mask) must be RAM-resident. For 16-bit
     display devices; no color reduction performed.
     @param    x   Top left corner x coordinate
     @param    y   Top left corner y coordinate
     @param    bitmap  byte array with 16-bit color bitmap
     @param    mask  byte array with monochrome mask bitmap
     @param    w   Width of bitmap in pixels
     @param    h   Height of bitmap in pixels
  */
  /**************************************************************************/
  void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask,
                     int16_t w, int16_t h);

  // TEXT- AND CHARACTER-HANDLING FUNCTIONS ----------------------------------

  /**************************************************************************/
  /*!
     @brief Draw a single character
     @param x     Bottom left corner x coordinate
     @param y     Bottom left corner y coordinate
     @param c     The 8-bit font-indexed character (likely ascii)
     @param color 16-bit 5-6-5 Color to draw chraracter with
     @param bg    16-bit 5-6-5 Color to fill background with (no background if
     bg==color)
     @param size  width and height magnification (original size = 1)
  */
  /**************************************************************************/
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
                uint16_t bg, uint8_t size);

  /**************************************************************************/
  /*!
     @brief Draw a single character
     @param x       Bottom left corner x coordinate
     @param y       Bottom left corner y coordinate
     @param c       The 8-bit font-indexed character (likely ascii)
     @param color   16-bit 5-6-5 Color to draw chraracter with
     @param bg      16-bit 5-6-5 Color to fill background with (no background if
     bg==color)
     @param size_x  width magnification (original width = 1)
     @param size_y  height magnification (original height = 1)
  */
  /**************************************************************************/
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
                uint16_t bg, uint8_t size_x, uint8_t size_y);

  /**************************************************************************/
  /*!
      @brief Helper to determine size of a string with current font/size.
      Pass string and a cursor position, returns UL corner and W,H.
      @param s  Ascii string to measure
      @param x  Current cursor X
      @param y  Current cursor Y
      @param x1 Boundary X coordinate, set by function
      @param y1 Boundary Y coordinate, set by function
      @param w  Boundary width, set by function
      @param h  Boundary height, set by function
  */
  /**************************************************************************/
  void getTextBounds(const char *s, int16_t x, int16_t y, int16_t *x1,
                     int16_t *y1, uint16_t *w, uint16_t *h);

  /**************************************************************************/
  /*!
      @brief Helper to determine size of a string with current font/size.
      Pass string and a cursor position, returns UL corner and W,H.
      @param s  Ascii string to measure (as an arduino String() class)
      @param x  Current cursor X
      @param y  Current cursor Y
      @param x1 Boundary X coordinate, set by function
      @param y1 Boundary Y coordinate, set by function
      @param w  Boundary width, set by function
      @param h  Boundary height, set by function
  */
  /**************************************************************************/
  void getTextBounds(const String &s, int16_t x, int16_t y, int16_t *x1,
                     int16_t *y1, uint16_t *w, uint16_t *h);

  /**************************************************************************/
  /*!
      @brief Helper to determine size of a PROGMEM string with current
     font/size. Pass string and a cursor position, returns UL corner and W,H.
      @param s  Flash-memory ascii string to measure
      @param x  Current cursor X
      @param y  Current cursor Y
      @param x1 Boundary X coordinate, set by function
      @param y1 Boundary Y coordinate, set by function
      @param w  Boundary width, set by function
      @param h  Boundary height, set by function
  */
  /**************************************************************************/
  void getTextBounds(const __FlashStringHelper *s, int16_t x, int16_t y,
                     int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);

  /**************************************************************************/
  /*!
     @brief Set text magnification.
     Each pixel is scaled up into an {s,s} square.
     For example, 1 means {6,8}, 2 means {12,16}, 3 means {18,24}.
     @param s text magnification (default=1).
  */
  /**************************************************************************/
  void setTextSize(uint8_t s);

  /**************************************************************************/
  /*!
     @brief Set text 'magnification' size.
     Each pixel is scaled up into an {sx,sy} rectangle.
     @param sx width magnification (default=1).
     @param sy height magnification (default=1).
  */
  /**************************************************************************/
  void setTextSize(uint8_t sx, uint8_t sy);

  /**************************************************************************/
  /*!
     @brief Set the font to display when print()ing, either custom or default
     @param f The GFXfont object. If NULL, use built in 6x8 font.
  */
  /**************************************************************************/
  void setFont(const GFXfont *f = NULL);

  /**********************************************************************/
  /*!
    @brief  Set text cursor location
    @param  x    X coordinate in pixels
    @param  y    Y coordinate in pixels
  */
  /**********************************************************************/
  void setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
  }

  /**********************************************************************/
  /*!
    @brief   Set text font color with transparant background
    @param   c   16-bit 5-6-5 Color to draw text with
    @note    For 'transparent' background, background and foreground
             are set to same color rather than using a separate flag.
  */
  /**********************************************************************/
  void setTextColor(uint16_t c) { textcolor = textbgcolor = c; }

  /**********************************************************************/
  /*!
    @brief   Set text font color with custom background color
    @param   c   16-bit 5-6-5 Color to draw text with
    @param   bg  16-bit 5-6-5 Color to draw background/fill with
  */
  /**********************************************************************/
  void setTextColor(uint16_t c, uint16_t bg) {
    textcolor = c;
    textbgcolor = bg;
  }

  /**********************************************************************/
  /*!
  @brief  Set whether text that is too long for the screen width should
          automatically wrap around to the next line (else clip right).
  @param  w  true for wrapping, false for clipping
  */
  /**********************************************************************/
  void setTextWrap(boolean w) { wrap = w; }

  /**********************************************************************/
  /*!
    @brief  Enable (or disable) Code Page 437-compatible charset.
            There was an error in glcdfont.c for the longest time -- one
            character (#176, the 'light shade' block) was missing -- this
            threw off the index of every character that followed it.
            But a TON of code has been written with the erroneous
            character indices. By default, the library uses the original
            'wrong' behavior and old sketches will still work. Pass
            'true' to this function to use correct CP437 character values
            in your code.
    @param  x  true = enable (new behavior), false = disable (old behavior)
  */
  /**********************************************************************/
  void cp437(boolean x = true) { _cp437 = x; }

  using Print::write;

  /**************************************************************************/
  /*!
      @brief Print one byte/character of data, used to support print()
      @param c  The 8-bit ascii character to write
      @returns 1
  */
  /**************************************************************************/
#if ARDUINO >= 100
  virtual size_t write(uint8_t c) { return _write(c); }
#else
  virtual void write(uint9_t c) { _write(c); } // old style
#endif

  /************************************************************************/
  /*!
    @brief      Get width of the display, accounting for current rotation
    @returns    Width in pixels
  */
  /************************************************************************/
  int16_t width() const { return _width; };

  /************************************************************************/
  /*!
    @brief      Get height of the display, accounting for current rotation
    @returns    Height in pixels
  */
  /************************************************************************/
  int16_t height() const { return _height; }

  /************************************************************************/
  /*!
    @brief      Get rotation setting for display
    @returns    0 thru 3 corresponding to 4 cardinal rotations
  */
  /************************************************************************/
  uint8_t getRotation() const { return rotation; }

  // get current cursor position (get rotation safe maximum values,
  // using: width() for x, height() for y)
  /************************************************************************/
  /*!
    @brief  Get text cursor X location
    @returns    X coordinate in pixels
  */
  /************************************************************************/
  int16_t getCursorX() const { return cursor_x; }

  /************************************************************************/
  /*!
    @brief      Get text cursor Y location
    @returns    Y coordinate in pixels
  */
  /************************************************************************/
  int16_t getCursorY() const { return cursor_y; };

protected:
  /**************************************************************************/
  /*!
      @brief Helper to determine size of a character with current font/size.
      Broke this out as it's used by both the PROGMEM- and RAM-resident
      getTextBounds() functions.
      @param c     The ascii character in question
      @param x     Pointer to x location of character
      @param y     Pointer to y location of character
      @param minx  Minimum clipping value for X
      @param miny  Minimum clipping value for Y
      @param maxx  Maximum clipping value for X
      @param maxy  Maximum clipping value for Y
  */
  /**************************************************************************/
  void charBounds(char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny,
                  int16_t *maxx, int16_t *maxy);

  int16_t WIDTH;        ///< This is the 'raw' display width - never changes
  int16_t HEIGHT;       ///< This is the 'raw' display height - never changes
  int16_t _width;       ///< Display width as modified by current rotation
  int16_t _height;      ///< Display height as modified by current rotation
  int16_t cursor_x;     ///< x location to start print()ing text
  int16_t cursor_y;     ///< y location to start print()ing text
  uint16_t textcolor;   ///< 16-bit background color for print()
  uint16_t textbgcolor; ///< 16-bit text color for print()
  uint8_t textsize_x;   ///< Desired magnification in X-axis of text to print()
  uint8_t textsize_y;   ///< Desired magnification in Y-axis of text to print()
  uint8_t rotation;     ///< Display rotation (0 thru 3)
  boolean wrap;         ///< If set, 'wrap' text at right edge of display
  boolean _cp437;       ///< If set, use correct CP437 charset (default is off)
  GFXfont *gfxFont;     ///< Pointer to special font

private:
  /*! @brief Common implementation of the `write` member functions */
  size_t _write(uint8_t c);
};

/// A simple drawn button UI element
class Adafruit_GFX_Button {

public:
  /**************************************************************************/
  /*!
     @brief Create a simple drawn button UI element
  */
  /**************************************************************************/
  Adafruit_GFX_Button();

  // "Classic" initButton() uses center & size

  /**************************************************************************/
  /*!
     @brief Initialize button with our desired color/size/settings
     @param gfx       Pointer to our display so we can draw to it!
     @param x         The X coordinate of the center of the button
     @param y         The Y coordinate of the center of the button
     @param w         Width of the buttton
     @param h         Height of the buttton
     @param outline   Color of the outline (16-bit 5-6-5 standard)
     @param fill      Color of the button fill (16-bit 5-6-5 standard)
     @param textcolor Color of the button label (16-bit 5-6-5 standard)
     @param label     Ascii string of the text inside the button
     @param textsize  The font magnification of the label text
  */
  /**************************************************************************/
  void initButton(Adafruit_GFX *gfx, int16_t x, int16_t y, uint16_t w,
                  uint16_t h, uint16_t outline, uint16_t fill,
                  uint16_t textcolor, char *label, uint8_t textsize);

  /**************************************************************************/
  /*!
     @brief Initialize button with our desired color/size/settings
     @param gfx        Pointer to our display so we can draw to it!
     @param x          The X coordinate of the center of the button
     @param y          The Y coordinate of the center of the button
     @param w          Width of the buttton
     @param h          Height of the buttton
     @param outline    Color of the outline (16-bit 5-6-5 standard)
     @param fill       Color of the button fill (16-bit 5-6-5 standard)
     @param textcolor  Color of the button label (16-bit 5-6-5 standard)
     @param label      Ascii string of the text inside the button
     @param textsize_x The font magnification in X-axis of the label text
     @param textsize_y The font magnification in Y-axis of the label text
  */
  /**************************************************************************/
  void initButton(Adafruit_GFX *gfx, int16_t x, int16_t y, uint16_t w,
                  uint16_t h, uint16_t outline, uint16_t fill,
                  uint16_t textcolor, char *label, uint8_t textsize_x,
                  uint8_t textsize_y);

  // New/alt initButton() uses upper-left corner & size

  /**************************************************************************/
  /*!
     @brief Initialize to color/size/settings, with upper-left coordinates
     @param gfx       Pointer to our display so we can draw to it!
     @param x1        The X coordinate of the Upper-Left corner of the button
     @param y1        The Y coordinate of the Upper-Left corner of the button
     @param w         Width of the buttton
     @param h         Height of the buttton
     @param outline   Color of the outline (16-bit 5-6-5 standard)
     @param fill      Color of the button fill (16-bit 5-6-5 standard)
     @param textcolor Color of the button label (16-bit 5-6-5 standard)
     @param label     Ascii string of the text inside the button
     @param textsize  The font magnification of the label text
  */
  /**************************************************************************/
  void initButtonUL(Adafruit_GFX *gfx, int16_t x1, int16_t y1, uint16_t w,
                    uint16_t h, uint16_t outline, uint16_t fill,
                    uint16_t textcolor, char *label, uint8_t textsize);

  /**************************************************************************/
  /*!
     @brief Initialize to color/size/settings, with upper-left coordinates
     @param gfx        Pointer to our display so we can draw to it!
     @param x1         The X coordinate of the Upper-Left corner of the button
     @param y1         The Y coordinate of the Upper-Left corner of the button
     @param w          Width of the buttton
     @param h          Height of the buttton
     @param outline    Color of the outline (16-bit 5-6-5 standard)
     @param fill       Color of the button fill (16-bit 5-6-5 standard)
     @param textcolor  Color of the button label (16-bit 5-6-5 standard)
     @param label      Ascii string of the text inside the button
     @param textsize_x The font magnification in X-axis of the label text
     @param textsize_y The font magnification in Y-axis of the label text
  */
  /**************************************************************************/
  void initButtonUL(Adafruit_GFX *gfx, int16_t x1, int16_t y1, uint16_t w,
                    uint16_t h, uint16_t outline, uint16_t fill,
                    uint16_t textcolor, char *label, uint8_t textsize_x,
                    uint8_t textsize_y);

  /**************************************************************************/
  /*!
     @brief Draw the button on the screen
     @param inverted  draw with fill and text colors swapped to indicate
     'pressed'
  */
  /**************************************************************************/
  void drawButton(boolean inverted = false);

  /**************************************************************************/
  /*!
      @brief Determine whether a coordinate is inside the button
      @param   x The X coordinate to check
      @param   y The Y coordinate to check
      @returns True if within button graphics outline
  */
  /**************************************************************************/
  boolean contains(int16_t x, int16_t y);

  /**********************************************************************/
  /*!
    @brief    Sets button state, should be done by some touch function
    @param    p  True for pressed, false for not.
  */
  /**********************************************************************/
  void press(boolean p) {
    laststate = currstate;
    currstate = p;
  }

  /**************************************************************************/
  /*!
     @brief    Query whether the button was pressed since we last checked state
     @returns  True if was not-pressed before, now is.
  */
  /**************************************************************************/
  boolean justPressed();

  /**************************************************************************/
  /*!
     @brief    Query whether the button was released since we last checked state
     @returns  True if was pressed before, now is not.
  */
  /**************************************************************************/
  boolean justReleased();

  /**********************************************************************/
  /*!
    @brief    Query whether the button is currently pressed
    @returns  True if pressed
  */
  /**********************************************************************/
  boolean isPressed() { return currstate; };

private:
  Adafruit_GFX *_gfx;
  int16_t _x1; // x coordinate of top-left corner
  int16_t _y1; // y coordinate of top-left corner
  uint16_t _w;
  uint16_t _h;
  uint8_t _textsize_x;
  uint8_t _textsize_y;
  uint16_t _outlinecolor;
  uint16_t _fillcolor;
  uint16_t _textcolor;
  char _label[10];
  boolean currstate;
  boolean laststate;
};

// -------------------------------------------------------------------------

// GFXcanvas1, GFXcanvas8 and GFXcanvas16 (currently a WIP, don't get too
// comfy with the implementation) provide 1-, 8- and 16-bit offscreen
// canvases, the address of which can be passed to drawBitmap() or
// pushColors() (the latter appears only in a couple of GFX-subclassed TFT
// libraries at this time).  This is here mostly to help with the recently-
// added proportionally-spaced fonts; adds a way to refresh a section of the
// screen without a massive flickering clear-and-redraw...but maybe you'll
// find other uses too.  VERY RAM-intensive, since the buffer is in MCU
// memory and not the display driver...GXFcanvas1 might be minimally useful
// on an Uno-class board, but this and the others are much more likely to
// require at least a Mega or various recent ARM-type boards (recommended,
// as the text+bitmap draw can be pokey).  GFXcanvas1 requires 1 bit per
// pixel (rounded up to nearest byte per scanline), GFXcanvas8 is 1 byte
// per pixel (no scanline pad), and GFXcanvas16 uses 2 bytes per pixel (no
// scanline pad).
// NOT EXTENSIVELY TESTED YET.  MAY CONTAIN WORST BUGS KNOWN TO HUMANKIND.

/// A GFX 1-bit canvas context for graphics
class GFXcanvas1 : public Adafruit_GFX {
public:
  /**************************************************************************/
  /*!
     @brief Instatiate a GFX 1-bit canvas context for graphics
     @param w Display width, in pixels
     @param h Display height, in pixels
  */
  /**************************************************************************/
  GFXcanvas1(uint16_t w, uint16_t h);

  /**************************************************************************/
  /*!
     @brief Delete the canvas, free memory
  */
  /**************************************************************************/
  ~GFXcanvas1();

  /**************************************************************************/
  /*!
      @brief  Draw a pixel to the canvas framebuffer
      @param  x     x coordinate
      @param  y     y coordinate
      @param  color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  void drawPixel(int16_t x, int16_t y, uint16_t color);

  /**************************************************************************/
  /*!
      @brief  Fill the framebuffer completely with one color
      @param  color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  void fillScreen(uint16_t color);

  /**********************************************************************/
  /*!
    @brief    Get a pointer to the internal buffer memory
    @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  uint8_t *getBuffer() const { return buffer; }

private:
  uint8_t *buffer;
};

/// A GFX 8-bit canvas context for graphics
class GFXcanvas8 : public Adafruit_GFX {
public:
  /**************************************************************************/
  /*!
     @brief Instatiate a GFX 8-bit canvas context for graphics
     @param w Display width, in pixels
     @param h Display height, in pixels
  */
  /**************************************************************************/
  GFXcanvas8(uint16_t w, uint16_t h);

  /**************************************************************************/
  /*!
     @brief Delete the canvas, free memory
  */
  /**************************************************************************/
  ~GFXcanvas8();

  /**************************************************************************/
  /*!
      @brief  Draw a pixel to the canvas framebuffer
      @param  x     x coordinate
      @param  y     y coordinate
      @param  color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  void drawPixel(int16_t x, int16_t y, uint16_t color);

  /**************************************************************************/
  /*!
      @brief Fill the framebuffer completely with one color
      @param color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  void fillScreen(uint16_t color);

  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

  /**********************************************************************/
  /*!
   @brief    Get a pointer to the internal buffer memory
   @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  uint8_t *getBuffer() const { return buffer; }

private:
  uint8_t *buffer;
};

///  A GFX 16-bit canvas context for graphics
class GFXcanvas16 : public Adafruit_GFX {
public:
  /**************************************************************************/
  /*!
     @brief Instatiate a GFX 16-bit canvas context for graphics
     @param w Display width, in pixels
     @param h Display height, in pixels
  */
  /**************************************************************************/
  GFXcanvas16(uint16_t w, uint16_t h);

  /**************************************************************************/
  /*!
     @brief Delete the canvas, free memory
  */
  /**************************************************************************/
  ~GFXcanvas16();

  /**************************************************************************/
  /*!
      @brief  Draw a pixel to the canvas framebuffer
      @param  x   x coordinate
      @param  y   y coordinate
      @param  color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  void drawPixel(int16_t x, int16_t y, uint16_t color);

  /**************************************************************************/
  /*!
      @brief  Fill the framebuffer completely with one color
      @param  color 16-bit 5-6-5 Color to fill with
  */
  /**************************************************************************/
  void fillScreen(uint16_t color);

  /**************************************************************************/
  /*!
      @brief  Reverses the "endian-ness" of each 16-bit pixel within the canvas.

      Changes little-endian to big-endian, or big-endian to little.
      Most microcontrollers (such as SAMD) are little-endian, while
      most displays tend toward big-endianness. All the drawing
      functions (including RGB bitmap drawing) take care of this
      automatically, but some specialized code (usually involving
      DMA) can benefit from having pixel data already in the
      display-native order.

      @note this does NOT convert to a SPECIFIC endian-ness, it just
      flips the bytes within each word.
  */
  /**************************************************************************/
  void byteSwap();

  /**********************************************************************/
  /*!
    @brief    Get a pointer to the internal buffer memory
    @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  uint16_t *getBuffer() const { return buffer; }

private:
  uint16_t *buffer;
};

#endif // _ADAFRUIT_GFX_H
