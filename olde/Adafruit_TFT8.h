#ifndef _ADAFRUIT_TFT8_H_
#define _ADAFRUIT_TFT8_H_

#if !defined(__AVR_ATtiny85__) // Not fot ATtiny, at all

#include "Adafruit_GFX.h"

//#define USE_PORT_DMA ///< If set, use PORT DMA if available
// If DMA is enabled, Arduino sketch MUST #include <Adafruit_ZeroDMA.h>

#if !defined(__SAMD51__)
 #undef USE_PORT_DMA   ///< Only for SAMD51 chips (SAMD21 lacks PORT DMA)
#endif

#ifdef USE_PORT_DMA
 #pragma message ("PORT DMA IS ENABLED. HIGHLY EXPERIMENTAL.")
 #include <Adafruit_ZeroDMA.h>
#endif

// Unlike SPITFT, "USE_FAST_PINIO" (direct PORT access) is IMPLICIT here,
// there is no digitalWrite() option. If a device doesn't support direct
// PORT writes, it's not supported by this code...it would be hopelessly
// slow and lose any parallel interface benefit...use SPI instead!

#if defined(__AVR__)
 typedef uint8_t  PORT_t;            ///< PORT values are 8-bit
#else
 typedef uint32_t PORT_t;            ///< PORT values are 32-bit
#endif
typedef volatile  PORT_t* PORTreg_t; ///< PORT register type

/// An optimized parallel display subclass of GFX.
class Adafruit_TFT8 : public Adafruit_GFX {

  public:

    Adafruit_TFT8(uint16_t w, uint16_t h, int8_t D0, int8_t WR, int8_t DC,
      int8_t CS = -1, int8_t RST = -1, int8_t RD = -1, bool wide = false);

    bool         init(void);
    inline void  startWrite(void);
    inline void  endWrite(void);
    void         write8(uint8_t b);
    void         write16(uint16_t w);
    void         writeCommand(uint8_t cmd);
    uint16_t     read(void);

    virtual bool begin() = 0;  ///< Subclass MUST provide begin() func
    virtual void setAddrWindow(
      uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;
    void         drawPixel(int16_t x, int16_t y, uint16_t color);
    uint16_t     color565(uint8_t r, uint8_t g, uint8_t b);




#if 0

        // Transaction API
// These are probably going away -- we'll just have draw funcs

        void      writePixel(int16_t x, int16_t y, uint16_t color);
        void      writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        void      writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
        void      writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

        // Transaction API not used by GFX

	/*!
	  @brief   SPI displays set an address window rectangle for blitting pixels
	  @param  x  Top left corner x coordinate
	  @param  y  Top left corner x coordinate
	  @param  w  Width of window
	  @param  h  Height of window
	*/
	virtual void      setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;

	/*!
	  @brief   Write a 2-byte color  (must have a transaction in progress)
	  @param    color 16-bit 5-6-5 Color to draw
	*/
	void      inline writePixel(uint16_t color) { SPI_WRITE16(color); }
        void      writePixels(uint16_t * colors, uint32_t len);
        void      writeColor(uint16_t color, uint32_t len);

        // Recommended Non-Transaction
        void      drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
        void      drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
        void      fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

        using     Adafruit_GFX::drawRGBBitmap; // Check base class first
        void      drawRGBBitmap(int16_t x, int16_t y,
                    uint16_t *pcolors, int16_t w, int16_t h);

#endif // 0

  protected:

    int8_t
      _d0 = -1,        ///< Arduino pin # for data bit 0 (1+ are extrapolated)
      _wr,             ///< Arduino pin # for write strobe
      _dc,             ///< Arduino pin # for data/command
      _cs,             ///< Arduino pin # for chip select (-1 if unused)
      _rst,            ///< Arduino pin # for reset (-1 if unused)
      _rd;             ///< Arduino pin # for read strobe (-1 if unused)
#if defined(__AVR__)
    PORT_t
      wrPinMaskSet,    ///< Bitmask for write strobe SET (OR bitmask)
      wrPinMaskClr,    ///< Bitmask for write strobe CLEAR (AND bitmask)
      dcPinMaskSet,    ///< Bitmask for data/command SET (OR bitmask)
      dcPinMaskClr,    ///< Bitmask for data/command CLEAR (AND bitmask)
      csPinMaskSet,    ///< Bitmask for chip select SET (OR bitmask)
      csPinMaskClr,    ///< Bitmask for chip select CLEAR (AND bitmask)
      rdPinMaskSet,    ///< Bitmask for read strobe SET (OR bitmask)
      rdPinMaskClr;    ///< Bitmask for read strobe CLEAR (AND bitmask)
    PORTreg_t
      wrPort,          ///< PORT register for write strobe
      dcPort,          ///< PORT register for data/command
      csPort,          ///< PORT register for chip select
      rdPort,          ///< PORT register for read strobe
      portDir;         ///< PORT direction register
#else
    PORT_t
      wrPinMask,       ///< Bitmask for write strobe
      dcPinMask,       ///< Bitmask for data/command
      csPinMask,       ///< Bitmask for chip select
      rdPinMask;       ///< Bitmask for read strobe
    PORTreg_t
      wrPortSet,       ///< PORT register for write strobe SET
      wrPortClr,       ///< PORT register for write strobe CLEAR
      dcPortSet,       ///< PORT register for data/command SET
      dcPortClr,       ///< PORT register for data/command CLEAR
      csPortSet,       ///< PORT register for chip select SET
      csPortClr,       ///< PORT register for chip select CLEAR
      rdPortSet,       ///< PORT register for read strobe SET
      rdPortClr;       ///< PORT register for read strobe CLEAR
    bool
      _wide   = false; ///< 16-bit interface (not avail on AVR)
    volatile uint8_t   // Always uint8_t regardless of PORT_t...
      *dirSet,         ///< PORT byte data direction SET
      *dirClr;         ///< PORT byte data direction CLEAR
#endif
    volatile uint8_t   // Always uint8_t regardless of PORT_t...
      *writePort,      ///< PORT byte for DATA WRITE
      *readPort;       ///< PORT byte for DATA READ
    int16_t
      _xstart = 0,     ///< Internal framebuffer X offset
      _ystart = 0;     ///< Internal framebuffer Y offset

#ifdef USE_PORT_DMA
    Adafruit_ZeroDMA dma;                  ///< DMA instance
    DmacDescriptor  *dptr          = NULL; ///< 1st descriptor
    DmacDescriptor  *descriptor    = NULL; ///< Allocated descriptor list
    uint16_t        *pixelBuf[2];          ///< Working buffers
    uint16_t         maxFillLen;           ///< Max pixels per DMA xfer
    uint16_t         lastFillColor = 0;    ///< Last color used w/fill
    uint32_t         lastFillLen   = 0;    ///< # of pixels w/last fill
    uint8_t          onePixelBuf;          ///< For hi==lo fill
#endif
};

#define writePixel(color) write16(color)

#endif // !__AVR_ATtiny85__

#endif // !_ADAFRUIT_TFT8_H_
