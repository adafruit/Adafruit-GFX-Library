/*!
* @file Adafruit_TFT8.cpp
*
* @mainpage Adafruit 8-bit Parallel TFT Displays
*
* @section intro_sec Introduction
* This is our library for TFT Displays using an 8-bit parallel interface
* with address windows and 16 bit color.
*
* These displays use ip to 13 pins to communicate:
*  - 8 data lines (required)
*  - Write strobe (required)
*  - Command/data (required)
*  - Chip select  (optional, can be tied LOW)
*  - Read strobe  (optional)
*  - Reset        (optional, can connect to MCU reset)
*
* Adafruit invests time and resources providing this open source code,
* please support Adafruit and open-source hardware by purchasing
* products from Adafruit!
*
* Written by Limor Fried/Ladyada for Adafruit Industries.
* MIT license, all text above must be included in any redistribution
* @section dependencies Dependencies
*
* This library depends on <a href="https://github.com/adafruit/Adafruit_GFX">
* Adafruit_GFX</a> being present on your system. Please make sure you have
* installed the latest version before using this library.
*
* @section author Author
*
* Written by Limor "ladyada" Fried for Adafruit Industries.
*
* @section license License
*
* BSD license, all text here must be included in any redistribution.
*
*/

#if !defined(__AVR_ATtiny85__) // Not for ATtiny, at all

#include "Adafruit_TFT8.h"

#ifdef PORT_IOBUS
// On SAMD21, redefine digitalPinToPort() to use the slightly-faster
// PORT_IOBUS rather than PORT (not needed on SAMD51).
#undef  digitalPinToPort
#define digitalPinToPort(P) (&(PORT_IOBUS->Group[g_APinDescription[P].ulPort]))
#endif

#ifdef USE_PORT_DMA
  #include <Adafruit_ZeroDMA.h>
  #include <malloc.h> // memalign() function

  // DMA transfer-in-progress indicator and callback
  static volatile boolean dma_busy = false;
  static void dma_callback(Adafruit_ZeroDMA *dma) {
    dma_busy = false;
  }
#endif // USE_PORT_DMA

#if defined(__AVR__)
  #define WR_LOW()        *wrPort &= wrPinMaskClr;
  #define WR_HIGH()       *wrPort |= wrPinMaskSet;
  #define DC_LOW()        *dcPort &= dcPinMaskClr;
  #define DC_HIGH()       *dcPort |= dcPinMaskSet;
  #define CS_LOW()        if(_cs >= 0) *csPort &= csPinMaskClr;
  #define CS_HIGH()       if(_cs >= 0) *csPort |= csPinMaskSet;
  #define RD_LOW()        *rdPort &= rdPinMaskClr;
  #define RD_HIGH()       *rdPort |= rdPinMaskSet;
  #define PORT_OUTPUT()   *portDir = 0xFF;
  #define PORT_INPUT()    *portDir = 0x00;
#else
  #define WR_LOW()        *wrPortClr = wrPinMask;
  #define WR_HIGH()       *wrPortSet = wrPinMask;
  #define DC_LOW()        *dcPortClr = dcPinMask;
  #define DC_HIGH()       *dcPortSet = dcPinMask;
  #define CS_LOW()        if(_cs >= 0) *csPortClr = csPinMask;
  #define CS_HIGH()       if(_cs >= 0) *csPortSet = csPinMask;
  #define RD_LOW()        *rdPortClr = rdPinMask;
  #define RD_HIGH()       *rdPortSet = rdPinMask;
  #define PORT_OUTPUT()   *dirSet    = 0xFF;
  #define PORT_INPUT()    *dirClr    = 0xFF;
  #define PORT_OUTPUT16() *(volatile uint16_t *)dirSet = 0xFFFF;
  #define PORT_INPUT16()  *(volatile uint16_t *)dirClr = 0xFFFF;
#endif
#define WR_STROBE() { WR_LOW(); WR_HIGH(); }

