/*!
* @file Adafruit_SPITFT.cpp
*
* @mainpage Adafruit SPI TFT Displays
*
* @section intro_sec Introduction
  This is our library for generic SPI TFT Displays with
  address windows and 16 bit color (e.g. ILI9341, HX8357D, ST7735...)

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
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

#if !defined(__AVR_ATtiny85__) // NOT A CHANCE of this stuff working on ATtiny

#include "Adafruit_SPITFT.h"
#if !defined(ARDUINO_STM32_FEATHER)
  #include "pins_arduino.h"
#endif
#if !defined(ARDUINO_STM32_FEATHER) && !defined(RASPI)
  #include "wiring_private.h"
#endif
#include <limits.h>

#ifdef PORT_IOBUS
// On SAMD21, redefine digitalPinToPort() to use the slightly-faster
// PORT_IOBUS rather than PORT (not needed on SAMD51).
#undef  digitalPinToPort
#define digitalPinToPort(P) (&(PORT_IOBUS->Group[g_APinDescription[P].ulPort]))
#endif

#include "Adafruit_SPITFT_Macros.h"

#ifdef USE_SPI_DMA
#include <Adafruit_ZeroDMA.h>
#include <malloc.h> // memalign() function

// DMA transfer-in-progress indicator and callback
static volatile boolean dma_busy = false;
static void dma_callback(Adafruit_ZeroDMA *dma) {
    dma_busy = false;
}

#endif // USE_SPI_DMA

/**************************************************************************/
/*!
    @brief  Pass 8-bit (each) R,G,B, get back 16-bit packed color
            This function converts 8-8-8 RGB data to 16-bit 5-6-5
    @param    red   Red 8 bit color
    @param    green Green 8 bit color
    @param    blue  Blue 8 bit color
    @return   Unsigned 16-bit down-sampled color in 5-6-5 format
*/
/**************************************************************************/
uint16_t Adafruit_SPITFT::color565(uint8_t red, uint8_t green, uint8_t blue) {
    return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
}


/**************************************************************************/
/*!
    @brief  Instantiate Adafruit SPI display driver with software SPI
    @param    w     Display width in pixels
    @param    h     Display height in pixels
    @param    cs    Chip select pin #
    @param    dc    Data/Command pin #
    @param    mosi  SPI MOSI pin #
    @param    sclk  SPI Clock pin #
    @param    rst   Reset pin # (optional, pass -1 if unused)
    @param    miso  SPI MISO pin # (optional, pass -1 if unused)
*/
/**************************************************************************/
Adafruit_SPITFT::Adafruit_SPITFT(uint16_t w, uint16_t h,
				 int8_t cs, int8_t dc, int8_t mosi,
				 int8_t sclk, int8_t rst, int8_t miso) 
  : Adafruit_GFX(w, h) {
    _cs   = cs;
    _dc   = dc;
    _rst  = rst;
    _sclk = sclk;
    _mosi = mosi;
    _miso = miso;
    _freq = 0;
#ifdef USE_FAST_PINIO
    dcport      = (RwReg *)portOutputRegister(digitalPinToPort(dc));
    dcpinmask   = digitalPinToBitMask(dc);
    clkport     = (RwReg *)portOutputRegister(digitalPinToPort(sclk));
    clkpinmask  = digitalPinToBitMask(sclk);
    mosiport    = (RwReg *)portOutputRegister(digitalPinToPort(mosi));
    mosipinmask = digitalPinToBitMask(mosi);
    if(miso >= 0){
        misoport    = (RwReg *)portInputRegister(digitalPinToPort(miso));
        misopinmask = digitalPinToBitMask(miso);
    } else {
        misoport    = 0;
        misopinmask = 0;
    }
    if(cs >= 0) {
        csport    = (RwReg *)portOutputRegister(digitalPinToPort(cs));
        cspinmask = digitalPinToBitMask(cs);
    } else {
        // No chip-select line defined; might be permanently tied to GND.
        // Assign a valid GPIO register (though not used for CS), and an
        // empty pin bitmask...the nonsense bit-twiddling might be faster
        // than checking _cs and possibly branching.
        csport    = dcport;
        cspinmask = 0;
    }
#endif
}

