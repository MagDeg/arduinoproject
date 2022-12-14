#include <Arduino.h>
#include "driving.h"
#include "distance.h"


void Driving::forward(int speed) {
  //setting pins to drive foreward with secific speed
  analogWrite(ENGINE_LEFT, speed);
  digitalWrite(BACKWARD_LEFT, LOW);
  digitalWrite(FORWARD_LEFT, HIGH);

  analogWrite(ENGINE_RIGHT, speed);
  digitalWrite(BACKWARD_RIGHT, LOW);
  digitalWrite(FORWARD_RIGHT, HIGH);
}

void Driving::backward(int speed) {
  //driving backward with specific speed
  analogWrite(ENGINE_LEFT, speed);
  digitalWrite(BACKWARD_LEFT, HIGH);
  digitalWrite(FORWARD_LEFT, LOW);

  analogWrite(ENGINE_RIGHT, speed);
  digitalWrite(BACKWARD_RIGHT, HIGH);
  digitalWrite(FORWARD_RIGHT, LOW);
}

void Driving::stop(){
  //stops driving 
  digitalWrite(BACKWARD_LEFT, LOW);
  digitalWrite(FORWARD_LEFT, LOW);
  digitalWrite(BACKWARD_RIGHT, LOW);
  digitalWrite(FORWARD_RIGHT, LOW);  
}


void Driving::init() {
  //setting pinModes of engine pins, with for loop so make code smaller
  for (int i = 0; i <= 5; i++) {
    pinMode(enginePins[i], OUTPUT);
  }
}

void Driving::handleDriving(int joystickVert, int speed, Distance distance){
  Serial.println(distance.barrier());
  if (joystickVert > 600 && (distance.barrier() == false)) {
    forward(speed);
  } else if (joystickVert < 400) {
    backward(speed);
  } else {
    stop();    
  }      
}