/*!
    @brief  Instantiate Adafruit TFT8 display driver.
    @param  w     Display width in pixels.
    @param  h     Display height in pixels.
    @param  D0    Arduino pin # for data bit 0 (1+ are extrapolated).
                  The 8 data bits MUST be contiguous and byte-aligned
                  (word-aligned for 'wide' interface) within the same
                  PORT register (may not correspond to Arduino pin sequence).
    @param  WR    Arduino pin # for write strobe.
    @param  DC    Arduino pin # for data/command.
    @param  CS    Arduino pin # for chip select (-1 if unused, tie CS low).
    @param  RST   Arduino pin # for reset (-1 if unused, tie to MCU reset).
    @param  RD    Arduino pin # for read strobe (-1 if unused).
    @param  wide  If true, use 16-bit wide interface (not on AVR).
*/
Adafruit_TFT8::Adafruit_TFT8(uint16_t w, uint16_t h,
  int8_t D0, int8_t WR, int8_t DC, int8_t CS, int8_t RST, int8_t RD,
  bool wide) : Adafruit_GFX(w, h),
  _d0(D0), _wr(WR), _dc(DC), _cs(CS), _rst(RST), _rd(RD)
{
#if defined(__AVR__)
    if(digitalPinToBitMask(D0) != 1) return; // D0 MUST be port bit 0
    _d0          = D0; // Save D0 pin to indicate valid alignment
    wrPort       = (PORTreg_t)portOutputRegister(digitalPinToPort(WR));
    wrPinMaskSet = digitalPinToBitMask(WR);
    dcPort       = (PORTreg_t)portOutputRegister(digitalPinToPort(DC));
    dcPinMaskSet = digitalPinToBitMask(DC);
    if(CS >= 0) {
      csPort       = (PORTreg_t)portOutputRegister(digitalPinToPort(CS));
      csPinMaskSet = digitalPinToBitMask(CS);
    } else {
      // No chip-select line defined; might be permanently tied to GND.
      // Assign a valid GPIO register (though not used for CS), and an
      // empty pin bitmask...the nonsense bit-twiddling might be faster
      // than checking _cs and possibly branching.
      csPort       = dcPort;
      csPinMaskSet = 0;
    }
    if(RD >= 0) {
      rdPort       = (PORTreg_t)portOutputRegister(digitalPinToPort(RD));
      rdPinMaskSet = digitalPinToBitMask(RD);
    } else {
      // No read-strobe line defined; similar to CS case above
      rdPort       = dcPort;
      rdPinMaskSet = 0;
    }
    wrPinMaskClr = ~wrPinMaskSet;
    dcPinMaskClr = ~dcPinMaskSet;
    csPinMaskClr = ~csPinMaskSet;
    rdPinMaskClr = ~rdPinMaskSet;
    writePort    = (PORTreg_t)portOutputRegister(digitalPinToPort(D0));
    readPort     = (PORTreg_t)portInputRegister(digitalPinToPort(D0));
    portDir      = (PORTreg_t)portModeRegister(digitalPinToPort(D0));
#else
    // Confirm D0 bit is byte- or word-aligned in PORT...
    if(g_APinDescription[D0].ulPin & (wide ? 15 : 7)) return;
    _d0       = D0; // Save D0 pin to indicate valid alignment
    _wide     = wide;
    wrPinMask = digitalPinToBitMask(WR);
    wrPortSet = &(PORT->Group[g_APinDescription[WR].ulPort].OUTSET.reg);
    wrPortClr = &(PORT->Group[g_APinDescription[WR].ulPort].OUTCLR.reg);
    dcPinMask = digitalPinToBitMask(DC);
    dcPortSet = &(PORT->Group[g_APinDescription[DC].ulPort].OUTSET.reg);
    dcPortClr = &(PORT->Group[g_APinDescription[DC].ulPort].OUTCLR.reg);
    if(CS >= 0) { // If chip-select pin is specified...
      csPinMask = digitalPinToBitMask(CS);
      csPortSet = &(PORT->Group[g_APinDescription[CS].ulPort].OUTSET.reg);
      csPortClr = &(PORT->Group[g_APinDescription[CS].ulPort].OUTCLR.reg);
    } else {
      // No chip-select line defined; might be permanently tied to GND.
      // Assign a valid GPIO register (though not used for CS), and an
      // empty pin bitmask...the nonsense bit-twiddling might be faster
      // than checking _cs and possibly branching.
      csPinMask = 0;
      csPortSet = dcPortSet;
      csPortClr = dcPortClr;
    }
    if(RD >= 0) { // If read-strobe pin is specified...
      rdPinMask = digitalPinToBitMask(RD);
      rdPortSet = &(PORT->Group[g_APinDescription[RD].ulPort].OUTSET.reg);
      rdPortClr = &(PORT->Group[g_APinDescription[RD].ulPort].OUTCLR.reg);
    } else {
      rdPinMask = 0;
      rdPortSet = dcPortSet;
      rdPortClr = dcPortClr;
    }

    // Get pointers to PORT write/read/dir bytes within 32-bit PORT
    uint8_t    dBit    = g_APinDescription[_d0].ulPin; // d0 bit # in PORT
    PortGroup *p       = (&(PORT->Group[g_APinDescription[_d0].ulPort]));
    uint8_t    offset  = dBit / 8; // d[7:0] byte # within PORT
    if(wide)   offset &= ~1; // d[15:8] byte # within PORT
    // These are all uint8_t* pointers -- elsewhere they're recast
    // as necessary if a 'wide' 16-bit interface is in use.
    writePort = (volatile uint8_t *)&(p->OUT.reg)    + offset;
    readPort  = (volatile uint8_t *)&(p->IN.reg)     + offset;
    dirSet    = (volatile uint8_t *)&(p->DIRSET.reg) + offset;
    dirClr    = (volatile uint8_t *)&(p->DIRCLR.reg) + offset;
#endif
}