/**************************************************************************/
/*!
    @brief  Instantiate Adafruit SPI display driver with hardware SPI
    @param    w     Display width in pixels
    @param    h     Display height in pixels
    @param    cs    Chip select pin #
    @param    dc    Data/Command pin #
    @param    rst   Reset pin # (optional, pass -1 if unused)
*/
/**************************************************************************/
Adafruit_SPITFT::Adafruit_SPITFT(uint16_t w, uint16_t h,
				 int8_t cs, int8_t dc, int8_t rst) 
  : Adafruit_SPITFT(w, h, &SPI, cs, dc, rst) 
{
  // We just call the hardware SPI instantiator with the default SPI device (&SPI)
}

/**************************************************************************/
/*!
    @brief  Instantiate Adafruit SPI display driver with hardware SPI
    @param    w     Display width in pixels
    @param    h     Display height in pixels
    @param    spiClass A pointer to an SPI hardware interface, e.g. &SPI1
    @param    cs    Chip select pin #
    @param    dc    Data/Command pin #
    @param    rst   Reset pin # (optional, pass -1 if unused)
*/
/**************************************************************************/
Adafruit_SPITFT::Adafruit_SPITFT(uint16_t w, uint16_t h, SPIClass *spiClass,
				 int8_t cs, int8_t dc, int8_t rst) 
  : Adafruit_GFX(w, h) {
    _cs   = cs;
    _dc   = dc;
    _rst  = rst;
    _spi = spiClass;
    _sclk = -1;
    _mosi = -1;
    _miso = -1;
    _freq = 0;
#ifdef USE_FAST_PINIO
    clkport     = 0;
    clkpinmask  = 0;
    mosiport    = 0;
    mosipinmask = 0;
    misoport    = 0;
    misopinmask = 0;
    dcport      = (RwReg *)portOutputRegister(digitalPinToPort(dc));
    dcpinmask   = digitalPinToBitMask(dc);
    if(cs >= 0) {
        csport    = (RwReg *)portOutputRegister(digitalPinToPort(cs));
        cspinmask = digitalPinToBitMask(cs);
    } else {
        // See notes in prior constructor.
        csport    = dcport;
        cspinmask = 0;
    }
#endif
}


/**************************************************************************/
/*!
    @brief   Initialiaze the SPI interface (hardware or software)
    @param    freq  The desired maximum SPI hardware clock frequency
*/
/**************************************************************************/
void Adafruit_SPITFT::initSPI(uint32_t freq) {
    _freq = freq;

    // Control Pins
    if(_cs >= 0) {
        pinMode(_cs, OUTPUT);
        digitalWrite(_cs, HIGH); // Deselect
    }
    pinMode(_dc, OUTPUT);
    digitalWrite(_dc, LOW);

    // Software SPI
    if(_sclk >= 0){
        pinMode(_mosi, OUTPUT);
        digitalWrite(_mosi, LOW);
        pinMode(_sclk, OUTPUT);
        digitalWrite(_sclk, HIGH);
        if(_miso >= 0){
            pinMode(_miso, INPUT);
        }
    }

    // Hardware SPI
    SPI_BEGIN();

    // toggle RST low to reset
    if (_rst >= 0) {
        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, HIGH);
        delay(100);
        digitalWrite(_rst, LOW);
        delay(100);
        digitalWrite(_rst, HIGH);
        delay(200);
    }

