/*!
 * @file arch_fill.h
 *
 * ESP8266 hardware SPI solid fill for Adafruit_SPITFT, see ../arch_fill.h.
 */

#ifndef _ADAFRUIT_SPITFT_ARCH_ESP8266_FILL_H_
#define _ADAFRUIT_SPITFT_ARCH_ESP8266_FILL_H_

#if defined(ESP8266)

inline void Adafruit_SPITFT::writeColorHardSPI(uint16_t color, uint32_t len) {
  uint8_t hi = color >> 8, lo = color;

  do {
    uint32_t pixelsThisPass = len;
    if (pixelsThisPass > 50000)
      pixelsThisPass = 50000;
    len -= pixelsThisPass;
    optimistic_yield(10000); // Yield if 10 ms since last yield
    while (pixelsThisPass--) {
      hwspi._spi->write(hi);
      hwspi._spi->write(lo);
    }
  } while (len);
}

#endif // end ESP8266
#endif // end _ADAFRUIT_SPITFT_ARCH_ESP8266_FILL_H_