/*!
    @brief  Initialiaze hardware interface.
*/
bool Adafruit_TFT8::init(void) {

  if(_d0 < 0) return false; // Bad alignment in constructor

  // Initialize data pins.  We were only passed d0, so scan
  // the pin description list looking for the other pins.
  // They'll be on the same PORT, and within the next 7 (or 15) bits
  // (because we need to write to a contiguous PORT byte or word).
#if defined(__AVR__)
  // PORT registers are 8 bits wide, so just need a register match...
  for(uint8_t i=0; i<NUM_DIGITAL_PINS; i++) {
    if((PORTreg_t)portOutputRegister(digitalPinToPort(i)) == writePort) {
      pinMode(i, OUTPUT);
      digitalWrite(i, LOW);
    }
  }
#else
  uint8_t portNum = g_APinDescription[_d0].ulPort, // d0 PORT #
          dBit    = g_APinDescription[_d0].ulPin,  // d0 bit # in PORT
          lastBit = dBit + (_wide ? 15 : 7);
  for(uint8_t i=0; i<PINS_COUNT; i++) {
    if((g_APinDescription[i].ulPort == portNum ) &&
       (g_APinDescription[i].ulPin  >= dBit    ) &&
       (g_APinDescription[i].ulPin  <= (uint32_t)lastBit)) {
          pinMode(i, OUTPUT);
          digitalWrite(i, LOW);
    }
  }
#endif

  // Initialize control signal pins, all active LOW
  pinMode(_wr, OUTPUT);
  digitalWrite(_wr, HIGH);
  pinMode(_dc, OUTPUT);
  digitalWrite(_dc, HIGH);
  if(_cs >= 0) {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH); // Deselect
  }
  if(_rd >= 0) {
    pinMode(_rd, OUTPUT);
    digitalWrite(_rd, HIGH);
  }
  if(_rst >= 0) {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW); // Toggle RST low to reset
    delay(100);
    digitalWrite(_rst, HIGH);
    delay(200);
  }

  return true;
}

/*!
    @brief  Initiate write (or read!) operation.
*/
inline void Adafruit_TFT8::startWrite(void) {
  CS_LOW(); // Chip select LOW
}

/*!
    @brief  End write (or read!) operation.
*/
inline void Adafruit_TFT8::endWrite(void) {
  CS_HIGH(); // Chip select HIGH
}

/*!
    @brief  Write one byte to hardware interface.
    @param  b  One byte to send, MSB order
*/
void Adafruit_TFT8::write8(uint8_t b) {
#if defined(__AVR__)
  *writePort = b;
#else
  if(!_wide) {
    *writePort = b;
  } else {
    *(volatile uint16_t *)writePort = b;
  }
#endif
  WR_STROBE(); // Write strobe LOW, HIGH
}

/*!
    @brief  Write one word to hardware interface.
    @param  w  One word to send, MSB order
*/
void Adafruit_TFT8::write16(uint16_t w) {
#if defined(__AVR__)
  *writePort = w >> 8;   // MSB
  WR_STROBE();           // Write strobe LOW, HIGH
  *writePort = w;        // LSB
  WR_STROBE();           // Write strobe LOW, HIGH
#else
  if(!_wide) {
    *writePort = w >> 8; // MSB
    WR_STROBE();         // Write strobe LOW, HIGH
    *writePort = w;      // LSB
  } else {
    *(volatile uint16_t *)writePort = w;
  }
  WR_STROBE();           // Write strobe LOW, HIGH
#endif
}

