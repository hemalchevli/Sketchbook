/*
Square Wave Generator
*/

/*
Copyright (c) 2013 Hemal M. Chevli

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
/*
Potentiometer 1 connected to A4 for varying frequency at pin D8
Potentiometer 2 connected to A5 for varying duty cycle of PWM wave generated on pin D9
*/
#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
                                     // for your motor
Stepper DriveStepper(stepsPerRevolution, 13,8);   
const int stepper_pot = A4;
int drive_speed; //speed of stepper in rpm

int i=1;
int ledPin = 7;      // LED connected to digital pin 7 to indicate program has entered the loop

void setup() {
  pinMode(stepper_pot,INPUT);
  pinMode(9,OUTPUT); //PWM pin f=500Hz
  pinMode(ledPin,OUTPUT); //LED for ON indication
  digitalWrite(ledPin,LOW); //Current is sinked from LED

}

void loop() {
 analogWrite(9, (analogRead(A5))/4);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
// tone(8,map(analogRead(A4), 0, 1023, 5000, 100)); //Generate frequency 

drive_speed = analogRead(stepper_pot);
// map it to a range from 0 to 100:
drive_speed = map(drive_speed, 0, 1023, 0, 100);
DriveStepper.setSpeed(drive_speed);
DriveStepper.step(stepsPerRevolution/100);
}


