#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#define TFT_RST               D0
#define TFT_DC                D1
#define TFT_CS                D2
#define TFT_BACKLIGHT         D3
#define SPI_SPEED             40000000
#define ROTATION              1

#define SCREEN_WIDTH          320
#define SCREEN_HEIGHT         240

#define SERIAL_SPEED          9600

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 canvas = GFXcanvas16(SCREEN_WIDTH, SCREEN_HEIGHT);

int16_t angle = 0;

void setup(void)
{
  Serial.begin(SERIAL_SPEED);
  Serial.println("Start up.");
  initializeTFT();
}

void loop()
{
  canvas.fillScreen(ST77XX_BLACK);
  canvas.drawRotatedRectangle(44, 44, 40, 30, angle, 0xFFFF);
  canvas.fillRotatedRectangle(84, 84, 40, 30, angle, ST77XX_GREEN);
  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT);
  ++angle %= 360;
}

void initializeTFT(void)
{
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);
  tft.init(SCREEN_WIDTH, SCREEN_HEIGHT);
  tft.setSPISpeed(SPI_SPEED);
  tft.setRotation(ROTATION);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);

  Serial.println("TFT initialized.");
  tft.println("TFT initialized.");
}