/*!
    @brief  Write a command byte.
    @param  cmd  The 8-bit command to send.
*/
void Adafruit_TFT8::writeCommand(uint8_t cmd) {
    DC_LOW();    // Data/Command LOW (command mode)
    write8(cmd); // Issue value
    DC_HIGH();   // Data/Command HIGH (data mode)
}

/*!
    @brief    Read one byte or word from TFT interface.
    @returns  One byte or word, native order.
*/
uint16_t Adafruit_TFT8::read(void) {
  uint16_t r = 0;
  if(_rd >= 0) {
#if defined(__AVR__)
    PORT_INPUT();      // Set port to INPUT
    RD_LOW();          // Read strobe LOW
    r = *readPort;     // Read value from port
    RD_HIGH();         // Read strobe HIGH
    PORT_OUTPUT();     // Set port back to OUTPUT
#else
    if(!_wide) {
      PORT_INPUT();    // Set port to INPUT
      RD_LOW();        // Read strobe LOW
      r = *readPort;   // Read value from port
      RD_HIGH();       // Read strobe HIGH
      PORT_OUTPUT();   // Set port back to OUTPUT
    } else {
      PORT_INPUT16();  // Set port to INPUT (16-bit)
      RD_LOW();        // Read strobe LOW
      r = *(volatile uint16_t *)readPort; // Read value from port
      RD_HIGH();       // Read strobe HIGH
      PORT_OUTPUT16(); // Set port back to OUTPUT (16-bit)
    }
#endif
  }
  return r;
}

/*!
    @brief  Draw a single pixel.
    @param  x      X coordinate.
    @param  y      Y coordinate.
    @param  color  16-bit 5-6-5 pixel color.
*/
void Adafruit_TFT8::drawPixel(int16_t x, int16_t y, uint16_t color) {
    // Clip first...
    if((x >= 0) && (x < _width) && (y >= 0) && (y < _height)) {
        // THEN device-select and draw...
        startWrite();
        setAddrWindow(x, y, 1, 1);
        writePixel(color);
        endWrite();
    }
}




/*!
    @brief   Converts 8-bit (each) R,G,B color to 16-bit packed 5-6-5 value.
    @param   red   Red level, 0 to 255
    @param   green Green level, 0 to 255
    @param   blue  Blue level, 0 to 255
    @return  Unsigned 16-bit decimated color in "5-6-5" format
*/
uint16_t Adafruit_TFT8::color565(uint8_t red, uint8_t green, uint8_t blue) {
  return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
}


#if 0






/*!
    @brief  Issue multiple 2-byte colors.
    @param  colors  Array of 16-bit 5-6-5 Colors to draw.
    @param  len     How many pixels to draw.
*/
void Adafruit_TFT8::writePixels(uint16_t *colors, uint32_t len) {
    SPI_WRITE_PIXELS((uint8_t*)colors , len * 2);
}

/*!
    @brief  Issue a 2-byte color many times.
    @param  color  The 16-bit 5-6-5 Color to draw.
    @param  len    How many pixels to draw.
*/
void Adafruit_TFT8::writeColor(uint16_t color, uint32_t len) {

  if(!len) return; // Avoid 0-byte transfers

  uint8_t hi = color >> 8, lo = color;

  if(hi != lo) {
  } else {
    len *= 2;
// Issue as bytes
  }
}

