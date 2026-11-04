#include <DFRobotDFPlayerMini.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <DFRobotDFPlayerMini.h>

const uint8_t gas_pedal    = A0;
const uint8_t brake_pedal  = A1;
const uint8_t clutch_pedal = A2;

const uint8_t shift_up   = 6;
const uint8_t shift_down = 7;

DFRobotDFPlayerMini myDFPlayer;
bool audioOK = false;

const uint8_t SND_GO    = 1;
const uint8_t SND_ZOOM  = 2;
const uint8_t SND_CRASH = 3;
const uint8_t SND_WIN   = 4;

#define OLED_CS     10
#define OLED_DC     12
#define OLED_RESET   9
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

#define STEER_SIGN -1
#define STEER_THRESHOLD 30.0

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
float steer = 0;

//Gas, Brake, Steer Left, Steer Right, Shift Up, Shift Down
bool action_select[6] = {false, false, false, false, false, false};

void waitForRelease(void){
  display.clearDisplay();
  while (checkInputs()){
    delay(50);
  }
}

bool checkInputs() {
  if (digitalRead(gas_pedal) == HIGH||digitalRead(clutch_pedal) == HIGH||digitalRead(brake_pedal) == HIGH||steerLeft()||steerRight()||digitalRead(shift_up) == HIGH||digitalRead(shift_down) == HIGH){
    return true;
  }
  return false;
}

bool checkShifter() {
  if (digitalRead(shift_up) == HIGH || digitalRead(shift_down) == HIGH){
    return true;
  }
  return false;
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
  display.print(F("RACES WON: "));
  display.println(score);
  display.print(F("Best: "));
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

void setup() {
  Wire.begin();
  #if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(3000, true);
  #endif

  if (!display.begin(0x3C)) {
    while (true) delay(100);
  }

  if (!bno.begin(OPERATION_MODE_IMUPLUS)) {
    showText(F("NO BNO055"));
    while (1);
  }
  delay(100);
  bno.setExtCrystalUse(true);
  delay(100);

  //DFPlayer setup
  Serial.begin(115200);   // USB serial monitor
  Serial1.begin(9600);

  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(30);        // 0 to 30
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  pinMode(clutch_pedal, INPUT);
  pinMode(brake_pedal, INPUT);
  pinMode(gas_pedal, INPUT);
  pinMode(shift_up, INPUT_PULLUP);
  pinMode(shift_down, INPUT_PULLUP);

  randomSeed(analogRead(A3));
}

void loop() {

  showText(F("PRESS GAS"), F("TO START"));
  myDFPlayer.play(1);

  while (digitalRead(gas_pedal) == LOW){
    delay(1);
  }

  waitForRelease();

  score = 0;
  correct = true;
  time_limit = 2500;
  timer = 0;

  ////////////////////////////////////////////
  //BEGINNING GAME VIDEO/AUDIO AND COUNTDOWN
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
  myDFPlayer.play(2);
  delay(500);

  waitForRelease();
  
  ///////////////////////////////////////////

  while (correct){

    timer = -300;
    //Resetting action select vector
    for (int i = 0; i < 6; i++) {
      action_select[i] = false;
    }
    
    //Selecting action
    rand_select = random(0, 6);
    action_select[rand_select] = true;

    //Playing sound of selected command
    // switch (rand_select) {
    //   case 0:
    //   //PLAY GAS COMMAND
    //   myDFPlayer.play(3);
    //   break;

    //   case 1:
    //   //PLAY BRAKE COMMAND
    //   myDFPlayer.play(4);
    //   break;

    //   case 2:
    //   //PLAY STEER LEFT COMMAND
    //   myDFPlayer.play(5);
    //   break;

    //   case 3:
    //   //PLAY STEER RIGHT COMMAND
    //   myDFPlayer.play(6);
    //   break;

    //   case 4:
    //   //PLAY SHIFT UP COMMAND
    //   myDFPlayer.play(7);
    //   break;

    //   case 5:
    //   //PLAY SHIFT DOWN COMMAND
    //   myDFPlayer.play(8);
    // }

    //Checking if an input is made; timeout otherwise
    while (!checkInputs()){
      delay(1);
      timer++;
      if (timer >= time_limit){
        correct = false;
        break;
      }
    }

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

    // CHECKING INPUTS TO SEE IF THEY ALIGN WITH COMMANDS
    //MAKE SURE ALL PIN NUMBERS ARE ASSIGNED IN IF STATEMENTS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if(digitalRead(gas_pedal) == HIGH && correct){
      if (!action_select[0]){
        correct = false;
      }
    }

    if (correct) delay(30);
    readSteer();

    if(digitalRead(brake_pedal) == HIGH && correct){
      if (!action_select[1]){
        correct = false;
      }
    }

    if(steerLeft() && correct){
      if (!action_select[2]){
        correct = false;
      }
    }

    if(steerRight() && correct){
      if (!action_select[3]){
        correct = false;
      }
    }

    if(checkShifter() && correct){
      correct = false;
    }

    if(digitalRead(clutch_pedal)==HIGH && correct){

      //Incorrect input if clutch is pressed without a shift command
      if (!action_select[4] || !action_select[5]){
        correct = false;
      } 
      else {
        
        //Small additional time since shifter requires additional actions
        timer -= 250;
        while (!checkShifter()){
          delay(1);
          timer++;
          if (timer >= time_limit){
            correct = false;
            break;
          }
        }

        if (digitalRead(shift_up) == HIGH && !action_select[4]){
          correct = false;
        } else if (digitalRead(shift_down) == HIGH && !action_select[5]){
          correct = false;
        }
      }
    }
    time_limit -= 5;

    if (correct) {
      score++;
      //OLED DISPLAY CORRECT
      myDFPlayer.play(9);
      waitForRelease();
      showText(F("ZOOOM!"));
      delay(500);
    }


    //Upper limit score    
    if (score == 100 && correct){
      correct = false;
    }
  }

  if (score == 100){
    //YOU WIN DISPLAY/AUDIO
    showText(F("YOU WIN!"));
    myDFPlayer.play(11);
  } else {
    //YOU CRASHED DISPLAY/AUDIO
    showText(F("YOU"), F("CRASHED!"));
    myDFPlayer.play(10);
  }

  if (score > high_score){
    high_score = score;
  }

  //FINAL SCORE DISPLAY
  while (digitalRead(gas_pedal) == LOW){
    delay(1);
  }
}

