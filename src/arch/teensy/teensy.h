/*!
 * @file teensy.h
 *
 * PJRC Teensy 3.x and 4.x arm of the Adafruit_SPITFT architecture contract, see
 * arch.h.
 * Selects itself and expands to nothing on any other chip.
 */

#ifndef _ADAFRUIT_SPITFT_ARCH_TEENSY_H_
#define _ADAFRUIT_SPITFT_ARCH_TEENSY_H_

#if !defined(__AVR__) && !defined(ARDUINO_STM32_FEATHER) &&                    \
    defined(__arm__) && !defined(ARDUINO_ARCH_SAMD) && defined(CORE_TEENSY)

#if defined(__IMXRT1052__) || defined(__IMXRT1062__) // Teensy 4.x
typedef uint32_t ADAGFX_PORT_t;                      ///< PORT values are 32-bit
#else                                                // Teensy 3.x
typedef uint8_t ADAGFX_PORT_t; ///< PORT values are 8-bit
#endif
#define USE_FAST_PINIO   ///< Use direct PORT register access
#define HAS_PORT_SET_CLR ///< PORTs have set & clear registers

#endif // end CORE_TEENSY
#endif // end _ADAFRUIT_SPITFT_ARCH_TEENSY_H_