#ifdef USE_SPI_DMA

    // INITIALIZE DMA

    if(dma.allocate() == DMA_STATUS_OK) { // Allocate channel
        // The DMA library needs to allocate at least one valid descriptor,
        // so we do that here. It's not used in the usual sense though,
        // just before a transfer we copy descriptor[0] to this address.
        if(dptr = dma.addDescriptor(NULL, NULL, 42, DMA_BEAT_SIZE_BYTE,
          false, false)) {
            // Allocate 2 scanlines worth of pixels on display's major axis,
            // whichever that is, rounding each up to 2-pixel boundary.
            int  major = (WIDTH > HEIGHT) ? WIDTH : HEIGHT;
            major     += (major & 1); // -> next 2-pixel bound, if needed.
            maxFillLen = major * 2;   // 2 scanlines
            // Note to future self: if you decide to make the pixel buffer
            // much larger, remember that DMA transfer descriptors can't
            // exceed 65,535 bytes (not 65,536), meaning 32,767 pixels tops.
            // Not that we have that kind of RAM to throw around right now.
            if((pixelBuf[0] =
              (uint16_t *)malloc(maxFillLen * sizeof(uint16_t)))) {
                // Alloc OK. Get pointer to start of second scanline.
                pixelBuf[1] = &pixelBuf[0][major];
                // Determine number of DMA descriptors needed to cover
                // entire screen when entire 2-line pixelBuf is used
                // (round up for fractional last descriptor).
                int numDescriptors = (WIDTH * HEIGHT + (maxFillLen - 1)) /
                      maxFillLen;
                // DMA descriptors MUST be 128-bit (16 byte) aligned.
                // memalign() is considered 'obsolete' but it's replacements
                // (aligned_alloc() or posix_memalign()) are not currently
                // available in the version of ARM GCC in use, but this is,
                // so here we are.
                if((descriptor = (DmacDescriptor *)memalign(16,
                  numDescriptors * sizeof(DmacDescriptor)))) {
                    int                dmac_id;
                    volatile uint32_t *data_reg;

                    // THIS IS AN AFFRONT TO NATURE, but I don't know
                    // any "clean" way to get the sercom number from the
                    // SPIClass pointer (e.g. &SPI or &SPI1), which is
                    // all we have to work with. SPIClass does contain
                    // a SERCOM pointer but it is a PRIVATE member!
                    // Doing an UNSPEAKABLY HORRIBLE THING here, directly
                    // accessing the first 32-bit value in the SPIClass
                    // structure, knowing that's (currently) where the
                    // SERCOM pointer lives, but this ENTIRELY DEPENDS
                    // on that structure not changing nor the compiler
                    // rearranging things. Oh the humanity!

                    if(*(SERCOM **)_spi == &sercom0) {
                        dmac_id  = SERCOM0_DMAC_ID_TX;
                        data_reg = &SERCOM0->SPI.DATA.reg;
#if defined SERCOM1
                    } else if(*(SERCOM **)_spi == &sercom1) {
                        dmac_id  = SERCOM1_DMAC_ID_TX;
                        data_reg = &SERCOM1->SPI.DATA.reg;
#endif
#if defined SERCOM2
                    } else if(*(SERCOM **)_spi == &sercom2) {
                        dmac_id  = SERCOM2_DMAC_ID_TX;
                        data_reg = &SERCOM2->SPI.DATA.reg;
#endif
#if defined SERCOM3
                    } else if(*(SERCOM **)_spi == &sercom3) {
                        dmac_id  = SERCOM3_DMAC_ID_TX;
                        data_reg = &SERCOM3->SPI.DATA.reg;
#endif
#if defined SERCOM4
                    } else if(*(SERCOM **)_spi == &sercom4) {
                        dmac_id  = SERCOM4_DMAC_ID_TX;
                        data_reg = &SERCOM4->SPI.DATA.reg;
#endif
#if defined SERCOM5
                    } else if(*(SERCOM **)_spi == &sercom5) {
                        dmac_id  = SERCOM5_DMAC_ID_TX;
                        data_reg = &SERCOM5->SPI.DATA.reg;
#endif
                    }

                    dma.setPriority(DMA_PRIORITY_3);
                    dma.setTrigger(dmac_id);
                    dma.setAction(DMA_TRIGGER_ACTON_BEAT);

                    // Initialize descriptor list.
                    for(int d=0; d<numDescriptors; d++) {
                        // No need to set SRCADDR, DESCADDR or BTCNT --
                        // those are done in the pixel-writing functions.
                        descriptor[d].BTCTRL.bit.VALID    = true;
                        descriptor[d].BTCTRL.bit.EVOSEL   =
                          DMA_EVENT_OUTPUT_DISABLE;
                        descriptor[d].BTCTRL.bit.BLOCKACT =
                           DMA_BLOCK_ACTION_NOACT;
                        descriptor[d].BTCTRL.bit.BEATSIZE = DMA_BEAT_SIZE_BYTE;
                        descriptor[d].BTCTRL.bit.DSTINC   = 0;
                        descriptor[d].BTCTRL.bit.STEPSEL  = DMA_STEPSEL_SRC;
                        descriptor[d].BTCTRL.bit.STEPSIZE =
                          DMA_ADDRESS_INCREMENT_STEP_SIZE_1;
                        descriptor[d].DSTADDR.reg         = (uint32_t)data_reg;
                    }
                    lastFillColor = 0x0000;
                    lastFillLen   = 0;
                    dma.setCallback(dma_callback);
                    return; // Success!
                }
                // Else some alloc/init error along the way...clean up...
                free(pixelBuf[0]);
                pixelBuf[0] = pixelBuf[1] = NULL;
            }
            // Don't currently have a descriptor delete function in
            // ZeroDMA lib, but if we did, it would be called here.
        }
        dma.free(); // Deallocate DMA channel
    }
