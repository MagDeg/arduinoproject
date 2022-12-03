#include <SPI.h>
#include <Wire.h>

#include <MFRC522.h>  //including RFID detctor libary
#define SS_PIN 53 
#define RST_PIN 6
MFRC522 mfrc522(SS_PIN, RST_PIN); //setting and initalizing of RFID pins

#include "MessageHandler.h" //including our own libary, which handles the communication with serial port
MessageHandler messageHandler;  //initializing class
HardwareSerial& serialCom = Serial3; //set variable for the serial port to easy change it while debugging

#include "display.h"  //including display libary
Display lcd(0x27, 16, 2); //initializing display

#include "ButtonHandler.h" //including own buttonHandler class

//initializing variables for button pins
const int CALBUT = 2;
const int LOCBUT = 3;
const int HORBUT = 4;
const int FREBUT = 5;

ButtonHandler buttonHandler(CALBUT, LOCBUT, HORBUT, FREBUT);  //initializing buttonhandler class with buttonPins

//joystick pins
const int JOYHORZ = A0;
const int JOYVERT = A1;

//poti pin
const int POTI = A2;

//value of joystick axis
int joyHorz = 0;
int joyVert = 0;

int potiState = 0;  //value of poti

bool calibrationMode = false; //variable for the output on the display

bool unlocked = false;  //status if remote control is unlocked

int keyId[] {108, 53, 208, 110};  //id of key chip for rfid unlocking
int phoneId[] {1, 2, 3, 4}; //id of rfid tag of phone

unsigned long MESSAGE_SEND_INTERVALL = 100; //duration to send message
unsigned long timeSendMessage = 0;  //time sending of last message
unsigned long time = 0; //time now

void setup() {
  lcd.init(); //initializing lcd display

  //initializing serial ports
  Serial.begin(9600);
  serialCom.begin(9600);

  buttonHandler.init(); //initializing ButtonHandler class

  SPI.begin();  //initializing communication with display
  mfrc522.PCD_Init(); //initializing RFID detector
}

void loop() {
  time = millis();  //setting time now

  if (unlocked == true) { //if control device is unlocked...
    lcd.displayClear(); //clearing display in duration of 5 seconds
    joyRead();  //reading joystick and poti values and sending to esp

    if (buttonHandler.isButtonPressed(LOCBUT)) {  //if lockbutton is pressed...
      unlocked = false; //lock device
      lcd.print(0, 0, "Gesperrt", true);  //print on display
    }

    if (buttonHandler.isButtonPressed(CALBUT) && (calibrationMode == false)) {  //if calibration button is pressed adn it wasn't calibrated so far...
      calibrationMode = true; 
      calibMode();  //print on display according to state of calibrationMode
      messageHandler.sendMessage(serialCom, '2', " ");  //sending message to calibrate car
    }

    if (calibrationMode == true) { //while calibration...
      lcd.printWait();  //printing wait indicator on display
    }

    if (buttonHandler.isButtonPressed(HORBUT)) {  //if horn button is pressed...
      messageHandler.sendMessage(serialCom, '4', " "); //sending order to car
      delay(500); //button cooldown
    }

  

    messageHandler.pollMessage(serialCom);  //getting message from serial
    if (messageHandler.isMessageAvailable()) {  //if an correct message is available...

      Serial.println("Message available");  //debug print

      char id;
      const char* data = messageHandler.getMessage(&id);

      // handle message
      switch (id) { //do sth. according to id of message
        case 'a': {
            Serial.println("data got"); //debug print
            calibrationMode = false;  //resetting calibrationMode
            calibMode();  //display printing according to calibrationMode
            break;  //get out of loop
          }
      }
    }
  } else if (unlocked == false) { //if control device not unlocked...
    readRFID(); //read RFID tag
  }
}

void joyRead() {
  if ((time - timeSendMessage) >= MESSAGE_SEND_INTERVALL) { //sending data every second to cooldown the serial port
    //setting values
    joyHorz = 1023 - analogRead(A0);  //inverting because joystick is build in the wrong direction  
    joyVert = 1023 - analogRead(A1);  //inverting because joystick is build in the wrong direction
    potiState = analogRead(POTI);

    char msg[MessageHandler::MESSAGE_BUFF_SIZE];
    sprintf(&(msg[0]), "%d;%d;%d", joyHorz, joyVert, potiState);   
    messageHandler.sendMessage(serialCom, '1', msg);  //sending message according to specific sheme(line above)
    timeSendMessage = time; //actualization of time senidng last message
  }
}

void readRFID() {
  //if same card is still present return 
  if (!mfrc522.PICC_IsNewCardPresent()) {
    unlocked = false;
    lcd.print(0, 0, "Bitte entsperren");
    return;
  }

  //if no card is available return
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  //if id is matching unlock device
  if ((mfrc522.uid.uidByte[0] == keyId[0] && mfrc522.uid.uidByte[1] == keyId[1] && mfrc522.uid.uidByte[2] == keyId[2] && mfrc522.uid.uidByte[3] == keyId[3]) || (mfrc522.uid.uidByte[0] == phoneId[0] && mfrc522.uid.uidByte[1] == phoneId[1] && mfrc522.uid.uidByte[2] == phoneId[2] && mfrc522.uid.uidByte[3] == phoneId[3])) {
    unlocked = true;
    if((mfrc522.uid.uidByte[0] == phoneId[0] && mfrc522.uid.uidByte[1] == phoneId[1] && mfrc522.uid.uidByte[2] == phoneId[2] && mfrc522.uid.uidByte[3] == phoneId[3])){
      lcd.clear();
      lcd.print(0, 0, "Willkommen");
      lcd.print(1, 0, "Magnus");
      

    } else {
      lcd.print(0, 0, "Entsperrt", true);
    }
      if (digitalRead(FREBUT) == HIGH) {
        delay(1500);
        lcd.clear();
        lcd.print(0, 0, "System wird");
        lcd.print(1, 0, "vorbereitet!");
        delay(1500);
        lcd.clear();
        lcd.progressIndicator();
        lcd.clear();
        lcd.print(0, 0, "Startbereit");
      }

    return;
  }
  unlocked = false;
  lcd.print(0, 0, "Bitte entsperren");
}

void calibMode() {
  if (calibrationMode == true) {
    lcd.print(0, 0, "Kalibrierung:", true);
    lcd.print(1, 0, "gestartet");
  };
  if (calibrationMode == false) {
    lcd.print(0, 0, "Kalibrierung:", true);
    lcd.print(1, 0, "abgeschlossen");
  }
}
