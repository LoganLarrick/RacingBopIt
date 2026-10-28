
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>

// Arduino Uno hardware SPI pins:
// SCK  = D13
// MOSI = D11

//setup pedal vars
const int clutchPin = A2; // Connect button to digital pin 2
int clutchState = 0;     // Variable for reading the button status
const int brakePin = A1; // Connect button to digital pin 2
int brakeState = 0;     // Variable for reading the button status
const int acceleratorPin = A0; // Connect button to digital pin 2
int acceleratorState = 0;     // Variable for reading the button status

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

  //setup pedals
  pinMode(clutchPin, INPUT);
  pinMode(brakePin, INPUT);
  pinMode(acceleratorPin, INPUT);



}

void loop() {
  // Keep the message on the screen.

  // read vals of the pedals
  clutchState = digitalRead(clutchPin);
  brakeState = digitalRead(brakePin);
  acceleratorState = digitalRead(acceleratorPin);

  // check if pedals pressed
  if (clutchState == HIGH) {
    Serial.println("Clutch Engaged");
  } else {
    Serial.println("Clutch Disengaged");
  }

  if (brakeState == HIGH) {
    Serial.println("Brake Engaged");
  } else {
    Serial.println("Brake Disengaged");
  }

  if (acceleratorState == HIGH) {
    Serial.println("Accelerator Engaged");
  } else {
    Serial.println("Accelerator Disengaged");
  }


  delay(50); // Small delay to help debounce the switch
}
