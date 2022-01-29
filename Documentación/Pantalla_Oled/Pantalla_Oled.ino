#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ANCHO 128
#define ALTO 64

#define OLED_RESET 4 
Adafruit_SSD1306 oled(ANCHO, ALTO, &Wire, OLED_RESET);

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  oled.begin(SSD1306_SWITCHCAPVCC, 0X3C);
}

void loop() {
  // put your main code here, to run repeatedly:
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.setTextSize(1);
  oled.print("Hola, han pasado: ");
  oled.setCursor(10, 30);
  oled.setTextSize(2);
  oled.print(millis());
  oled.print(" seg.");
  oled.display();
}
