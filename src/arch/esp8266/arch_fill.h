/*!
 * @file arch_fill.h
 *
 * ESP8266 hardware SPI solid fill for Adafruit_SPITFT, see ../arch_fill.h.
 */

#ifndef _ADAFRUIT_SPITFT_ARCH_ESP8266_FILL_H_
#define _ADAFRUIT_SPITFT_ARCH_ESP8266_FILL_H_

#if defined(ESP8266)

inline void Adafruit_SPITFT::writeColorHardSPI(uint16_t color, uint32_t len) {
  // Solid-color fill: use the SPI hardware pattern engine instead of two
  // per-pixel SPI.write() calls. writePattern() repeats the 2-byte color
  // from the FIFO at line rate (~9 MB/s vs ~0.5 MB/s for the naive loop).
  uint8_t colorPattern[2] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
  do {
    uint32_t pixelsThisPass = len;
    if (pixelsThisPass > 50000)
      pixelsThisPass = 50000;
    len -= pixelsThisPass;
    hwspi._spi->writePattern(colorPattern, 2, pixelsThisPass);
    optimistic_yield(10000); // Yield if 10 ms since last yield
  } while (len);
}

#endif // end ESP8266
#endif // end _ADAFRUIT_SPITFT_ARCH_ESP8266_FILL_H_
