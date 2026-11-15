/*!
 * @file arch.h
 *
 * Architecture dispatcher for Adafruit_SPITFT.
 */

#ifndef _ADAFRUIT_SPITFT_ARCH_H_
#define _ADAFRUIT_SPITFT_ARCH_H_

// Every family provides:
//   ADAGFX_PORT_t      Integer type of the PORT registers.
//   USE_FAST_PINIO     Defined if direct PORT register access is available.
//   HAS_PORT_SET_CLR   Defined if PORTs have set & clear registers.
//
// These are headers because Arduino compiles every source file under src/
// regardless of the selected board. To add a family, write
// arch/<name>/<name>.h, guard it, and include it here. Exclude the new family
// from the generic guard.

// Every family is included unconditionally; each selects itself.
#include "avr/avr.h"
#include "generic/generic.h"
#include "samd/samd.h"
#include "teensy/teensy.h"
#include "wiced/wiced.h"

#endif // end _ADAFRUIT_SPITFT_ARCH_H_
