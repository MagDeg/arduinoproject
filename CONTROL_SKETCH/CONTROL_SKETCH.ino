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
bool calibState = false;
bool lockButton = false;
bool hornButton = false;
bool freeButton = false;

bool unlocked = false; //CHANGE!! AFTER DEBUGGING

int speedState = 0;

int keyId[] {108, 53, 208, 110};


void setup() {
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);

  pinMode(CALBUT, INPUT);
  pinMode(FREBUT, INPUT);
  pinMode(HORBUT, INPUT);
  pinMode(LOCBUT, INPUT);

  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
 if (unlocked == true) {    

      buttonRead();
      joyRead();   
      
    if (lockButton == true) {
      unlocked = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Gesperrt");
    }

    /*
    if (calibState == false) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bitte");
      lcd.setCursor(0, 1);
      lcd.print("Kalibrieren!");
    }
    */
    
    
    Serial.println(Serial2.read());
    messageHandler.pollMessage(Serial2);
    if (messageHandler.isMessageAvailable()) {

      Serial.println("Message available");
      byte id;
      const char* data = messageHandler.getMessage(&id);

      // handle message
      switch (id) {
        case 1: {
          Serial.println("data got");
          calibrationMode = false;
          calibMode();
        break;}
      }
    } else {
      //delay(500); 
   
      }
    
    
    calibMode();

    if (freeButton == true) {
      barrierDetected();
    }

    //Serial.println("Horz: " + String(joyHorz) + " Vert: " + String(joyVert) + " Cal: " + String(calibButton) + " Hor: " + String(hornButton) + " Loc: " + String(lockButton) + " Free: " + String(freeButton) + " Spe: " + String(speedState));
  } else if (unlocked == false) {
    readRFID();
  }

}

void buttonRead() {
  calibButton = digitalRead(CALBUT);
  
  if (calibButton == true) {
    messageHandler.sendMessage(Serial1, 4, "");    
  }
  

  lockButton = digitalRead(LOCBUT);

  hornButton = digitalRead(HORBUT);
  if (hornButton == true) {
    messageHandler.sendMessage(Serial1, 5, "");    
  }
  

  freeButton = digitalRead(FREBUT);
}

void joyRead() {
  joyHorz = 1023 - analogRead(A0);
  joyVert = 1023 - analogRead(A1);
  speedState = analogRead(POTI);


  char msg[MessageHandler::MESSAGE_BUFF_SIZE];
  sprintf(&(msg[0]),"%d;%d;%d", joyHorz, joyVert, speedState);

  messageHandler.sendMessage(Serial1, 1, msg);
}

void readRFID() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    unlocked = false;  
    lcd.setCursor(0, 0);
    lcd.print("Bitte entsperren"); 
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  if (mfrc522.uid.uidByte[0] == keyId[0] && mfrc522.uid.uidByte[1] == keyId[1] && mfrc522.uid.uidByte[2] == keyId[2] && mfrc522.uid.uidByte[3] == keyId[3]) {
    unlocked = true;
    lcd.clear();    
    lcd.setCursor(0, 0);
    lcd.print("Entsperrt");
    delay(2000);
    lcd.clear();
    return;
  } 
  unlocked = false;   
  lcd.setCursor(0, 0);
  lcd.print("Bitte entsperren");   
  
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
    messageHandler.sendMessage(Serial1, 4, 1);
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
      messageHandler.sendMessage(Serial1, 4, 0);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Kalibrierung");
      lcd.setCursor(0, 1);
      lcd.print("beendet");

    }
  };
}

void barrierDetected() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hindernis");
  lcd.setCursor(0, 1);  
  lcd.print("erkannt");  
}