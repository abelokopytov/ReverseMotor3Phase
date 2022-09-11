/*
Copyright 2022 abelokopytov at gmail
This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published
by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.
*/

/*
ReverseMotor3Phaze with Timer
*/

// https://github.com/marcelloromani/arduino/tree/master/SimpleTimer
#include <SimpleTimer.h>

int pinPower = 4;
int pinMoveUp = 11;
bool boolMoveUp = true;
//Miniature snap-action switches
int pinSwitchUp = A0;
int pinSwitchDown = A1;
bool boolSwitchUp = false;
bool boolSwitchDown = false;
// boolMoveUp=true: moving from SwitchUp to SwitchDown
// if boolSwitchUp  ==true than change boolMoveUp to true 
// if boolSwitchDown==true than change boolMoveUp to false (ie move down)

SimpleTimer timer;
int timerId;
unsigned long timerInterval;
bool boolPowerOn = false;

//work scenario: power on for intervalOn, power off for intervalOff
int intervalOn = 2; // in minutes
int intervalOff = 30; // in minutes

void checkSwitches() {
  //check switches
  //the pcb schematics is such that when switch is closed
  //then 3.5V is supplied to arduino's analog input
  boolSwitchUp = (analogRead(pinSwitchUp) > 500);
  boolSwitchDown = (analogRead(pinSwitchDown) > 500);
}

void setup() {                
  Serial.begin(115200);
  pinMode(pinPower, OUTPUT);
  pinMode(pinMoveUp, OUTPUT);
  digitalWrite(pinPower, LOW); //unnecessary?
  delay(100);
  checkSwitches();
  if (boolSwitchUp && boolSwitchDown) {
    stopMoving();
  } else {
    //change direction as needed
    if (boolSwitchUp) boolMoveUp = true;
    if (boolSwitchDown) boolMoveUp = false;
    if (boolMoveUp) {
      digitalWrite(pinMoveUp, HIGH);
      Serial.println("Setup: MoveUp");
    } else {
      digitalWrite(pinMoveUp, LOW);
      Serial.println("Setup: MoveDown");
    }
    delay(100);
    digitalWrite(pinPower, HIGH);
    Serial.println("Setup: Power On");
    boolPowerOn = true;
    delay(100);
  }
  timerInterval = intervalOn;
  timerId = timer.setInterval(timerInterval*60*1000, changeOnOff);
}

void changeOnOff() {
  int currentPowerOn = boolPowerOn;
  if (currentPowerOn) {
    digitalWrite(pinPower, LOW);
    Serial.println("Power off");
    boolPowerOn = false;
    timerInterval = intervalOff;
  } else {  
    digitalWrite(pinPower, HIGH);
    Serial.println("Power on");
    boolPowerOn = true;
    timerInterval = intervalOn;
  }
  timer.deleteTimer(timerId);
  timerId = timer.setInterval(timerInterval*60*1000, changeOnOff);  
}

void changeDirection() {
   Serial.print("changeDirection: ");
   digitalWrite(pinPower, LOW);
   delay(2000); //wait for motor to stop
   boolMoveUp = !boolMoveUp;
   if (boolMoveUp) {
     Serial.println("MoveUp");
     digitalWrite(pinMoveUp, HIGH);
   } else {
     Serial.println("MoveDown");
     digitalWrite(pinMoveUp, LOW);
   }
   delay(100);
   digitalWrite(pinPower, HIGH);
}
void stopMoving() {
   digitalWrite(pinPower, LOW);
   while (true) {
     Serial.println("stopMoving");
     digitalWrite(LED_BUILTIN, HIGH);
     delay(200);
     digitalWrite(LED_BUILTIN, LOW);
     delay(200);
   }
}
// the loop routine runs over and over again forever:
void loop() {
   timer.run();
   if (boolPowerOn) {
     checkSwitches();
     if (boolSwitchUp && boolSwitchDown) {
       stopMoving();
     } else {
       if (boolSwitchUp && !boolMoveUp) changeDirection();
       if (boolSwitchDown &&  boolMoveUp) changeDirection();
     }
   }
   delay(100);
}
