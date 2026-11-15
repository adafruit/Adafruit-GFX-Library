/*!
 * @file samd.h
 *
 * SAMD (Adafruit M0, M4) arm of the Adafruit_SPITFT architecture contract, see
 * arch.h.
 * Selects itself and expands to nothing on any other chip.
 */

#ifndef _ADAFRUIT_SPITFT_ARCH_SAMD_H_
#define _ADAFRUIT_SPITFT_ARCH_SAMD_H_

#if !defined(__AVR__) && !defined(ARDUINO_STM32_FEATHER) &&                    \
    defined(__arm__) && defined(ARDUINO_ARCH_SAMD)

typedef uint32_t ADAGFX_PORT_t; ///< PORT values are 32-bit
#define USE_FAST_PINIO          ///< Use direct PORT register access
#define HAS_PORT_SET_CLR        ///< PORTs have set & clear registers

#endif // end ARDUINO_ARCH_SAMD
#endif // end _ADAFRUIT_SPITFT_ARCH_SAMD_H_
