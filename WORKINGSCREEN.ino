#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <DFRobotDFPlayerMini.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);
DFRobotDFPlayerMini myDFPlayer;

// Arduino Uno hardware SPI pins:
// SCK  = D13
// MOSI = D11


//setup pedal vars
uint8_t clutchPin = A2; // Connect button to digital pin 2
uint8_t clutchState = 0;     // Variable for reading the button status
uint8_t brakePin = A1; // Connect button to digital pin 2
uint8_t brakeState = 0;     // Variable for reading the button status
uint8_t acceleratorPin = A0; // Connect button to digital pin 2
uint8_t acceleratorState = 0;     // Variable for reading the button status

//vibration motor pin
uint8_t vibratorPin = 8; // Connect button to digital pin 2

//microsdcard module pins
uint8_t resetPin = 4; // 
uint8_t clockPin = 5; // 
uint8_t dataPin = 6; //
uint8_t busyPin = 7; // 

//shifter
uint8_t shiftUpPin = 6; 
uint8_t shiftUpState = 0;    
uint8_t shiftDownPin = 7; 
uint8_t shiftDownState = 0;  



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
  Serial1.begin(9600);
  Serial.println("Starting OLED...");

  if (!display.begin(0x3C)) {
    Serial.println("OLED initialization failed!");
    while (true) {
      delay(100);
    }
  }

  Serial.println("OLED initialized!");

  //setup audio
  Serial.println("Initializing DFPlayer...");

  if (!myDFPlayer.begin(Serial1)) {
    Serial.println("DFPlayer init failed:");
    Serial.println("1. Check wiring and 1k resistor");
    Serial.println("2. Check SD card is inserted and FAT32");
    while (true);
  }

  Serial.println("DFPlayer ready.");

  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(30);        // 0 to 30
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);


  myDFPlayer.play(1);  

  Serial.println("Orientation Sensor Test"); Serial.println("");
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }


  //setup pedals
  pinMode(clutchPin, INPUT);
  pinMode(brakePin, INPUT);
  pinMode(acceleratorPin, INPUT);

  //vibrator setup
  pinMode(vibratorPin, OUTPUT);

  //shifter pin setup
  pinMode(shiftUpPin, INPUT);
  pinMode(shiftDownPin, INPUT);


  bno.setExtCrystalUse(true);


}

void loop() {
  // setup display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setTextWrap(true);
  display.setCursor(0, 0);
  // read vals of the pedals
  clutchState = digitalRead(clutchPin);
  brakeState = digitalRead(brakePin);
  acceleratorState = digitalRead(acceleratorPin);

  //read vals of the shifter
  shiftUpState = digitalRead(shiftUpPin);
  shiftDownState = digitalRead(shiftDownPin);

  //create random vibrations half of the time
  uint8_t randomVibration = random(0, 2);
  if(randomVibration == 1){
    digitalWrite(vibratorPin, HIGH);
  }
  else{
    digitalWrite(vibratorPin, LOW);
  }

  // check if pedals pressed
  if (clutchState == HIGH) {
    //Serial.println("Clutch Engaged");
    display.println("Clutch Engaged");
  } else {
    //Serial.println("Clutch Disengaged");
    display.println("Clutch Disengaged");
  }

  if (brakeState == HIGH) {
    //Serial.println("Brake Engaged");
    display.println("Brake Engaged");
  } else {
    //Serial.println("Brake Disengaged");
    display.println("Brake Disengaged");

  }

  if (acceleratorState == HIGH) {
    //Serial.println("Accelerator Engaged");
    display.println("Accelerator Engaged");
    //make the wheel vibrate when accelerating
    digitalWrite(vibratorPin, HIGH);
  } else {
    //Serial.println("Accelerator Disengaged");
    display.println("Accelerator Disengaged");
  }

  //shifter logic
  if (shiftUpState == HIGH) {
    display.println("Shifting Up");
  } 
  else if (shiftDownState == HIGH){
    display.println("Shifting Down");
  }
  else {
    display.println("Shifting Neutral");
  }

  /* Get a new sensor event */ 
  sensors_event_t event; 
  bno.getEvent(&event);
  
  if((event.orientation.y) > 45.0){
    //wheel if turned left
    display.println("LEFT TURN");
  }
  else if ((event.orientation.y) < -45.0){
    //wheel if turned right
    display.println("RIGHT TURN");
  }
  else{
    display.println("STRAIGHT");
  }

  display.display();

  delay(50); // Small delay to help debounce the switch
}
