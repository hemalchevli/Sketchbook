/*Paint shooter project
 
 Servo horizontal
 servo verticle
 */
#include <Servo.h>

Servo Hservo;
Servo Vservo;

int colour1 = 11;
int colour2 = 12;
int colour3 = A0; 
int colour4 = 8 ;
int booted = 3;

static int v = 0;
 
void setup(){
  Serial.begin(9600);
  Hservo.attach(10);
  Vservo.attach(9);

  pinMode(colour1,OUTPUT);
  pinMode(colour2,OUTPUT);
  pinMode(colour3,OUTPUT);
  pinMode(colour4,OUTPUT);
  pinMode(booted,OUTPUT);

  digitalWrite(colour1,LOW);
  digitalWrite(colour2,LOW);
  digitalWrite(colour3,LOW);
  digitalWrite(colour4,LOW);
  digitalWrite(booted,LOW);

  //  Serial.println("Ready");
}
/*
command format 
 chars assigned
 z,v,h,a,d,r all numbers
 x Yellow
 c Green
 b Red
 n Blue
 
 z120v
 z90h
 z -> val = 0
 shoot colours
 colour 1 
 c->start wait 3 sec s->stop
 */
void loop(){
 
  if(Serial.available()){
    char ch = Serial.read();

    switch(ch){
      //read number
    case '0'...'9':
      v=v*10 +ch -'0';
      if( v < 0){
        v=0;
      }
      if( v >180){
        v=180;
      }
     // Serial.println(v);
      break;
    case 'x':
      delay(1000);
      digitalWrite(colour1,HIGH);

      digitalWrite(colour2,LOW);
      digitalWrite(colour3,LOW);
      digitalWrite(colour4,LOW);
    //  Serial.println("ok");
      break;

    case 'c':
      delay(1000);
      digitalWrite(colour2,HIGH);

      digitalWrite(colour1,LOW);
      digitalWrite(colour3,LOW);
      digitalWrite(colour4,LOW);
     // Serial.println("ok");
      break;
    case 'b':
      delay(1000);
      digitalWrite(colour3,HIGH);

      digitalWrite(colour1,LOW);
      digitalWrite(colour2,LOW);
      digitalWrite(colour4,LOW);
     // Serial.println("ok");
      break;
    case 'n':
      delay(1000);
      digitalWrite(colour4,HIGH);

      digitalWrite(colour1,LOW);
      digitalWrite(colour2,LOW);
      digitalWrite(colour3,LOW);
     // Serial.println("ok");
      break;


    case 's':
      delay(2000);
      digitalWrite(colour1,LOW);
      digitalWrite(colour2,LOW);
      digitalWrite(colour3,LOW);
      digitalWrite(colour4,LOW);
     // Serial.println("ok");
      break;
      //horizontal servo
    case 'h'://send ok with h and val
      Hservo.write(v);
     // Serial.println("ok");
      v=0;
      break;

      //vertical servo 70 to 110
    case 'v'://send o with v and val
      Vservo.write(v);
     // Serial.print("ok");
      v=0;
      break;

      //detach servos
    case 'd': //send detached
      Hservo.detach();
      Vservo.detach();
    //  Serial.println("ok");
      break;
      //attach servos
    case 'a'://send attached
      Hservo.attach(10);
      Vservo.attach(9);
     // Serial.println("ok");
      break;

      //reset
    case'r':
      Hservo.attach(10);
      Vservo.attach(9);
      Hservo.write(90);
      Vservo.write(90);
      digitalWrite(colour1,LOW);
      digitalWrite(colour2,LOW);
      digitalWrite(colour3,LOW);
      digitalWrite(colour4,LOW);
      digitalWrite(booted,LOW);
      Serial.println("RESET");
      v=0;
      break;
    case 'z':
      v = 0;
     // Serial.println("ok");
      break;

    default:
      //      Serial.println("ERR");
      break;
    }
  }

}