#endif // end DMA init
}

/**************************************************************************/
/*!
    @brief   Read one byte from SPI interface (hardware or software)
    @returns One byte, MSB order
*/
/**************************************************************************/
uint8_t Adafruit_SPITFT::spiRead() {
    if(_sclk < 0){
        return HSPI_READ();
    }
    if(_miso < 0){
        return 0;
    }
    uint8_t r = 0;
    for (uint8_t i=0; i<8; i++) {
        SSPI_SCK_LOW();
        SSPI_SCK_HIGH();
        r <<= 1;
        if (SSPI_MISO_READ()){
            r |= 0x1;
        }
    }
    return r;
}

/**************************************************************************/
/*!
    @brief   Write one byte to SPI interface (hardware or software)
    @param  b  One byte to send, MSB order
*/
/**************************************************************************/
void Adafruit_SPITFT::spiWrite(uint8_t b) {
    if(_sclk < 0){
        HSPI_WRITE(b);
        return;
    }
    for(uint8_t bit = 0x80; bit; bit >>= 1){
        if((b) & bit){
            SSPI_MOSI_HIGH();
        } else {
            SSPI_MOSI_LOW();
        }
        SSPI_SCK_LOW();
        SSPI_SCK_HIGH();
    }
}


/*
 * Transaction API
 * */

/**************************************************************************/
/*!
    @brief   Begin an SPI transaction & set CS low.
*/
/**************************************************************************/
void inline Adafruit_SPITFT::startWrite(void){
    SPI_BEGIN_TRANSACTION();
    SPI_CS_LOW();
}

/**************************************************************************/
/*!
    @brief   Begin an SPI transaction & set CS high.
*/
/**************************************************************************/
void inline Adafruit_SPITFT::endWrite(void){
    SPI_CS_HIGH();
    SPI_END_TRANSACTION();
}

/**************************************************************************/
/*!
    @brief   Write a command byte (must have a transaction in progress)
    @param   cmd  The 8-bit command to send
*/
/**************************************************************************/
void Adafruit_SPITFT::writeCommand(uint8_t cmd){
    SPI_DC_LOW();
    spiWrite(cmd);
    SPI_DC_HIGH();
}

/**************************************************************************/
/*!
    @brief   Push a 2-byte color to the framebuffer RAM, will start transaction
    @param    color 16-bit 5-6-5 Color to draw
*/
/**************************************************************************/
void Adafruit_SPITFT::pushColor(uint16_t color) {
  startWrite();
  SPI_WRITE16(color);
  endWrite();
}

