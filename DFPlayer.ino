#include <DFRobotDFPlayerMini.h>

DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(115200);   // USB serial monitor
  Serial1.begin(9600);    // DFPlayer on pins 0 (RX) and 1 (TX)

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

  myDFPlayer.play(1);           // play 0001.mp3
}

void loop() {
  // Print status messages from the player
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());
  }
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println("Time out");
      break;
    case DFPlayerPlayFinished:
      Serial.print("Track finished: ");
      Serial.println(value);
      break;
    case DFPlayerError:
      Serial.print("Error: ");
      switch (value) {
        case Busy:          Serial.println("Card not found"); break;
        case FileMismatch:  Serial.println("File not found"); break;
        case Sleeping:      Serial.println("Sleeping"); break;
        case SerialWrongStack: Serial.println("Bad stack"); break;
        case CheckSumNotMatch: Serial.println("Checksum error"); break;
        default:            Serial.println(value); break;
      }
      break;
    default:
      break;
  }
}

