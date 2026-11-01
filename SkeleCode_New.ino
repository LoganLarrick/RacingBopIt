void setup() {
  // put your setup code here, to run once:
  bool correct = true;
  int score = 0, rand_select;
  int time_limit, timer = 0;
  int high_score = 0;

  //Gas, Brake, Steer Left, Steer Right, Shift Up, Shift Down
  bool action_select[6] = {false, false, false, false, false, false};
}

void loop() {

  //Startup screen OLED
  //Startup audio

  while (digitalRead(gas_pedal) == LOW){
    delay(1);
  }

  score = -1;
  correct = true;
  time_limit = 2500;
  timer = 0;

  ////////////////////////////////////////////
  //BEGINNING GAME VIDEO/AUDIO AND COUNTDOWN
  ///////////////////////////////////////////

  while (correct){
    score++;
    timer = 0;
    //Resetting action select vector
    for (int i = 0; i < 6; i++) {
      action_select[i] = false;
    }
    
    //Selecting action
    rand_select = random(0, 6);
    action_select[rand_select] = true;

    //Playing sound of selected command
    switch (rand_select) {
      case 0:
      //PLAY GAS COMMAND
      break;

      case 1:
      //PLAY BRAKE COMMAND
      break;

      case 2:
      //PLAY STEER LEFT COMMAND
      break;

      case 3:
      //PLAY STEER RIGHT COMMAND
      break;

      case 4:
      //PLAY SHIFT UP COMMAND
      break;

      case 5:
      //PLAY SHIFT DOWN COMMAND
    }

    //Checking if an input is made; timeout otherwise
    while (!checkInputs()){
      delay(1);
      timer++;
      if (timer >= time_limit){
        correct = false;
        break;
      }
    }

    //CHECKING INPUTS TO SEE IF THEY ALIGN WITH COMMANDS
    //MAKE SURE ALL PIN NUMBERS ARE ASSIGNED IN IF STATEMENTS
    if(digitalRead(gas_pedal)==HIGH && correct){
      if (!action_select[0]){
        correct = false;
      }
    }
    if(digitalRead(brake_pedal)==HIGH && correct){
      if (!action_select[1]){
        correct = false;
      }
    }
    if(digitalRead(steer_left)==HIGH && correct){
      if (!action_select[2]){
        correct = false;
      }
    }
    if(digitalRead(steer_right)==HIGH && correct){
      if (!action_select[3]){
        correct = false;
      }
    }
    if(digitalRead(shift up)==HIGH && correct){
      if (!action_select[4]){
        correct = false;
      }
    }
    if(digitalRead(shift_down)==HIGH && correct){
      if (!action_select[5]){
        correct = false;
      }
    }

    time_limit -= 5;


    //Upper limit score    
    if (score == 99 && correct){
      correct = false;
      score++;
    }
  }

  if (score == 100){
    //YOU WIN DISPLAY/AUDIO
  } else {
    //YOU CRASHED DISPLAY/AUDIO
  }

  if (score > high_score){
    high_score = score;
  }

  //FINAL SCORE DISPLAY
}

bool checkInputs() {
  if (digitalRead(gas_pedal)||digitalRead(clutch_pedal)||digitalRead(brake_pedal)||digitalRead(steer_left)||digitalRead(steer_right)||digitalRead(shift_up)||digitalRead(shift_down)){
    return true;
  }
  return false;
}