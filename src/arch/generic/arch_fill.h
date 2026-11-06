/*!
 * @file arch_fill.h
 *
 * Fallback hardware SPI solid fill for Adafruit_SPITFT, see ../arch_fill.h.
 */

#ifndef _ADAFRUIT_SPITFT_ARCH_GENERIC_FILL_H_
#define _ADAFRUIT_SPITFT_ARCH_GENERIC_FILL_H_

#if !defined(ESP8266) && !defined(ARDUINO_ARCH_RP2040)

inline void Adafruit_SPITFT::writeColorHardSPI(uint16_t color, uint32_t len) {
  uint8_t hi = color >> 8, lo = color;

  while (len--) {
#if defined(__AVR__)
    AVR_WRITESPI(hi);
    AVR_WRITESPI(lo);
#elif defined(ESP32)
    hwspi._spi->write(hi);
    hwspi._spi->write(lo);
#else
    hwspi._spi->transfer(hi);
    hwspi._spi->transfer(lo);
#endif
  }
}

#endif // end fallback
#endif // end _ADAFRUIT_SPITFT_ARCH_GENERIC_FILL_H_
