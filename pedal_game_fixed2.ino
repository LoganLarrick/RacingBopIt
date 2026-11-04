#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <DFRobotDFPlayerMini.h>

// ---------------- Pins ----------------
const uint8_t gas_pedal    = A0;
const uint8_t brake_pedal  = A1;
const uint8_t clutch_pedal = A2;

const uint8_t shift_up   = 6;   // INPUT_PULLUP: pressed = LOW
const uint8_t shift_down = 7;   // INPUT_PULLUP: pressed = LOW

// ---------------- Audio ----------------
DFRobotDFPlayerMini myDFPlayer;
bool audioOK = false;

const uint8_t SND_INTRO   = 1;
const uint8_t SND_GO      = 2;
const uint8_t SND_CORRECT = 9;
const uint8_t SND_CRASH   = 10;
const uint8_t SND_WIN     = 11;

void playSound(uint8_t n) {
  if (audioOK) myDFPlayer.play(n);
}

// ---------------- Display ----------------
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

// ---------------- IMU / steering ----------------
#define STEER_SIGN -1
#define STEER_THRESHOLD 30.0

Adafruit_BNO055 bno = Adafruit_BNO055(55);

// ---------------- Game state ----------------
bool correct = true;
int score = 0, rand_select;
int time_limit, timer = 0;
int high_score = 0;
float steerZero = 0;
float steer = 0;

// Gas, Brake, Steer Left, Steer Right, Shift Up, Shift Down
bool action_select[6] = {false, false, false, false, false, false};

// ---------------- Forward declarations ----------------
bool checkInputs();
bool checkShifter();
bool steerLeft();
bool steerRight();
void readSteer();
float readRawX();

// ---------------- Helpers ----------------
void waitForRelease(void) {
  display.clearDisplay();
  uint16_t stuck = 0;
  while (checkInputs()) {
    delay(50);
    // Debug: if something stays "pressed" for ~2 s, print what it is
    if (++stuck == 40) {
      Serial.print(F("Stuck input -> gas:"));    Serial.print(digitalRead(gas_pedal));
      Serial.print(F(" brake:"));                Serial.print(digitalRead(brake_pedal));
      Serial.print(F(" clutch:"));               Serial.print(digitalRead(clutch_pedal));
      Serial.print(F(" up:"));                   Serial.print(digitalRead(shift_up));
      Serial.print(F(" down:"));                 Serial.print(digitalRead(shift_down));
      Serial.print(F(" steer:"));                Serial.println(steer);
    }
  }
}

bool checkInputs() {
  readSteer();
  if (digitalRead(gas_pedal) == HIGH ||
      digitalRead(clutch_pedal) == HIGH ||
      digitalRead(brake_pedal) == HIGH ||
      steerLeft() || steerRight() ||
      digitalRead(shift_up) == LOW ||      // FIXED: pullup inputs are LOW when pressed
      digitalRead(shift_down) == LOW) {    // (HIGH meant "always pressed")
    return true;
  }
  return false;
}

bool checkShifter() {
  return (digitalRead(shift_up) == LOW || digitalRead(shift_down) == LOW);
}

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
  display.print(F("WON: "));
  display.println(score);
  display.print(F("BEST: "));
  display.println(high_score);
  display.display();
}

float readRawX() {
  sensors_event_t event;
  bno.getEvent(&event);
  return event.orientation.x;
}

void readSteer() {
  steer = (readRawX() - steerZero) * STEER_SIGN;
  while (steer > 180) steer -= 360;
  while (steer < -180) steer += 360;
}

bool steerLeft()  { return steer >  STEER_THRESHOLD; }
bool steerRight() { return steer < -STEER_THRESHOLD; }

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);   // USB serial monitor (don't wait for it)

  Wire.begin();
  #if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(3000, true);
  #endif

  if (!display.begin(0x3C)) {
    Serial.println(F("OLED not found"));
    while (true) delay(100);
  }

  if (!bno.begin(OPERATION_MODE_IMUPLUS)) {
    Serial.println(F("BNO055 not found"));
    showText(F("NO BNO055"));
    while (1) delay(100);
  }
  delay(100);
  bno.setExtCrystalUse(true);
  delay(100);

  // ---- DFPlayer setup (FIXED: begin() attaches Serial1 to the player) ----
  Serial1.begin(9600);
  delay(500);  // give the DFPlayer time to power up

  // isACK = false: don't wait for the module's reply. Many modules/clones play
  // fine but never answer, which made begin() report "not found".
  if (!myDFPlayer.begin(Serial1, false, true)) {
    Serial.println(F("DFPlayer begin() returned false (continuing anyway)"));
  }
  audioOK = true;  // audio works, so always allow it
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(30);        // 0 to 30
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  // Pedals need external pull-downs (pressed = HIGH)
  pinMode(clutch_pedal, INPUT);
  pinMode(brake_pedal, INPUT);
  pinMode(gas_pedal, INPUT);
  pinMode(shift_up, INPUT_PULLUP);
  pinMode(shift_down, INPUT_PULLUP);

  randomSeed(analogRead(A3));
}

