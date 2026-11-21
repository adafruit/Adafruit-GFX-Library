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
  spi_hw_t *hw = spi_get_hw(pi_spi);

  // 16-bit frames send the high byte first, so no byte swap is needed.
  // Keep the FIFO full instead of waiting for each pixel to finish.
  hw_write_masked(&hw->cr0, 15 << SPI_SSPCR0_DSS_LSB, SPI_SSPCR0_DSS_BITS);
  while (len--) {
    while (!spi_is_writable(pi_spi))
      ;
    hw->dr = color;
  }
  // Wait for the last pixel, then drop the RX data like spi_write16_blocking
  while (spi_is_readable(pi_spi))
    (void)hw->dr;
  while (hw->sr & SPI_SSPSR_BSY_BITS)
    ;
  while (spi_is_readable(pi_spi))
    (void)hw->dr;
  hw->icr = SPI_SSPICR_RORIC_BITS;
  hw_write_masked(&hw->cr0, 7 << SPI_SSPCR0_DSS_LSB, SPI_SSPCR0_DSS_BITS);
}

#endif // end ARDUINO_ARCH_RP2040
#endif // end _ADAFRUIT_SPITFT_ARCH_RP2040_FILL_H_