/**************************************************************************/
/*!
    @brief  Blit multiple 2-byte colors (must have a transaction in progress)
    @param  colors  Array of 16-bit 5-6-5 Colors to draw
    @param  len     How many pixels to draw - 2 bytes per pixel!
*/
/**************************************************************************/
void Adafruit_SPITFT::writePixels(uint16_t *colors, uint32_t len) {
#ifdef USE_SPI_DMA
    if(_sclk < 0) { // using hardware SPI?
        int     maxSpan     = maxFillLen / 2; // One scanline max
        uint8_t pixelBufIdx = 0;              // Active pixel buffer number
        while(len) {
            int count = (len < maxSpan) ? len : maxSpan;

            // Because TFT and SAMD endianisms are different, must swap bytes
            // from the 'colors' array passed into a DMA working buffer. This
            // can take place while the prior DMA transfer is in progress,
            // hence the need for two pixelBufs.
            for(int i=0; i<count; i++) {
                pixelBuf[pixelBufIdx][i] = __builtin_bswap16(*colors++);
            }
            // The transfers themselves are relatively small, so we don't
            // need a long descriptor list. We just alternate between the
            // first two, sharing pixelBufIdx for that purpose.
            descriptor[pixelBufIdx].SRCADDR.reg       =
              (uint32_t)pixelBuf[pixelBufIdx] + count * 2;
            descriptor[pixelBufIdx].BTCTRL.bit.SRCINC = 1;
            descriptor[pixelBufIdx].BTCNT.reg         = count * 2;
            descriptor[pixelBufIdx].DESCADDR.reg      = 0;

            while(dma_busy); // wait for prior line to complete

            // Move new descriptor into place...
            memcpy(dptr, &descriptor[pixelBufIdx], sizeof(DmacDescriptor));
            dma_busy = true;
            dma.startJob();                // Trigger SPI DMA transfer
            pixelBufIdx = 1 - pixelBufIdx; // Swap DMA pixel buffers

            len -= count;
        }
        lastFillColor = 0x0000; // pixelBuf has been sullied
        lastFillLen   = 0;
        while(dma_busy);        // Wait for last line to complete
#ifdef __SAMD51__
        _spi->setDataMode(SPI_MODE0); // See note in writeColor()
#endif
        return;
    }
#else
    SPI_WRITE_PIXELS((uint8_t*)colors , len * 2);
#endif
}

