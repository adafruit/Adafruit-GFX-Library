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

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SPIDevice.h>

#define MONOOLED_BLACK               0    ///< Draw 'off' pixels
#define MONOOLED_WHITE               1    ///< Draw 'on' pixels
#define MONOOLED_INVERSE             2    ///< Invert pixels

/// These seem to be common commands for OLEDs
#define MONOOLED_SETCONTRAST         0x81 ///< See datasheet
#define MONOOLED_NORMALDISPLAY       0xA6 ///< See datasheet
#define MONOOLED_INVERTDISPLAY       0xA7 ///< See datasheet
#define MONOOLED_DISPLAYOFF          0xAE ///< See datasheet
#define MONOOLED_DISPLAYON           0xAF ///< See datasheet

/*!
    @brief  Class that stores state and functions for interacting with
            generic monochrome OLED displays.
*/
class Adafruit_MonoOLED : public Adafruit_GFX {
 public:
  Adafruit_MonoOLED(uint16_t w, uint16_t h, TwoWire *twi=&Wire, int8_t rst_pin=-1,
		    uint32_t preclk=400000, uint32_t postclk=100000);
  Adafruit_MonoOLED(uint16_t w, uint16_t h, int8_t mosi_pin, int8_t sclk_pin,
    int8_t dc_pin, int8_t rst_pin, int8_t cs_pin);
  Adafruit_MonoOLED(uint16_t w, uint16_t h, SPIClass *spi,
    int8_t dc_pin, int8_t rst_pin, int8_t cs_pin, uint32_t bitrate=8000000UL);

  ~Adafruit_MonoOLED(void);
  virtual void         display(void)=0;
  void         clearDisplay(void);
  void         invertDisplay(boolean i);
  void         setContrast(uint8_t contrastlevel);
  uint8_t      getContrast(void);
  void         drawPixel(int16_t x, int16_t y, uint16_t color);
  boolean      getPixel(int16_t x, int16_t y);
  uint8_t     *getBuffer(void);

  void         oled_command(uint8_t c);
  bool         oled_commandList(const uint8_t *c, uint8_t n);

 protected:
  bool         _init(uint8_t i2caddr=0x3C, boolean reset=true);

  Adafruit_SPIDevice    *spi_dev = NULL;
  Adafruit_I2CDevice    *i2c_dev = NULL;
  TwoWire     *_theWire = NULL;
  int          dcPin, csPin, rstPin;
  uint8_t     *buffer = NULL;
  int8_t       i2caddr;

  uint8_t      contrast;    // normal contrast setting for this device
};

#endif // _Adafruit_MonoOLED_H_
