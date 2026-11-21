/*!
 * @file stm32.h
 *
 * STM32 arm of the Adafruit_SPITFT architecture contract, see arch.h.
 * Selects itself and expands to nothing on any other chip.
 */

#ifndef _ADAFRUIT_SPITFT_ARCH_STM32_H_
#define _ADAFRUIT_SPITFT_ARCH_STM32_H_

#if !defined(__AVR__) && !defined(ARDUINO_STM32_FEATHER) &&                    \
    defined(__arm__) && !defined(ARDUINO_ARCH_SAMD) &&                         \
    !defined(CORE_TEENSY) && defined(STM32_CORE_VERSION)

#if defined(STM32F2xx) || defined(STM32F4xx) || defined(STM32F7xx)
typedef uint16_t ADAGFX_PORT_t; ///< 16-bit halves of BSRR, see SPITFT_SET_REG
#else
typedef uint32_t ADAGFX_PORT_t; ///< PORT values are 32-bit
#endif
#define USE_FAST_PINIO   ///< Use direct PORT register access
#define HAS_PORT_SET_CLR ///< PORTs have set & clear registers

#endif // end STM32_CORE_VERSION
#endif // end _ADAFRUIT_SPITFT_ARCH_STM32_H_
