#include <SPI.h>  // Not actualy used but needed to compile

//including needed subclasses
#include "steering.h"
#include "driving.h"
#include "distance.h"
#include "MessageHandler.h"

MessageHandler messageHandler;
HardwareSerial& serialCom = Serial3; //defining variable for serial port 3 (faster changes while debugging)

int speedVal = 200;  //this will be the speed variable, which can be controled by remote device TEMPORARY

//values of joystick
int joyVert = 0;
int joyHorz = 0;

//defining of different subclasses
Steering steering;
Driving driving;
Distance distance;

void setup() {
  //initalizing subclasses, to set pinModes or Pins
  steering.init();
  driving.init();
  distance.init(50, 51);

  //initializing serial ports for data transfer and debug output
  Serial.begin(115200);
  serialCom.begin(9600);
}

void loop() {
  messageHandler.pollMessage(serialCom);
  if (messageHandler.isMessageAvailable()) {
  
    
    Serial.print("Message available"); //debug output

    char id;
    const char* data = messageHandler.getMessage(&id);
    Serial.println(id);
    // handle message
    switch (id) { //if message id is equal to one of the following values do...
      case '1': {
          Serial.println(data); //debug output
          sscanf(data, "%d;%d;%d", &joyHorz, &joyVert, &speedVal);
          //driving process of car; with current position of joystick and speed of engines
          distance.stopIf(7); //stops engines if distance tracer gets data lower than 7
          driving.handleDriving(joyVert, 200, distance); //drives with speed of 200 if joystick is moved in right direction
          if (steering.isCalibrated() == true) {    //steering only possible if steering was calibrated before
            steering.handleSteering(joyHorz);
          }
          break;
        }
      case '2': {
          Serial.println("Calibration Starting");
          steering.startCalibration();  //starting calibration
          messageHandler.sendMessage(serialCom , 'a', " "); //sending backsignal to controldevice
          break;
        }
      case '4': {
          tone(24, 200, 1000); //plays the horn
          break;
        }
    }
    
  }
}