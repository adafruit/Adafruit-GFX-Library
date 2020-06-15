/*!
 * @file Adafruit_MonoOLED.h
 *
 * This is part of for Adafruit's GFX library, supplying generic support
 * for monochrome OLED displays: http://www.adafruit.com/category/63_98
 *
 * These displays use I2C or SPI to communicate. I2C requires 2 pins
 * (SCL+SDA) and optionally a RESET pin. SPI requires 4 pins (MOSI, SCK,
 * select, data/command) and optionally a reset pin. Hardware SPI or
 * 'bitbang' software SPI are both supported.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries, with
 * contributions from the open source community.
 *
 * BSD license, all text above, and the splash screen header file,
 * must be included in any redistribution.
 *
 */

#ifndef _Adafruit_MONOOLED_H_
#define _Adafruit_MONOOLED_H_

#if !defined(__AVR_ATtiny85__) // Not for ATtiny, at all

#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SPIDevice.h>
#include <SPI.h>
#include <Wire.h>

#define MONOOLED_BLACK 0   ///< Draw 'off' pixels
#define MONOOLED_WHITE 1   ///< Draw 'on' pixels
#define MONOOLED_INVERSE 2 ///< Invert pixels

/// These seem to be common commands for OLEDs
#define MONOOLED_SETCONTRAST 0x81   ///< See datasheet
#define MONOOLED_NORMALDISPLAY 0xA6 ///< See datasheet
#define MONOOLED_INVERTDISPLAY 0xA7 ///< See datasheet
#define MONOOLED_DISPLAYOFF 0xAE    ///< See datasheet
#define MONOOLED_DISPLAYON 0xAF     ///< See datasheet

/*!
    @brief  Class that stores state and functions for interacting with
            generic monochrome OLED displays.
*/
class Adafruit_MonoOLED : public Adafruit_GFX {
public:
  Adafruit_MonoOLED(uint16_t w, uint16_t h, TwoWire *twi = &Wire,
                    int8_t rst_pin = -1, uint32_t preclk = 400000,
                    uint32_t postclk = 100000);
  Adafruit_MonoOLED(uint16_t w, uint16_t h, int8_t mosi_pin, int8_t sclk_pin,
                    int8_t dc_pin, int8_t rst_pin, int8_t cs_pin);
  Adafruit_MonoOLED(uint16_t w, uint16_t h, SPIClass *spi, int8_t dc_pin,
                    int8_t rst_pin, int8_t cs_pin,
                    uint32_t bitrate = 8000000UL);

  ~Adafruit_MonoOLED(void);

  /**
   @brief The function that sub-classes define that writes out the buffer to
   the display over I2C or SPI
   **/
  virtual void display(void) = 0;
  void clearDisplay(void);
  void invertDisplay(bool i);
  void setContrast(uint8_t contrastlevel);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  bool getPixel(int16_t x, int16_t y);
  uint8_t *getBuffer(void);

  void oled_command(uint8_t c);
  bool oled_commandList(const uint8_t *c, uint8_t n);

protected:
  bool _init(uint8_t i2caddr = 0x3C, bool reset = true);

  Adafruit_SPIDevice *spi_dev = NULL; ///< The SPI interface BusIO device
  Adafruit_I2CDevice *i2c_dev = NULL; ///< The I2C interface BusIO device
  int32_t i2c_preclk = 400000,        ///< Configurable 'high speed' I2C rate
      i2c_postclk = 100000;           ///< Configurable 'low speed' I2C rate
  uint8_t *buffer = NULL; ///< Internal 1:1 framebuffer of display mem

  int16_t window_x1, ///< Dirty tracking window minimum x
      window_y1,     ///< Dirty tracking window minimum y
      window_x2,     ///< Dirty tracking window maximum x
      window_y2;     ///< Dirty tracking window maximum y

  int dcPin,  ///< The Arduino pin connected to D/C (for SPI)
      csPin,  ///< The Arduino pin connected to CS (for SPI)
      rstPin; ///< The Arduino pin connected to reset (-1 if unused)

private:
  TwoWire *_theWire = NULL; ///< The underlying hardware I2C
};

#endif // end __AVR_ATtiny85__
#endif // _Adafruit_MonoOLED_H_
