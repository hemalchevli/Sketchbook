#include "Ultrasonic.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>
#define RESET false

LiquidCrystal lcd(A0,A1,A2,A3,A4,A5);
Ultrasonic ultrasonic(2,3);
int   distanceIN;
int distanceIN_raw;
int   distanceCM;

int addr= 0; //Address of the variable whre the variable will be stored
int val;

//flag = 0 => not played
//flag = 1 => played

int flag1 = 0;
int flag2 = 0;
int flag3 = 0;
int flag4 = 0;

int msg1 = 5;
int msg2 = 8;
int msg3 = 7;
int msg4 = 6;

int lowtime = 100;
int bytei,i;

const int numReadings = 5;
int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

int feet;
int inches;
int remainder_of_inches;
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Initializing...");
  delay(1000); //effects :p


  val =  EEPROM.read(addr);
  EEPROM.write(addr, val+1);

  pinMode(msg1,INPUT);//Hi-z
  pinMode(msg2,INPUT);
  pinMode(msg3,INPUT);
  pinMode(msg4,INPUT);
  /*
  //scroll msg
   lcd.setCursor(0,1);
   lcd.print("black-electronic");
   delay(1500);
   while (Line1[bytei+15] != '\0') {
   // scrolly scrolly 
   lcd.setCursor(0,1);
   for ( i= 0; i < 16; i++) {
   lcd.write(Line1[bytei + i]);
   }
   bytei++;
   delay(300);
   } 
   delay(1000);*/
  // initialize all the readings to 0: 
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0; 
}

void loop()
{
  distanceIN_raw = ultrasonic.Ranging(INC);
  //  distanceCM = ultrasonic.Ranging(CM);
  // subtract the last reading:
  total= total - readings[index];         
  // read from the sensor:  
  readings[index] = distanceIN_raw; 
  // add the reading to the total:
  total= total + readings[index];       
  // advance to the next position in the array:  
  index = index + 1;                    

  // if we're at the end of the array...
  if (index >= numReadings)              
    // ...wrap around to the beginning: 
    index = 0;                           

  // calculate the average:
  distanceIN = total / numReadings;         

  //convert to feet
  remainder_of_inches = distanceIN % 12;
  feet = (distanceIN - remainder_of_inches)/12;
  

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance:");
  lcd.print(feet);
  lcd.print("'");
  lcd.print(remainder_of_inches);
  lcd.print("\"");
  lcd.setCursor(0, 1);
  lcd.print("BlackElectronics");
  
  Serial.print(feet,DEC);
  Serial.print(" Feet, ");
  Serial.print(remainder_of_inches,DEC);
  Serial.print(" Inches");
  Serial.println("");

  if(distanceIN <= 24 && flag1 == 0){
    flag1=1; //played
    flag2=0;
    flag3=0;
    flag4=0;
    pinMode(msg1,OUTPUT);
    digitalWrite(msg1,LOW);
    delay(lowtime);
    pinMode(msg1,INPUT);
    Serial.println("Less than 2feet");
  }
  if(distanceIN > 24 && distanceIN <= 60 && flag2 == 0){
    flag1=0; //played
    flag2=1;
    flag3=0;
    flag4=0;
    pinMode(msg2,OUTPUT);
    digitalWrite(msg2,LOW);
    delay(lowtime);
    pinMode(msg2,INPUT);
    Serial.println("Between 2 and 5 feet");
  }
  if(distanceIN > 60 && distanceIN <= 120 && flag3 == 0){
    flag1=0; //played
    flag2=0;
    flag3=1;
    flag4=0;
    pinMode(msg3,OUTPUT);
    digitalWrite(msg3,LOW);
    delay(lowtime);
    pinMode(msg3,INPUT);
    Serial.println("Between 5 and 10");
  }
  if(distanceIN > 120 && distanceIN <= 156 && flag4 == 0){
    flag1=0; //played
    flag2=0;
    flag3=0;
    flag4=1;
    pinMode(msg4,OUTPUT);
    digitalWrite(msg4,LOW);
    delay(lowtime);
    pinMode(msg4,INPUT);
    Serial.println("Between 10 and 15");
  }
  if(distanceIN > 156){// none played
    flag1=0;
    flag2=0;
    flag3=0;
    flag4=0;
    lcd.setCursor(0, 0);
    lcd.print("Out of range    ");
    Serial.println("Between 5 and 10");
    delay(200);
  }

  delay(200);
}














