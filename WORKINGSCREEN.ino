#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

const uint8_t gas_pedal    = A0;
const uint8_t brake_pedal  = A1;
const uint8_t clutch_pedal = A2;

const uint8_t shift_up   = 2;
const uint8_t shift_down = 3;

const uint8_t vibratorPin = 8;

#define OLED_CS     10
#define OLED_DC     12
#define OLED_RESET   9
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

#define STEER_THRESHOLD 45

Adafruit_BNO055 bno = Adafruit_BNO055(55);

Adafruit_SSD1305 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &SPI,
  OLED_DC,
  OLED_RESET,
  OLED_CS,
  4000000UL
);

bool correct = true;
int score = 0, rand_select;
int time_limit, timer = 0;
int high_score = 0;
float steerZero = 0;

bool action_select[6] = {false, false, false, false, false, false};

void showText(const __FlashStringHelper* line1, const __FlashStringHelper* line2 = nullptr) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(line1);
  if (line2) display.println(line2);
  display.display();
}

void showScore(const __FlashStringHelper* title) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(title);
  display.print(F("RACES WON: "));
  display.println(score);
  display.print(F("Best: "));
  display.println(high_score);
  display.display();
}



bool steerLeft(float y)  { 
   y  < -STEER_THRESHOLD; }
bool steerRight(float y) { y >  STEER_THRESHOLD; }

bool checkShifter() {
  return digitalRead(shift_up) == HIGH || digitalRead(shift_down) == HIGH;
}

bool checkInputs(float y) {
  if (digitalRead(gas_pedal) == HIGH)    return true;
  if (digitalRead(clutch_pedal) == HIGH) return true;
  if (digitalRead(brake_pedal) == HIGH)  return true;
  if (checkShifter())                    return true;
  if (steerLeft(y) || steerRight(y))       return true;
  return false;
}

void waitForRelease(float y) {
  while (checkInputs(y)) {
    delay(5);
  }
}

void setup() {
  Serial.begin(9600);

  if (!display.begin(0x3C)) {
    Serial.println(F("OLED initialization failed!"));
    while (true) delay(100);
  }

  if (!bno.begin()) {
    Serial.println(F("No BNO055 detected. Check wiring or I2C address."));
    while (1);
  }
  bno.setExtCrystalUse(true);

  pinMode(clutch_pedal, INPUT);
  pinMode(brake_pedal, INPUT);
  pinMode(gas_pedal, INPUT);
  pinMode(shift_up, INPUT);
  pinMode(shift_down, INPUT);
  pinMode(vibratorPin, OUTPUT);
  digitalWrite(vibratorPin, LOW);

  randomSeed(analogRead(A3));
}

void loop() {

  showText(F("PRESS GAS"), F("TO START"));

  while (digitalRead(gas_pedal) == LOW) {
    delay(1);
  }

   sensors_event_t event; 
  bno.getEvent(&event);
  float y = event.orientation.y;

  score = -1;
  correct = true;
  time_limit = 2500;
  timer = 0;

  showText(F("GET READY"));
  delay(1000);
  showText(F("3"));
  delay(1000);
  showText(F("2"));
  delay(1000);
  showText(F("1"));
  delay(1000);
  showText(F("GO!"));
  delay(500);

  waitForRelease(y);

  while (correct) {
    score++;
    timer = 0;

    for (int i = 0; i < 6; i++) {
      action_select[i] = false;
    }

    rand_select = random(0, 6);
    action_select[rand_select] = true;

      sensors_event_t event; 
      bno.getEvent(&event);

    switch (rand_select) {
      case 0:
        showText(F("GAS!"));
        break;
      case 1:
        showText(F("BRAKE!"));
        break;
      case 2:
        showText(F("STEER"), F("LEFT!"));
        break;
      case 3:
        showText(F("STEER"), F("RIGHT!"));
        break;
      case 4:
        showText(F("SHIFT"), F("UP!"));
        break;
      case 5:
        showText(F("SHIFT"), F("DOWN!"));
        break;
    }

    while (!checkInputs(y)) {
      delay(1);
      timer++;
      if (timer >= time_limit) {
        correct = false;
        break;
      }
    }

    if (correct) delay(30);

    if (correct && digitalRead(gas_pedal) == HIGH) {
      if (!action_select[0]) correct = false;
    }

    if (correct && digitalRead(brake_pedal) == HIGH) {
      if (!action_select[1]) correct = false;
    }

    if (correct && steerLeft(y)) {
      if (!action_select[2]) correct = false;
    }

    if (correct && steerRight(y)) {
      if (!action_select[3]) correct = false;
    }

    if (correct && checkShifter() && digitalRead(clutch_pedal) == LOW) {
      correct = false;
    }

    if (correct && digitalRead(clutch_pedal) == HIGH) {

      if (!action_select[4] && !action_select[5]) {
        correct = false;
      }
      else {
        timer -= 500;
        while (!checkShifter()) {
          delay(1);
          timer++;
          if (timer >= time_limit) {
            correct = false;
            break;
          }
        }

        if (correct) {
          if (digitalRead(shift_up) == HIGH && !action_select[4]) {
            correct = false;
          } else if (digitalRead(shift_down) == HIGH && !action_select[5]) {
            correct = false;
          }
        }
      }
    }

    time_limit -= 5;

    if (score == 99 && correct) {
      correct = false;
      score++;
    }

    if (correct) {
      waitForRelease(y);
      showText(F("ZOOOM!"));
      delay(500);
    }
  }

  if (score == 100) {
    showText(F("YOU WIN!"));
  } else {
    showText(F("YOU"), F("CRASHED!"));
    digitalWrite(vibratorPin, HIGH);
    delay(500);
    digitalWrite(vibratorPin, LOW);
  }
  delay(2000);

  if (score > high_score) {
    high_score = score;
  }

  showScore(F("RACE OVER"));
  delay(4000);

  waitForRelease(y);
}