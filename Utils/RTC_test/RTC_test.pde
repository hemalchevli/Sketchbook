//Arduino 1.0+ Only
//Arduino 1.0+ Only

#include "Wire.h"
#define DS1307_ADDRESS 0x68

void setup(){
  Wire.begin();
  Serial.begin(9600);
}

void loop(){
  printDate();
  delay(1000);
}

byte bcdToDec(byte val)  {
// Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}

void printDate(){

  // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);

  byte zero = 0x00;
  Wire.send(zero);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);

  int second = bcdToDec(Wire.receive());
  int minute = bcdToDec(Wire.receive());
  int hour = bcdToDec(Wire.receive() & 0b111111); //24 hour time
  int weekDay = bcdToDec(Wire.receive()); //0-6 -> sunday - Saturday
  int monthDay = bcdToDec(Wire.receive());
  int month = bcdToDec(Wire.receive());
  int year = bcdToDec(Wire.receive());

  //print the date EG   3/1/11 23:59:59
  Serial.print(month);
  Serial.print("/");
  Serial.print(monthDay);
  Serial.print("/");
  Serial.print(year);
  Serial.print(" ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);

}
