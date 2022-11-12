#include <SPI.h>
#include <MFRC522.h>

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "MessageHeader.h"
MessageHandler messageHandler;

#define SS_PIN 53
#define RST_PIN 6

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522(SS_PIN, RST_PIN);
bool correctKey = false;

const int JOYHORZ = A0;
const int JOYVERT = A1;

const int CALBUT = 2;
const int LOCBUT = 3;
const int HORBUT = 4;
const int FREBUT = 5;

const int POTI = A2;

int joyHorz = 0;
int joyVert = 0;

bool calibButton = false;
bool calibrationMode = false;
bool lockButton = false;
bool hornButton = false;
bool freeButton = false;

int speedState = 0;



void setup() {
  Serial.begin(9600);
  Serial2.begin(9600 * 4);

  pinMode(CALBUT, INPUT);
  pinMode(FREBUT, INPUT);
  pinMode(HORBUT, INPUT);
  pinMode(LOCBUT, INPUT);

  SPI.begin();
  mfrc522.PCD_Init();

  lcd.init();
  lcd.backlight();
}

void loop() {
  joyRead();
  buttonRead();
  potiRead();
  readRFID();

  if (correctKey == true) {
    lcd.setCursor(0, 0);
    lcd.print("Entsperrt");
  }


  messageHandler.sendMessage(Serial2, 1, 100);
  Serial.println("data send");

  messageHandler.pollMessage(Serial2);
    if (messageHandler.isMessageAvailable()) {

      Serial.println("Message available");
      int id;
      int data;
      messageHandler.getMessage(id, data);

      // handle message
      switch (id) {
        case 1: {
        Serial.print(temp++);
        break;}
      }
    }


  calibMode();


  Serial.println("Horz: " + String(joyHorz) + " Vert: " + String(joyVert) + " Cal: " + String(calibButton) + " Hor: " + String(hornButton) + " Loc: " + String(lockButton) + " Free: " + String(freeButton) + " Spe: " + String(speedState));
}

void buttonRead() {
  calibButton = digitalRead(CALBUT);
  lockButton = digitalRead(LOCBUT);
  hornButton = digitalRead(HORBUT);
  freeButton = digitalRead(FREBUT);
}

void joyRead() {
  joyHorz = 1023 - analogRead(A0);
  joyVert = 1023 - analogRead(A1);
}

void potiRead() {
  speedState = analogRead(POTI);
}

void readRFID() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  if (mfrc522.uid.uidByte[0] == 108 && mfrc522.uid.uidByte[1] == 53 && mfrc522.uid.uidByte[2] == 208 && mfrc522.uid.uidByte[3] == 110) {
    correctKey = true;
  } else {
    correctKey = false;
  }
}

void progressIndicator() {
  int progressPos = 3;


  for (int i = 0; i < 100; i++) {
    lcd.setCursor(0, 0);
    lcd.print("Progress: " + String(i) + "%");

    lcd.setCursor(2, 1);
    lcd.print("[");
    lcd.setCursor(13, 1);
    lcd.print("]");

    delay(250);

    if (((i % 10) == 0) && (i != 0)) {
      lcd.setCursor(progressPos, 1);
      lcd.print("=");
      if (progressPos <= 12) {
        progressPos += 1;
      }
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Done!");
  delay(1000);
  lcd.clear();
}

void calibMode() {
  if (calibButton == true) {
    delay(5);
    calibrationMode = !calibrationMode;

    if (calibrationMode == true) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Kalibrierungs");
      lcd.setCursor(0, 1);
      lcd.print("Modus");
    };    
    if (calibrationMode == false) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Kalibrierung");
      lcd.setCursor(0, 1);
      lcd.print("beendet");
    }
  };
}