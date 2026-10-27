//vars
const int buttonPin = 7; 
const int ledPin1 = 8;   
const int ledPin2 = 6;   
const int ledPin3 = 5;    

int buttonState = 0;  // variable for reading the pushbutton status

void setup() {
  //pin initializing
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop() {
  //read state
  buttonState = digitalRead(buttonPin);

  // check if pressed
  if (buttonState == HIGH) {
    //First LED
    //LED on
    digitalWrite(ledPin1, HIGH);
    delay(100);
    //LED off
    digitalWrite(ledPin1, LOW);
    delay(100);

    //Second LED
    //LED on
    digitalWrite(ledPin2, HIGH);
    delay(100);
    //LED off
    digitalWrite(ledPin2, LOW);
    delay(100);

    //Third LED
    //LED on
    digitalWrite(ledPin3, HIGH);
    delay(100);
    //LED off
    digitalWrite(ledPin3, LOW);
    delay(100);
  } else {
    //LED off
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
  }
}
