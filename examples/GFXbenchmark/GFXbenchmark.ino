// GFX benchmark: times common drawing calls so speed changes can be measured.
// Results print to Serial as "test,microseconds", one line per test.
// Defaults to the Adafruit ILI9341 shield (CS 10, DC 9).
// Build with -DBENCH_HX8357 for the 3.5" HX8357 FeatherWing (CS 9, DC 10),
// or -DBENCH_SSD1351 for a 128x128 SSD1351 OLED (CS 9, DC 10, RST 11).

#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>
#include <SPI.h>

#if defined(BENCH_HX8357)
#include <Adafruit_HX8357.h>
Adafruit_HX8357 tft(9, 10, -1);
#elif defined(BENCH_SSD1351)
#include <Adafruit_SSD1351.h>
Adafruit_SSD1351 tft(128, 128, &SPI, 9, 10, 11);
#else
#include <Adafruit_ILI9341.h>
Adafruit_ILI9341 tft(10, 9);
#endif

#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define YELLOW 0xFFE0

static uint8_t mono[32 * 32 / 8]; // 32x32 1-bit bitmap, filled in setup()
static uint16_t rgb[16 * 16];     // 16x16 color bitmap, filled in setup()
static uint32_t seed;

// Small fixed-seed generator so every board draws the same lines.
static uint16_t next(uint16_t max) {
  seed = seed * 1103515245UL + 12345UL;
  return (seed >> 16) % max;
}

static void report(const __FlashStringHelper *name, uint32_t us) {
  Serial.print(name);
  Serial.print(',');
  Serial.println(us);
}

uint32_t testFillScreen() {
  uint32_t t = micros();
  tft.fillScreen(RED);
  tft.fillScreen(GREEN);
  tft.fillScreen(BLUE);
  tft.fillScreen(BLACK);
  return (micros() - t) / 4;
}

uint32_t testFillRects() {
  int16_t w = tft.width(), h = tft.height();
  uint32_t t = micros();
  for (int16_t y = 0; y < h; y += 16)
    for (int16_t x = 0; x < w; x += 16)
      tft.fillRect(x, y, 15, 15, ((x ^ y) & 16) ? YELLOW : BLUE);
  return micros() - t;
}

uint32_t testLines() {
  int16_t w = tft.width(), h = tft.height();
  tft.fillScreen(BLACK);
  seed = 1;
  uint32_t t = micros();
  for (int i = 0; i < 1000; i++)
    tft.drawLine(next(w), next(h), next(w), next(h), WHITE);
  return micros() - t;
}

uint32_t testFastLines() {
  int16_t w = tft.width(), h = tft.height();
  tft.fillScreen(BLACK);
  uint32_t t = micros();
  for (int16_t y = 0; y < h; y += 4)
    tft.drawFastHLine(0, y, w, GREEN);
  for (int16_t x = 0; x < w; x += 4)
    tft.drawFastVLine(x, 0, h, RED);
  return micros() - t;
}

uint32_t testText(const GFXfont *font, bool bg) {
  tft.fillScreen(BLACK);
  tft.setFont(font);
  tft.setTextSize(1);
  if (bg)
    tft.setTextColor(WHITE, BLUE);
  else
    tft.setTextColor(WHITE);
  tft.setCursor(0, font ? 14 : 0);
  uint32_t t = micros();
  for (int i = 0; i < 8; i++)
    tft.println(F("The quick brown fox 0123"));
  t = micros() - t;
  tft.setFont();
  return t;
}

uint32_t testBitmap(bool bg) {
  int16_t w = tft.width(), h = tft.height();
  tft.fillScreen(BLACK);
  uint32_t t = micros();
  for (int16_t y = 0; y + 32 <= h; y += 32)
    for (int16_t x = 0; x + 32 <= w; x += 32) {
      if (bg)
        tft.drawBitmap(x, y, mono, 32, 32, YELLOW, BLUE);
      else
        tft.drawBitmap(x, y, mono, 32, 32, YELLOW);
    }
  return micros() - t;
}

uint32_t testRGBBitmap() {
  int16_t w = tft.width(), h = tft.height();
  tft.fillScreen(BLACK);
  uint32_t t = micros();
  for (int16_t y = 0; y + 16 <= h; y += 16)
    for (int16_t x = 0; x + 16 <= w; x += 16)
      tft.drawRGBBitmap(x, y, rgb, 16, 16);
  return micros() - t;
}

uint32_t testCircles() {
  int16_t w = tft.width(), h = tft.height();
  tft.fillScreen(BLACK);
  uint32_t t = micros();
  for (int16_t y = 10; y < h; y += 20)
    for (int16_t x = 10; x < w; x += 20)
      tft.fillCircle(x, y, 9, GREEN);
  return micros() - t;
}

void setup() {
  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && millis() - t0 < 3000)
    delay(10);

  for (uint16_t i = 0; i < sizeof(mono); i++) // diagonal stripes
    mono[i] = 0x0F << ((i / 4) & 3);
  for (uint16_t i = 0; i < 16 * 16; i++) // color gradient
    rgb[i] = ((i & 15) << 12) | ((i >> 4) << 7) | 0x0010;

  tft.begin();
  Serial.print(F("GFX benchmark "));
  Serial.print(tft.width());
  Serial.print('x');
  Serial.println(tft.height());
  Serial.println(F("test,us"));

  report(F("fillScreen"), testFillScreen());
  report(F("fillRect grid"), testFillRects());
  report(F("1000 lines"), testLines());
  report(F("fast H/V lines"), testFastLines());
  report(F("text builtin"), testText(NULL, false));
  report(F("text builtin bg"), testText(NULL, true));
  report(F("text FreeSans9pt"), testText(&FreeSans9pt7b, false));
  report(F("bitmap 1-bit"), testBitmap(false));
  report(F("bitmap 1-bit bg"), testBitmap(true));
  report(F("RGB bitmap"), testRGBBitmap());
  report(F("fillCircle grid"), testCircles());
  Serial.println(F("done"));
}

void loop() {}
