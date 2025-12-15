// 最简单的测试 - 只画一个像素点
// 接线：GND->GND, VCC->5V, D0->D13, D1->D11, RES->D8, DC->D7, CS->D9

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
  Serial.println(F("===== 简单测试 ====="));

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("[错误] 初始化失败"));
    while (1);
  }

  Serial.println(F("[成功] 初始化完成"));

  // 测试1: 清屏
  Serial.println(F("测试1: 清屏"));
  display.clearDisplay();
  display.display();
  delay(1000);

  // 测试2: 画一个像素点
  Serial.println(F("测试2: 画像素点 (64, 32)"));
  display.drawPixel(64, 32, SSD1306_WHITE);
  display.display();
  delay(1000);

  // 测试3: 画一条线
  Serial.println(F("测试3: 画线"));
  display.clearDisplay();
  display.drawLine(0, 0, 127, 63, SSD1306_WHITE);
  display.display();
  delay(1000);

  // 测试4: 画矩形
  Serial.println(F("测试4: 画矩形"));
  display.clearDisplay();
  display.drawRect(10, 10, 50, 30, SSD1306_WHITE);
  display.display();
  delay(1000);

  // 测试5: 显示文字
  Serial.println(F("测试5: 显示文字"));
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println(F("HELLO"));
  display.display();

  Serial.println(F("所有测试完成！"));
}

void loop() {
  // 空
}

