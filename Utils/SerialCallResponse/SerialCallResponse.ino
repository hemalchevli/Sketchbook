//  Serial Call and Response
//  by Tom Igoe
//  Language: Wiring/Arduino
  
//  This program sends an ASCII A (byte of value 65) on startup
//  and repeats that until it gets some data in.
//  Then it waits for a byte in the serial port, and 
//  sends three sensor values whenever it gets a byte in.
  
//  Thanks to Greg Shakar for the improvements
  
//  Created 26 Sept. 2005
//  Updated 18 April 2008


int firstSensor = 0;    // first analog sensor
int secondSensor = 0;   // second analog sensor
int thirdSensor = 0;    // digital sensor
int inByte = 0;         // incoming serial byte

void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
  pinMode(2, INPUT);   // digital sensor is on digital pin 2
  establishContact();  // send a byte to establish contact until Processing responds 
}

void loop()
{
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    // read first analog input, divide by 4 to make the range 0-255:
    firstSensor = analogRead(0)/4;
    // delay 10ms to let the ADC recover:
    delay(10);
    // read second analog input, divide by 4 to make the range 0-255:
    secondSensor = analogRead(1)/4;
    // read  switch, multiply by 155 and add 100
    // so that you're sending 100 or 255:
    thirdSensor = 100 + (155 * digitalRead(2));
    // send sensor values:
    Serial.print(firstSensor);
    Serial.print(secondSensor);
    Serial.print(thirdSensor);               
  }
}

void establishContact() {
 while (Serial.available() <= 0) {
      Serial.print('C');   // send a capital C
      delay(300);
  }
}


