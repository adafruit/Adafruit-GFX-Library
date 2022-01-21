#ifndef __PRINT_H__
#define __PRINT_H__

#include <cstdio>

class Print {
public:
  virtual size_t write(uint8_t) = 0;

  virtual size_t write(const uint8_t *buffer, size_t size) {
    size_t n = 0;
    while (size--) {
      if (write(*buffer++))
        n++;
      else
        break;
    }
    return n;
  }

  size_t print(const char *str) { return write(str, strlen(str)); }

  size_t write(const char *buffer, size_t size) {
    return write((const uint8_t *)buffer, size);
  }
};

#endif
