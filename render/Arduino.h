#ifndef __ARDUINO_H__
#define __ARDUINO_H__

// Minimal Arduino.h implementation
// to support compilation of Adafruit_GFX

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> // for strlen

#define PROGMEM
typedef const char *__FlashStringHelper;

// Empty String class
// Doesn't really work, but isn't used either
// Only defined to make Adafruit_GFX compile
class String {
public:
  inline unsigned int length(void) const { return 0; }

  const char *c_str() const { return ""; }
};

#endif
