/*
  GFX Library Test - 测试新增的绘图方法
  drawPentagram(), fillPentagram(), drawEllipse(), fillEllipse()

  硬件连接 (Arduino UNO + SSD1306 OLED SPI 7针):
  - GND -> GND
  - VCC -> 5V
  - D0/SCLK -> D13
  - D1/MOSI -> D11
  - RES/RST -> D8
  - DC      -> D7
  - CS      -> D9
*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_DC   7
#define OLED_RST  8
#define OLED_CS   9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println(F("===== GFX Test 启动 ====="));
  Serial.println(F("[OLED] 开始初始化 display.begin() ..."));

  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("[OLED] 初始化失败！请检查："));
    Serial.println(F(" 1) VCC/GND 是否接反"));
    Serial.println(F(" 2) 电压是否正确(3.3V 还是 5V)"));
    Serial.println(F(" 3) D0->D13, D1->D11, RST->D8, DC->D7, CS->D9"));
    Serial.println(F(" 4) 屏幕芯片是否真的是 SSD1306"));
    for(;;);
  }

  Serial.println(F("[OLED] 初始化成功！"));

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("GFX Test Start"));
  display.display();

  Serial.println(F("[OLED] 已显示 'GFX Test Start'"));
  delay(2000);
}

void loop() {
  Serial.println(F("[Loop] 开始测试 drawPentagram"));
  testDrawPentagram();
  delay(2000);

  Serial.println(F("[Loop] 开始测试 fillPentagram"));
  testFillPentagram();
  delay(2000);

  Serial.println(F("[Loop] 开始测试 drawEllipse"));
  testDrawEllipse();
  delay(2000);

  Serial.println(F("[Loop] 开始测试 fillEllipse"));
  testFillEllipse();
  delay(2000);

  Serial.println(F("[Loop] 开始测试 All Shapes"));
  testAllShapes();
  delay(3000);

  Serial.println(F("[Loop] 一轮测试完成\n"));
}

void testDrawPentagram() {
  Serial.println(F("  清屏..."));
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("drawPentagram()"));

  Serial.println(F("  绘制五角星..."));
  display.drawPentagram(64, 40, 20, SSD1306_WHITE);

  Serial.println(F("  刷新显示..."));
  display.display();
  Serial.println(F("  完成"));
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

