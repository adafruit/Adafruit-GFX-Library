/*!
 * @file generic.h
 *
 * Fallback arm of the Adafruit_SPITFT architecture contract, see arch.h.
 * Selects itself and expands to nothing on any other chip.
 */

#ifndef _ADAFRUIT_SPITFT_ARCH_GENERIC_H_
#define _ADAFRUIT_SPITFT_ARCH_GENERIC_H_

#if !defined(__AVR__) && !defined(ARDUINO_STM32_FEATHER) &&                    \
    !(defined(__arm__) &&                                                      \
      (defined(ARDUINO_ARCH_SAMD) || defined(CORE_TEENSY)))

// Other ARM (Arduino Due?): USE_FAST_PINIO not available here (yet)...Due has
// a totally different GPIO register set and will require some changes
// elsewhere (e.g. in constructors especially).
// Probably ESP8266 or ESP32: USE_FAST_PINIO is not available here (yet) but
// don't worry about it too much...the digitalWrite() implementation on these
// platforms is reasonably efficient and already RAM-resident, only gotcha then
// is no parallel connection support for now.
typedef uint32_t ADAGFX_PORT_t; ///< PORT values are 32-bit

#endif // end fallback
#endif // end _ADAFRUIT_SPITFT_ARCH_GENERIC_H_
