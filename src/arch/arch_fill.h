/*!
 * @file arch_fill.h
 *
 * Dispatcher for the hardware SPI solid fill in Adafruit_SPITFT.
 */

#ifndef _ADAFRUIT_SPITFT_ARCH_FILL_H_
#define _ADAFRUIT_SPITFT_ARCH_FILL_H_

// Every family defines Adafruit_SPITFT::writeColorHardSPI(). This is only
// included by Adafruit_SPITFT.cpp because the definitions need the class and
// that file's SPI macros, which arch.h comes before. Each family selects
// itself.
#include "esp8266/arch_fill.h"
#include "generic/arch_fill.h"
#include "rp2040/arch_fill.h"

#endif // end _ADAFRUIT_SPITFT_ARCH_FILL_H_
