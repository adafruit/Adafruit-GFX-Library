/*!
 * @file avr.h
 *
 * AVR arm of the Adafruit_SPITFT architecture contract, see arch.h.
 * Selects itself and expands to nothing on any other chip.
 */

#ifndef _ADAFRUIT_SPITFT_ARCH_AVR_H_
#define _ADAFRUIT_SPITFT_ARCH_AVR_H_

#if defined(__AVR__)

typedef uint8_t ADAGFX_PORT_t; ///< PORT values are 8-bit
#define USE_FAST_PINIO         ///< Use direct PORT register access

#endif // end __AVR__
#endif // end _ADAFRUIT_SPITFT_ARCH_AVR_H_
