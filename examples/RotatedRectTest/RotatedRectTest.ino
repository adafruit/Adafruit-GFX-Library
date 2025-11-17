/***************************************************
  This is an example to exercise the rotated rectangle
  methods 'drawRotatedRect' and 'fillRotatedRect'.
  The commented lines represent hardware-specific
  details that will need to be customized for the
  particular display chosen by the end-user.
 ****************************************************/

// #include <SPI.h>
// #include <Adafruit_ST7789.h>
#include <Adafruit_GFX.h>

// #define TFT_RST               D0
// #define TFT_DC                D1
// #define TFT_CS                D2
// #define TFT_BACKLIGHT         D3
// #define SPI_SPEED             40000000
// #define ROTATION              1

#define SCREEN_WIDTH          128
#define SCREEN_HEIGHT         128

#define SERIAL_SPEED          9600

#define DRAW_CIRCLE_X         44
#define DRAW_CIRCLE_Y         44
#define FILL_CIRCLE_X         84
#define FILL_CIRCLE_Y         84
#define WIDTH                 40
#define HEIGHT                30
#define BLACK                 0x0000
#define WHITE                 0xFFFF
#define GREEN                 0x07E0


//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
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
  canvas.fillScreen(BLACK);
  canvas.drawRotatedRect(DRAW_CIRCLE_X, DRAW_CIRCLE_Y, WIDTH, HEIGHT, angle, WHITE);
  canvas.fillRotatedRect(FILL_CIRCLE_X, FILL_CIRCLE_Y, WIDTH, HEIGHT, angle, GREEN);
  //Un-comment to display the rotated rectangles on your display
  //tft.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT);
  ++angle %= 360;
}

void initializeTFT(void)
{
  // pinMode(TFT_BACKLIGHT, OUTPUT);
  // digitalWrite(TFT_BACKLIGHT, HIGH);
  // tft.init(SCREEN_WIDTH, SCREEN_HEIGHT);
  // tft.setSPISpeed(SPI_SPEED);
  // tft.setRotation(ROTATION);
}