/**************************************************************************/
/*!
    @brief  Blit a 2-byte color many times (must have a transaction in progress)
    @param  color  The 16-bit 5-6-5 Color to draw
    @param  len    How many pixels to draw
*/
/**************************************************************************/
void Adafruit_SPITFT::writeColor(uint16_t color, uint32_t len) {

    if(!len) return; // Avoid 0-byte transfers

    uint8_t hi = color >> 8, lo = color;

    if(_sclk < 0) { // Using hardware SPI

#ifdef USE_SPI_DMA

        int i, d, numDescriptors;
        if(hi == lo) { // If high & low bytes are same...
            onePixelBuf = color;
            // Can do this with a relatively short descriptor list,
            // each transferring a max of 32,767 (not 32,768) pixels.
            // This won't run off the end of the allocated descriptor list,
            // since we're using much larger chunks per descriptor here.
            numDescriptors = (len + 32766) / 32767;
            for(d=0; d<numDescriptors; d++) {
                int count = (len < 32767) ? len : 32767;
                descriptor[d].SRCADDR.reg       = (uint32_t)&onePixelBuf;
                descriptor[d].BTCTRL.bit.SRCINC = 0;
                descriptor[d].BTCNT.reg         = count * 2;
                descriptor[d].DESCADDR.reg      = (uint32_t)&descriptor[d+1];
                len -= count;
            }
            descriptor[d-1].DESCADDR.reg        = 0;
        } else {
            // If high and low bytes are distinct, it's necessary to fill
            // a buffer with pixel data (swapping high and low bytes because
            // TFT and SAMD are different endianisms) and create a longer
            // descriptor list pointing repeatedly to this data. We can do
            // this slightly faster working 2 pixels (32 bits) at a time.
            uint32_t *pixelPtr  = (uint32_t *)pixelBuf[0],
                      twoPixels = __builtin_bswap16(color) * 0x00010001;
            // We can avoid some or all of the buffer-filling if the color
            // is the same as last time...
            if(color == lastFillColor) {
                // If length is longer than prior instance, fill only the
                // additional pixels in the buffer and update lastFillLen.
                if(len > lastFillLen) {
                    int fillStart = lastFillLen / 2,
                        fillEnd   = (((len < maxFillLen) ?
                                       len : maxFillLen) + 1) / 2;
                    for(i=fillStart; i<fillEnd; i++) pixelPtr[i] = twoPixels;
                    lastFillLen = fillEnd * 2;
                } // else do nothing, don't set pixels or change lastFillLen
            } else {
                int fillEnd = (((len < maxFillLen) ?
                                 len : maxFillLen) + 1) / 2;
                for(i=0; i<fillEnd; i++) pixelPtr[i] = twoPixels;
                lastFillLen   = fillEnd * 2;
                lastFillColor = color;
            }

            numDescriptors = (len + maxFillLen - 1) / maxFillLen;
            for(d=0; d<numDescriptors; d++) {
                int pixels = (len < maxFillLen) ? len : maxFillLen,
                    bytes  = pixels * 2;
                descriptor[d].SRCADDR.reg       = (uint32_t)pixelPtr + bytes;
                descriptor[d].BTCTRL.bit.SRCINC = 1;
                descriptor[d].BTCNT.reg         = bytes;
                descriptor[d].DESCADDR.reg      = (uint32_t)&descriptor[d+1];
                len -= pixels;
            }
            descriptor[d-1].DESCADDR.reg        = 0;
        }
        memcpy(dptr, &descriptor[0], sizeof(DmacDescriptor));

        dma_busy = true;
        dma.startJob();
        while(dma_busy); // Wait for completion
#ifdef __SAMD51__
        // SAMD51: SPI DMA seems to leave the SPI peripheral in a freaky
        // state on completion. Workaround is to explicitly set it back...
        _spi->setDataMode(SPI_MODE0);
#endif

        // Unfortunately blocking is necessary. An earlier version returned
        // immediately and checked dma_busy on startWrite() instead, but it
        // turns out to be MUCH slower on many graphics operations (as when
        // drawing lines, pixel-by-pixel), perhaps because it's a volatile
        // type and doesn't cache. Working on this.

#else // Non-DMA

  #ifdef SPI_HAS_WRITE_PIXELS
        #define TMPBUF_LONGWORDS (SPI_MAX_PIXELS_AT_ONCE + 1) / 2
        #define TMPBUF_PIXELS    (TMPBUF_LONGWORDS * 2)
        static uint32_t temp[TMPBUF_LONGWORDS];
        uint32_t        c32    = color * 0x00010001;
        uint16_t        bufLen = (len < TMPBUF_PIXELS) ? len : TMPBUF_PIXELS,
                        xferLen, fillLen;

        // Fill temp buffer 32 bits at a time
        fillLen = (bufLen + 1) / 2; // Round up to next 32-bit boundary
        for(uint32_t t=0; t<fillLen; t++) {
            temp[t] = c32;
        }

        // Issue pixels in blocks from temp buffer
        while(len) {                                 // While pixels remain
            xferLen = (bufLen < len) ? bufLen : len; // How many this pass?
            writePixels((uint16_t *)temp, xferLen);
            len -= xferLen;
        }
  #else
        while(len--) {
            HSPI_WRITE(hi);
            HSPI_WRITE(lo);
        }
  #endif

#endif // end non-DMA

    } else { // Bitbang SPI
        while(len--) {
            spiWrite(hi);
            spiWrite(lo);
        }
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
    @brief  Draw a pixel - sets up transaction
    @param  x  x coordinate
    @param  y  y coordinate
    @param  color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void Adafruit_SPITFT::drawPixel(int16_t x, int16_t y, uint16_t color){
    // Clip first...
    if((x >= 0) && (x < _width) && (y >= 0) && (y < _height)) {
        // THEN set up transaction (if needed) and draw...
        startWrite();
        setAddrWindow(x, y, 1, 1);
        writePixel(color);
        endWrite();
    }
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

#endif // !__AVR_ATtiny85__

