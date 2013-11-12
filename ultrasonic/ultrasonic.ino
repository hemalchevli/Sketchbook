#include "Ultrasonic.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>
#define RESET false

LiquidCrystal lcd(A0,A1,A2,A3,A4,A5);
Ultrasonic ultrasonic(2,3);
int   distanceIN;
int   distanceCM;

int addr= 0; //Address of the variable whre the variable will be stored
int val;
char Line1[] = "black-electronics.com/blog";

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
}

void loop()
{
  distanceIN = ultrasonic.Ranging(INC);
  distanceCM = ultrasonic.Ranging(CM);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(distanceIN);
  lcd.print("in");
  lcd.setCursor(0, 1);
  lcd.print("BlackElectronics");
  Serial.print(distanceIN);
  Serial.print("in");
  Serial.print("    ");
  Serial.print(distanceCM);
  Serial.print("cm");
  Serial.print("    ");
  //print total run values value
  Serial.println(val);

  if(distanceIN <= 6 && flag1 == 0){
    flag1=1; //played
    flag2=0;
    flag3=0;
    flag4=0;
    pinMode(msg1,OUTPUT);
    digitalWrite(msg1,LOW);
    delay(lowtime);
    pinMode(msg1,INPUT);
    Serial.println("Less than 6");
  }
  if(distanceIN > 6 && distanceIN <= 12 && flag2 == 0){
    flag1=0; //played
    flag2=1;
    flag3=0;
    flag4=0;
    pinMode(msg2,OUTPUT);
    digitalWrite(msg2,LOW);
    delay(lowtime);
    pinMode(msg2,INPUT);
    Serial.println("Between 6 and 12");
  }
  if(distanceIN > 12 && distanceIN <= 18 && flag3 == 0){
    flag1=0; //played
    flag2=0;
    flag3=1;
    flag4=0;
    pinMode(msg3,OUTPUT);
    digitalWrite(msg3,LOW);
    delay(lowtime);
    pinMode(msg3,INPUT);
    Serial.println("Between 12 and 18");
  }
  if(distanceIN > 18 && distanceIN <= 24 && flag4 == 0){
    flag1=0; //played
    flag2=0;
    flag3=0;
    flag4=1;
    pinMode(msg4,OUTPUT);
    digitalWrite(msg4,LOW);
    delay(lowtime);
    pinMode(msg4,INPUT);
    Serial.println("Between 18 and 24");
  }
  if(distanceIN > 24){
    flag1=0;
    flag2=0;
    flag3=0;
    flag4=0;
  }

  delay(500);
}