// ---------------- Main loop ----------------
void loop() {

  showText(F("PRESS GAS"), F("TO START"));
  playSound(SND_INTRO);

  while (digitalRead(gas_pedal) == LOW) {
    delay(1);
  }

  // FIXED: zero the steering BEFORE waitForRelease(). Otherwise steerZero is 0,
  // the raw heading is usually beyond +/-30 degrees, steerLeft()/steerRight()
  // read as "pressed", and waitForRelease() hangs forever.
  steerZero = readRawX();
  steer = 0;

  waitForRelease();

  score = 0;
  correct = true;
  time_limit = 2500;
  timer = 0;

  //////////////// COUNTDOWN ////////////////
  showText(F("GET READY"));
  delay(1000);
  showText(F("3"));
  delay(1000);
  showText(F("2"));
  delay(1000);
  showText(F("1"));
  delay(1000);

  steerZero = readRawX();
  steer = 0;

  showText(F("GO!"));
  playSound(SND_GO);
  delay(500);

  waitForRelease();

  //////////////// GAME ////////////////
  while (correct) {

    timer = -300;

    // Reset action select vector
    for (int i = 0; i < 6; i++) {
      action_select[i] = false;
    }

    // Select action
    rand_select = random(0, 6);
    action_select[rand_select] = true;

    switch (rand_select) {
      case 0: showText(F("GAS!")); break;
      case 1: showText(F("BRAKE!")); break;
      case 2: showText(F("STEER"), F("LEFT!")); break;
      case 3: showText(F("STEER"), F("RIGHT!")); break;
      case 4: showText(F("SHIFT"), F("UP!")); break;
      case 5: showText(F("SHIFT"), F("DOWN!")); break;
    }

    // Wait for an input; time out otherwise
    while (!checkInputs()) {
      delay(1);
      timer++;
      if (timer >= time_limit) {
        correct = false;
        break;
      }
    }

    // Check inputs against the command
    if (digitalRead(gas_pedal) == HIGH && correct) {
      if (!action_select[0]) correct = false;
    }

    if (correct) delay(30);
    readSteer();

    if (digitalRead(brake_pedal) == HIGH && correct) {
      if (!action_select[1]) correct = false;
    }

    if (steerLeft() && correct) {
      if (!action_select[2]) correct = false;
    }

    if (steerRight() && correct) {
      if (!action_select[3]) correct = false;
    }

    // Shifting without the clutch is always wrong
    if (checkShifter() && correct) {
      correct = false;
    }

    if (digitalRead(clutch_pedal) == HIGH && correct) {

      // FIXED: wrong if the command is neither shift up nor shift down
      // (was ||, which was always true)
      if (!action_select[4] && !action_select[5]) {
        correct = false;
      }
      else {
        // Extra time since shifting needs additional actions
        timer -= 250;
        while (!checkShifter()) {
          delay(1);
          timer++;
          if (timer >= time_limit) {
            correct = false;
            break;
          }
        }

        if (correct) {
          if (digitalRead(shift_up) == LOW && !action_select[4]) {
            correct = false;
          } else if (digitalRead(shift_down) == LOW && !action_select[5]) {
            correct = false;
          }
        }
      }
    }

    time_limit -= 5;

    if (correct) {
      score++;
      playSound(SND_CORRECT);
      waitForRelease();
      showText(F("ZOOOM!"));
      delay(500);
    }

    // Upper limit score
    if (score == 100 && correct) {
      correct = false;
    }
  }

  //////////////// GAME OVER ////////////////
  if (score == 100) {
    showText(F("YOU WIN!"));
    playSound(SND_WIN);
  } else {
    showText(F("YOU"), F("CRASHED!"));
    playSound(SND_CRASH);
  }

  if (score > high_score) {
    high_score = score;
  }

  delay(2000);
  showScore(F("GAME OVER"));

  // Wait for gas pedal to restart
  while (digitalRead(gas_pedal) == LOW) {
    delay(1);
  }
}
