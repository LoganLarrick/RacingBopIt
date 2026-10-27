//This is Nate Weikel's edit to the code
//vars
//vars\\


///////////////////////////////////////////////


//JIMMY CATALANO WAS HERE


///////////////////////////////////////////////


const int buttonPin = 7;  // the number of the pushbutton pin
const int ledPin = 8;    // the number of the LED pin

int buttonState = 0;  // variable for reading the pushbutton status

void setup() {
  //pin initializing
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop() {
  //read state
  buttonState = digitalRead(buttonPin);

  // check if pressed
  if (buttonState == HIGH) {
    //LED on
    digitalWrite(ledPin, HIGH);
    delay(100);
    //LED off
    digitalWrite(ledPin, LOW);
    delay(100);
  } else {
    //LED off
    digitalWrite(ledPin, LOW);
  }
}
