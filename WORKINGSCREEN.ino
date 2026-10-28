
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>

// Arduino Uno hardware SPI pins:
// SCK  = D13
// MOSI = D11

#define OLED_CS     10
#define OLED_DC     12
#define OLED_RESET   9

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

Adafruit_SSD1305 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &SPI,
  OLED_DC,
  OLED_RESET,
  OLED_CS,
  4000000UL
);

void setup() {
  Serial.begin(9600);
  Serial.println("Starting OLED...");

  if (!display.begin(0x3C)) {
    Serial.println("OLED initialization failed!");
    while (true) {
      delay(100);
    }
  }

  Serial.println("OLED initialized!");

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setTextWrap(true);
  display.setCursor(0, 0);

  display.println("OLED WORKING!");
  display.println();
  display.println("JIMMY");
  display.println("LOGAN");
  display.println("DADDYS LIL MONSTER");

  display.display();
}

void loop() {
  // Keep the message on the screen.
}
