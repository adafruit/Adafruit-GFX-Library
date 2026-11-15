/*!
 * @file wiced.h
 *
 * WICED Feather arm of the Adafruit_SPITFT architecture contract, see arch.h.
 * Selects itself and expands to nothing on any other chip.
 */

#ifndef _ADAFRUIT_SPITFT_ARCH_WICED_H_
#define _ADAFRUIT_SPITFT_ARCH_WICED_H_

#if !defined(__AVR__) && defined(ARDUINO_STM32_FEATHER)

typedef class HardwareSPI SPIClass; ///< SPI is a bit odd on WICED
typedef uint32_t ADAGFX_PORT_t;     ///< PORT values are 32-bit

#endif // end ARDUINO_STM32_FEATHER
#endif // end _ADAFRUIT_SPITFT_ARCH_WICED_H_
