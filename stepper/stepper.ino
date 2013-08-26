
/* 
 Stepper Motor Control - one revolution
 
 This program drives a unipolar or bipolar stepper motor. 
 The motor is attached to digital pins 8 - 11 of the Arduino.
 
 The motor should revolve one revolution in one direction, then
 one revolution in the other direction.  
 
  
 Created 11 Mar. 2007
 Modified 30 Nov. 2009
 by Tom Igoe
 
 */

#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
                                     // for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 17,16);            
char inChar;
void setup() {
  // set the speed at 60 rpm:
  myStepper.setSpeed(60);
  // initialize the serial port:
 Serial.begin(9600);
}
// myStepper.step(stepsPerRevolution);
//myStepper.setSpeed(motorSpeed);
void loop() {
  
  if(Serial.available()>0){
    inChar = Serial.read();
    if(inChar =='f'){
        myStepper.step(stepsPerRevolution);
        Serial.println("Forward");
    }
     else if (inChar == 'r'){
         myStepper.step(-stepsPerRevolution);
     }
  }
}

