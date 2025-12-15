/*
  GFX Library Test - 测试新增的绘图方法
  drawPentagram(), fillPentagram(), drawEllipse(), fillEllipse()

  硬件连接 (Arduino UNO + SSD1306 OLED SPI 7针):
  - GND -> GND
  - VCC -> 5V
  - D0 (CLK)  -> D13
  - D1 (MOSI) -> D11
  - RES -> D9
  - DC  -> D8
  - CS  -> D10
*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI  11
#define OLED_CLK   13
#define OLED_DC    8
#define OLED_CS    10
#define OLED_RESET 9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void setup() {
  Serial.begin(9600);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("GFX Test Start"));
  display.display();
  delay(2000);
}

void loop() {
  testDrawPentagram();
  delay(2000);
  
  testFillPentagram();
  delay(2000);
  
  testDrawEllipse();
  delay(2000);
  
  testFillEllipse();
  delay(2000);
  
  testAllShapes();
  delay(3000);
}

void testDrawPentagram() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("drawPentagram()"));
  
  display.drawPentagram(64, 40, 20, SSD1306_WHITE);
  
  display.display();
}

void testFillPentagram() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("fillPentagram()"));
  
  display.fillPentagram(64, 40, 20, SSD1306_WHITE);
  
  display.display();
}

void testDrawEllipse() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("drawEllipse()"));
  
  display.drawEllipse(64, 36, 40, 20, SSD1306_WHITE);
  display.drawEllipse(64, 36, 20, 25, SSD1306_WHITE);
  
  display.display();
}

void testFillEllipse() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("fillEllipse()"));
  
  display.fillEllipse(64, 40, 35, 18, SSD1306_WHITE);
  
  display.display();
}

void testAllShapes() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("All Shapes"));
  
  display.drawPentagram(25, 40, 15, SSD1306_WHITE);
  display.drawEllipse(70, 40, 25, 15, SSD1306_WHITE);
  display.fillEllipse(110, 40, 12, 18, SSD1306_WHITE);
  
  display.display();
}

