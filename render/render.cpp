#include "Arduino.h"
#include "RenderGFX.h"
#include <cstdio>
#include <stdint.h>

// Update includes:
// for FILE in ../Fonts/*.h; do printf '#include "%s"\n' "$FILE"; done
#include "../Fonts/FreeMono12pt7b.h"
#include "../Fonts/FreeMono18pt7b.h"
#include "../Fonts/FreeMono24pt7b.h"
#include "../Fonts/FreeMono9pt7b.h"
#include "../Fonts/FreeMonoBold12pt7b.h"
#include "../Fonts/FreeMonoBold18pt7b.h"
#include "../Fonts/FreeMonoBold24pt7b.h"
#include "../Fonts/FreeMonoBold9pt7b.h"
#include "../Fonts/FreeMonoBoldOblique12pt7b.h"
#include "../Fonts/FreeMonoBoldOblique18pt7b.h"
#include "../Fonts/FreeMonoBoldOblique24pt7b.h"
#include "../Fonts/FreeMonoBoldOblique9pt7b.h"
#include "../Fonts/FreeMonoOblique12pt7b.h"
#include "../Fonts/FreeMonoOblique18pt7b.h"
#include "../Fonts/FreeMonoOblique24pt7b.h"
#include "../Fonts/FreeMonoOblique9pt7b.h"
#include "../Fonts/FreeSans12pt7b.h"
#include "../Fonts/FreeSans18pt7b.h"
#include "../Fonts/FreeSans24pt7b.h"
#include "../Fonts/FreeSans9pt7b.h"
#include "../Fonts/FreeSansBold12pt7b.h"
#include "../Fonts/FreeSansBold18pt7b.h"
#include "../Fonts/FreeSansBold24pt7b.h"
#include "../Fonts/FreeSansBold9pt7b.h"
#include "../Fonts/FreeSansBoldOblique12pt7b.h"
#include "../Fonts/FreeSansBoldOblique18pt7b.h"
#include "../Fonts/FreeSansBoldOblique24pt7b.h"
#include "../Fonts/FreeSansBoldOblique9pt7b.h"
#include "../Fonts/FreeSansOblique12pt7b.h"
#include "../Fonts/FreeSansOblique18pt7b.h"
#include "../Fonts/FreeSansOblique24pt7b.h"
#include "../Fonts/FreeSansOblique9pt7b.h"
#include "../Fonts/FreeSerif12pt7b.h"
#include "../Fonts/FreeSerif18pt7b.h"
#include "../Fonts/FreeSerif24pt7b.h"
#include "../Fonts/FreeSerif9pt7b.h"
#include "../Fonts/FreeSerifBold12pt7b.h"
#include "../Fonts/FreeSerifBold18pt7b.h"
#include "../Fonts/FreeSerifBold24pt7b.h"
#include "../Fonts/FreeSerifBold9pt7b.h"
#include "../Fonts/FreeSerifBoldItalic12pt7b.h"
#include "../Fonts/FreeSerifBoldItalic18pt7b.h"
#include "../Fonts/FreeSerifBoldItalic24pt7b.h"
#include "../Fonts/FreeSerifBoldItalic9pt7b.h"
#include "../Fonts/FreeSerifItalic12pt7b.h"
#include "../Fonts/FreeSerifItalic18pt7b.h"
#include "../Fonts/FreeSerifItalic24pt7b.h"
#include "../Fonts/FreeSerifItalic9pt7b.h"
#include "../Fonts/Org_01.h"
#include "../Fonts/Picopixel.h"
#include "../Fonts/Tiny3x3a2pt7b.h"
#include "../Fonts/TomThumb.h"
#include "../gfxfont.h"

#define SCALE 2
#define BORDER_SIZE 5 * SCALE

void render(const char *filename, GFXfont *font, bool correctBox = true) {
  printf("%s\n", filename);
  const char *text = "The Quick Brown Fox Jumps Over The Lazy Dog";
  int16_t sizex, sizey;
  uint16_t sizew, sizeh;

  RenderGFX render(1000, 1000);
  render.setTextWrap(false);
  render.setTextSize(SCALE);
  render.setFont(font);
  render.getTextBounds(text, 0, 0, &sizex, &sizey, &sizew, &sizeh);
  printf("Size: %d, %d, %d, %d\n", sizex, sizey, sizew, sizeh);

  render = RenderGFX(sizew + 2 * BORDER_SIZE, sizeh + 2 * BORDER_SIZE);
  int black = render.color(0, 0, 0);
  int white = render.color(255, 255, 255);
  int lgrey = render.color(200, 200, 200);

  int yoff = BORDER_SIZE - (correctBox ? sizey : 0);

  render.fillScreen(white);
  render.setFont(font);
  render.setCursor(BORDER_SIZE, yoff);
  render.setTextSize(SCALE);
  render.setTextColor(black);
  render.print(text);
  render.drawRect(BORDER_SIZE, BORDER_SIZE, sizew, sizeh, lgrey);
  render.save(filename);
}