/**************************************************************************/
/*!
   @brief    Write a pixel (must have a transaction in progress)
    @param   x   x coordinate
    @param   y   y coordinate
   @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void Adafruit_SPITFT::writePixel(int16_t x, int16_t y, uint16_t color) {
    if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
    setAddrWindow(x,y,1,1);
    writePixel(color);
}

/**************************************************************************/
/*!
   @brief    Write a filled rectangle (must have a transaction in progress)
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void Adafruit_SPITFT::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
    if((x >= _width) || (y >= _height)) return;
    int16_t x2 = x + w - 1, y2 = y + h - 1;
    if((x2 < 0) || (y2 < 0)) return;

    // Clip left/top
    if(x < 0) {
        x = 0;
        w = x2 + 1;
    }
    if(y < 0) {
        y = 0;
        h = y2 + 1;
    }

    // Clip right/bottom
    if(x2 >= _width)  w = _width  - x;
    if(y2 >= _height) h = _height - y;

    setAddrWindow(x, y, w, h);
    writeColor(color, (int32_t)w * h);
}

/**************************************************************************/
/*!
   @brief    Write a perfectly vertical line (must have a transaction in progress)
    @param    x   Top-most x coordinate
    @param    y   Top-most y coordinate
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void inline Adafruit_SPITFT::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color){
    writeFillRect(x, y, 1, h, color);
}

/**************************************************************************/
/*!
   @brief    Write a perfectly horizontal line (must have a transaction in progress)
    @param    x   Left-most x coordinate
    @param    y   Left-most y coordinate
    @param    w   Width in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void inline Adafruit_SPITFT::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color){
    writeFillRect(x, y, w, 1, color);
}

/**************************************************************************/
/*!
   @brief    Write a perfectly vertical line - sets up transaction
    @param    x   Top-most x coordinate
    @param    y   Top-most y coordinate
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void Adafruit_SPITFT::drawFastVLine(int16_t x, int16_t y,
        int16_t h, uint16_t color) {
    startWrite();
    writeFastVLine(x, y, h, color);
    endWrite();
}

/**************************************************************************/
/*!
   @brief    Write a perfectly horizontal line - sets up transaction
    @param    x   Left-most x coordinate
    @param    y   Left-most y coordinate
    @param    w   Width in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void Adafruit_SPITFT::drawFastHLine(int16_t x, int16_t y,
        int16_t w, uint16_t color) {
    startWrite();
    writeFastHLine(x, y, w, color);
    endWrite();
}

/**************************************************************************/
/*!
   @brief    Fill a rectangle completely with one color.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void Adafruit_SPITFT::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color) {
    startWrite();
    writeFillRect(x,y,w,h,color);
    endWrite();
}


/**************************************************************************/
/*!
    @brief      Invert the display using built-in hardware command
    @param   i  True if you want to invert, false to make 'normal'
*/
/**************************************************************************/
void Adafruit_SPITFT::invertDisplay(boolean i) {
  startWrite();
  writeCommand(i ? invertOnCommand : invertOffCommand);
  endWrite();
}


/**************************************************************************/
/*!
   @brief   Draw a 16-bit image (RGB 5/6/5) at the specified (x,y) position.  
   For 16-bit display devices; no color reduction performed.
   Adapted from https://github.com/PaulStoffregen/ILI9341_t3 
   by Marc MERLIN. See examples/pictureEmbed to use this.
   5/6/2017: function name and arguments have changed for compatibility
   with current GFX library and to avoid naming problems in prior
   implementation.  Formerly drawBitmap() with arguments in different order.

    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    pcolors  16-bit array with 16-bit color bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
*/
/**************************************************************************/
void Adafruit_SPITFT::drawRGBBitmap(int16_t x, int16_t y,
  uint16_t *pcolors, int16_t w, int16_t h) {

    int16_t x2, y2; // Lower-right coord
    if(( x             >= _width ) ||      // Off-edge right
       ( y             >= _height) ||      // " top
       ((x2 = (x+w-1)) <  0      ) ||      // " left
       ((y2 = (y+h-1)) <  0)     ) return; // " bottom

    int16_t bx1=0, by1=0, // Clipped top-left within bitmap
            saveW=w;      // Save original bitmap width value
    if(x < 0) { // Clip left
        w  +=  x;
        bx1 = -x;
        x   =  0;
    }
    if(y < 0) { // Clip top
        h  +=  y;
        by1 = -y;
        y   =  0;
    }
    if(x2 >= _width ) w = _width  - x; // Clip right
    if(y2 >= _height) h = _height - y; // Clip bottom

    pcolors += by1 * saveW + bx1; // Offset bitmap ptr to clipped top-left
    startWrite();
    setAddrWindow(x, y, w, h); // Clipped area
    while(h--) { // For each (clipped) scanline...
      writePixels(pcolors, w); // Push one (clipped) row
      pcolors += saveW; // Advance pointer by one full (unclipped) line
    }
    endWrite();
}






#endif // 0




#endif // !__AVR_ATtiny85__

