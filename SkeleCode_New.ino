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