void render(const char *filename, GFXfont font, bool correctBox = true) {
  render(filename, &font, correctBox);
}

int main(int argc, char **argv) {
  // Update render calls:
  // for FILE in ../Fonts/*.h; do
  //  NAME=$(basename "$FILE" .h);
  //  printf '  render("%s.png", %s);\n' "$NAME" "$NAME";
  // done
  render("FreeMono12pt7b.png", FreeMono12pt7b);
  render("FreeMono18pt7b.png", FreeMono18pt7b);
  render("FreeMono24pt7b.png", FreeMono24pt7b);
  render("FreeMono9pt7b.png", FreeMono9pt7b);
  render("FreeMonoBold12pt7b.png", FreeMonoBold12pt7b);
  render("FreeMonoBold18pt7b.png", FreeMonoBold18pt7b);
  render("FreeMonoBold24pt7b.png", FreeMonoBold24pt7b);
  render("FreeMonoBold9pt7b.png", FreeMonoBold9pt7b);
  render("FreeMonoBoldOblique12pt7b.png", FreeMonoBoldOblique12pt7b);
  render("FreeMonoBoldOblique18pt7b.png", FreeMonoBoldOblique18pt7b);
  render("FreeMonoBoldOblique24pt7b.png", FreeMonoBoldOblique24pt7b);
  render("FreeMonoBoldOblique9pt7b.png", FreeMonoBoldOblique9pt7b);
  render("FreeMonoOblique12pt7b.png", FreeMonoOblique12pt7b);
  render("FreeMonoOblique18pt7b.png", FreeMonoOblique18pt7b);
  render("FreeMonoOblique24pt7b.png", FreeMonoOblique24pt7b);
  render("FreeMonoOblique9pt7b.png", FreeMonoOblique9pt7b);
  render("FreeSans12pt7b.png", FreeSans12pt7b);
  render("FreeSans18pt7b.png", FreeSans18pt7b);
  render("FreeSans24pt7b.png", FreeSans24pt7b);
  render("FreeSans9pt7b.png", FreeSans9pt7b);
  render("FreeSansBold12pt7b.png", FreeSansBold12pt7b);
  render("FreeSansBold18pt7b.png", FreeSansBold18pt7b);
  render("FreeSansBold24pt7b.png", FreeSansBold24pt7b);
  render("FreeSansBold9pt7b.png", FreeSansBold9pt7b);
  render("FreeSansBoldOblique12pt7b.png", FreeSansBoldOblique12pt7b);
  render("FreeSansBoldOblique18pt7b.png", FreeSansBoldOblique18pt7b);
  render("FreeSansBoldOblique24pt7b.png", FreeSansBoldOblique24pt7b);
  render("FreeSansBoldOblique9pt7b.png", FreeSansBoldOblique9pt7b);
  render("FreeSansOblique12pt7b.png", FreeSansOblique12pt7b);
  render("FreeSansOblique18pt7b.png", FreeSansOblique18pt7b);
  render("FreeSansOblique24pt7b.png", FreeSansOblique24pt7b);
  render("FreeSansOblique9pt7b.png", FreeSansOblique9pt7b);
  render("FreeSerif12pt7b.png", FreeSerif12pt7b);
  render("FreeSerif18pt7b.png", FreeSerif18pt7b);
  render("FreeSerif24pt7b.png", FreeSerif24pt7b);
  render("FreeSerif9pt7b.png", FreeSerif9pt7b);
  render("FreeSerifBold12pt7b.png", FreeSerifBold12pt7b);
  render("FreeSerifBold18pt7b.png", FreeSerifBold18pt7b);
  render("FreeSerifBold24pt7b.png", FreeSerifBold24pt7b);
  render("FreeSerifBold9pt7b.png", FreeSerifBold9pt7b);
  render("FreeSerifBoldItalic12pt7b.png", FreeSerifBoldItalic12pt7b);
  render("FreeSerifBoldItalic18pt7b.png", FreeSerifBoldItalic18pt7b);
  render("FreeSerifBoldItalic24pt7b.png", FreeSerifBoldItalic24pt7b);
  render("FreeSerifBoldItalic9pt7b.png", FreeSerifBoldItalic9pt7b);
  render("FreeSerifItalic12pt7b.png", FreeSerifItalic12pt7b);
  render("FreeSerifItalic18pt7b.png", FreeSerifItalic18pt7b);
  render("FreeSerifItalic24pt7b.png", FreeSerifItalic24pt7b);
  render("FreeSerifItalic9pt7b.png", FreeSerifItalic9pt7b);
  render("Org_01.png", Org_01);
  render("Picopixel.png", Picopixel);
  render("Tiny3x3a2pt7b.png", Tiny3x3a2pt7b);
  render("TomThumb.png", TomThumb);
  render("DefaultFont.png", NULL);
  return 0;
}
