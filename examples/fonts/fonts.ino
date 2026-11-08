// Fonts example: the built-in font, a few FreeFonts, and changing text that
// uses a custom font. Written for the Adafruit 2.8" ILI9341 shield
// (CS 10, DC 9). For another display, change the include and the tft line;
// the font calls are the same on every GFX display.

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSerifBold12pt7b.h>

Adafruit_ILI9341 tft(10, 9); // CS, DC

void setup() {
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  // Built-in 6x8 font: the cursor is the top-left corner of the text.
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println("Built-in font, size 1");
  tft.setTextSize(2);
  tft.println("Built-in font, size 2");
  tft.setTextSize(1);

  // Custom fonts: the cursor is the baseline, the bottom of most letters.
  // Set it below the top of the text, or the text is cut off.
  tft.setFont(&FreeSans9pt7b);
  tft.setCursor(0, 60);
  tft.setTextColor(ILI9341_YELLOW);
  tft.println("FreeSans 9pt");
  tft.setFont(&FreeSerifBold12pt7b);
  tft.setTextColor(ILI9341_CYAN);
  tft.println("FreeSerif Bold 12pt");
  tft.setFont(&FreeMonoBold9pt7b);
  tft.setTextColor(ILI9341_GREEN);
  tft.println("FreeMono Bold 9pt");

  // setFont() with no argument goes back to the built-in font.
  tft.setFont();
  tft.setTextColor(ILI9341_WHITE);
  tft.println("Back to the built-in font");
}

void loop() {
  // Custom fonts draw no background color, so new text would pile on top of
  // the old. Measure the old text with getTextBounds() and clear that box
  // with fillRect() before printing the new text.
  static char text[20] = "";
  static uint16_t count = 0;
  const int16_t x = 0, y = 200;
  int16_t x1, y1;
  uint16_t w, h;

  tft.setFont(&FreeSans9pt7b);
  tft.getTextBounds(text, x, y, &x1, &y1, &w, &h);
  tft.fillRect(x1, y1, w, h, ILI9341_BLACK);

  snprintf(text, sizeof(text), "Count: %u", count++);
  tft.setCursor(x, y);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(text);
  delay(1000);
}
