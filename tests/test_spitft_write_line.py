#!/usr/bin/env python3
"""Exercise Adafruit_SPITFT::writeLine without an Arduino toolchain.

The test compiles the actual method body from Adafruit_SPITFT.cpp against a
small display mock, then compares its clipped pixels with the original
pixel-by-pixel Bresenham walk.
"""

from pathlib import Path
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src" / "Adafruit_SPITFT.cpp"
MARKER = "void Adafruit_SPITFT::writeLine("


def method_definition(source: str) -> str:
    start = source.index(MARKER)
    opening = source.index("{", start)
    depth = 0
    for index in range(opening, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                return source[start : index + 1]
    raise ValueError("writeLine definition has unbalanced braces")


HARNESS = r"""
#include <algorithm>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <vector>

struct LineCall {
  int16_t x, y, length;
};

class Adafruit_SPITFT {
public:
  int16_t _width, _height;
  std::vector<uint8_t> pixels;
  std::vector<LineCall> horizontal, vertical;
  int pixelCalls = 0;

  Adafruit_SPITFT(int16_t width, int16_t height)
      : _width(width), _height(height), pixels(width * height, 0) {}

  void writePixel(int16_t x, int16_t y, uint16_t color);
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                 uint16_t color);

  void mark(int16_t x, int16_t y) {
    if (x >= 0 && x < _width && y >= 0 && y < _height)
      pixels[y * _width + x] = 1;
  }
};

void Adafruit_SPITFT::writePixel(int16_t x, int16_t y, uint16_t) {
  ++pixelCalls;
  mark(x, y);
}

void Adafruit_SPITFT::writeFastHLine(int16_t x, int16_t y, int16_t w,
                                     uint16_t) {
  horizontal.push_back({x, y, w});
  if (y >= 0 && y < _height && w) {
    if (w < 0) {
      x += w + 1;
      w = -w;
    }
    if (x < _width) {
      int16_t x2 = x + w - 1;
      if (x2 >= 0) {
        if (x < 0) {
          x = 0;
          w = x2 + 1;
        }
        if (x2 >= _width) w = _width - x;
        for (int16_t i = 0; i < w; ++i) mark(x + i, y);
      }
    }
  }
}

void Adafruit_SPITFT::writeFastVLine(int16_t x, int16_t y, int16_t h,
                                     uint16_t) {
  vertical.push_back({x, y, h});
  if (x >= 0 && x < _width && h) {
    if (h < 0) {
      y += h + 1;
      h = -h;
    }
    if (y < _height) {
      int16_t y2 = y + h - 1;
      if (y2 >= 0) {
        if (y < 0) {
          y = 0;
          h = y2 + 1;
        }
        if (y2 >= _height) h = _height - y;
        for (int16_t i = 0; i < h; ++i) mark(x, y + i);
      }
    }
  }
}

__METHOD_DEFINITION__

static void referenceLine(std::vector<uint8_t> &pixels, int16_t width,
                          int16_t height, int16_t x0, int16_t y0,
                          int16_t x1, int16_t y1) {
  bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
  if (steep) {
    std::swap(x0, y0);
    std::swap(x1, y1);
  }
  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }
  int16_t dx = x1 - x0;
  int16_t dy = std::abs(y1 - y0);
  int16_t err = dx / 2;
  int16_t ystep = (y0 < y1) ? 1 : -1;
  for (; x0 <= x1; ++x0) {
    int16_t x = steep ? y0 : x0;
    int16_t y = steep ? x0 : y0;
    if (x >= 0 && x < width && y >= 0 && y < height)
      pixels[y * width + x] = 1;
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

static bool checkDirection(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                           bool expectHorizontal, bool expectVertical,
                           int expectedPixelCalls = -1,
                           bool expectCallReduction = false) {
  Adafruit_SPITFT display(32, 24);
  display.writeLine(x0, y0, x1, y1, 0x1234);
  if ((!expectHorizontal && !display.horizontal.empty()) ||
      (!expectVertical && !display.vertical.empty()) ||
      (expectHorizontal && display.horizontal.empty()) ||
      (expectVertical && display.vertical.empty()) ||
      (expectedPixelCalls >= 0 && display.pixelCalls != expectedPixelCalls)) {
    std::cerr << "unexpected writer direction/count for (" << x0 << ", "
              << y0 << ") -> (" << x1 << ", " << y1 << ")\n";
    return false;
  }
  for (const LineCall &call : display.horizontal)
    if (call.length <= 1) return false;
  for (const LineCall &call : display.vertical)
    if (call.length <= 1) return false;
  int writerCalls = display.pixelCalls + display.horizontal.size() +
                    display.vertical.size();
  int pixelCount = std::count(display.pixels.begin(), display.pixels.end(), 1);
  if (expectCallReduction && writerCalls >= pixelCount) {
    std::cerr << "writer calls were not reduced for (" << x0 << ", " << y0
              << ") -> (" << x1 << ", " << y1 << ")\n";
    return false;
  }
  if (expectCallReduction) {
    std::cout << pixelCount << " pixels, " << writerCalls
              << " API writer calls for (" << x0 << ", " << y0 << ") -> ("
              << x1 << ", " << y1 << ")\n";
  }
  return true;
}

int main() {
  const int16_t width = 16, height = 12;
  // Avoid the reference loop here: its int16_t increment overflows at x1.
  Adafruit_SPITFT maxEndpoint(width, height);
  maxEndpoint.writeLine(0, 0, 32767, 1, 0x1234);
  std::vector<uint8_t> expectedMaxEndpoint(width * height, 0);
  std::fill(expectedMaxEndpoint.begin(), expectedMaxEndpoint.begin() + width,
            1);
  if (maxEndpoint.pixels != expectedMaxEndpoint) {
    std::cerr << "unexpected pixels for endpoint at INT16_MAX\n";
    return 1;
  }

  // Exhaustively compare all endpoint pairs in and just beyond every edge.
  for (int16_t y0 = -2; y0 <= height + 1; ++y0) {
    for (int16_t x0 = -2; x0 <= width + 1; ++x0) {
      for (int16_t y1 = -2; y1 <= height + 1; ++y1) {
        for (int16_t x1 = -2; x1 <= width + 1; ++x1) {
          Adafruit_SPITFT display(width, height);
          display.writeLine(x0, y0, x1, y1, 0x1234);
          std::vector<uint8_t> expected(width * height, 0);
          referenceLine(expected, width, height, x0, y0, x1, y1);
          if (display.pixels != expected) {
            std::cerr << "pixel mismatch for (" << x0 << ", " << y0
                      << ") -> (" << x1 << ", " << y1 << ")\n";
            return 1;
          }
        }
      }
    }
  }

  // Shallow lines use H runs; steep lines use V runs, for both slope signs.
  if (!checkDirection(1, 2, 13, 6, true, false, -1, true) ||
      !checkDirection(1, 10, 13, 6, true, false) ||
      !checkDirection(2, 1, 6, 11, false, true, -1, true) ||
      !checkDirection(12, 1, 8, 11, false, true) ||
      !checkDirection(1, 1, 8, 8, false, false, 8) ||
      !checkDirection(8, 8, 1, 1, false, false, 8) ||
      !checkDirection(3, 4, 3, 4, false, false, 1))
    return 1;

  std::cout << "SPITFT writeLine: exhaustive pixel equivalence and direction checks passed\n";
}
"""


def main() -> None:
    definition = method_definition(SOURCE.read_text())
    program = HARNESS.replace("__METHOD_DEFINITION__", definition)
    with tempfile.TemporaryDirectory(prefix="gfx-write-line-") as temp_dir:
        cpp_file = Path(temp_dir) / "test_write_line.cpp"
        binary = Path(temp_dir) / "test_write_line"
        cpp_file.write_text(program)
        subprocess.run(
            [
                "g++",
                "-std=c++11",
                "-Wall",
                "-Wextra",
                "-Werror",
                str(cpp_file),
                "-o",
                str(binary),
            ],
            check=True,
        )
        subprocess.run([str(binary)], check=True, timeout=10)


if __name__ == "__main__":
    main()
