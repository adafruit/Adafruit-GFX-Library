/*!
 * @file arch_fill.h
 *
 * RP2040 and RP2350 hardware SPI solid fill for Adafruit_SPITFT, see
 * ../arch_fill.h.
 */

#ifndef _ADAFRUIT_SPITFT_ARCH_RP2040_FILL_H_
#define _ADAFRUIT_SPITFT_ARCH_RP2040_FILL_H_

#if !defined(ESP8266) && defined(ARDUINO_ARCH_RP2040)

inline void Adafruit_SPITFT::writeColorHardSPI(uint16_t color, uint32_t len) {
  spi_inst_t *pi_spi = hwspi._spi == &SPI ? __SPI0_DEVICE : __SPI1_DEVICE;
  color = __builtin_bswap16(color);

  while (len--)
    spi_write_blocking(pi_spi, (uint8_t *)&color, 2);
}

#endif // end ARDUINO_ARCH_RP2040
#endif // end _ADAFRUIT_SPITFT_ARCH_RP2040_FILL_H_
