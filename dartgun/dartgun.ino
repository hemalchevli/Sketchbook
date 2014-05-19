/*Dart gun project
 soft serial
 rx 
 tx
 
 Servo horizontal
 servo veritcal
 */
#include <Servo.h>


Servo Hservo;
Servo Vservo;
Servo shoot;

int pos = 0;

void setup(){
  Serial.begin(9600);

  Hservo.attach(10);
  Vservo.attach(9);
  shoot.attach(11);
  //set home
  Hservo.write(90);
  Vservo.write(90);
  shoot.write(0);

}
void loop(){
  static int v = 0;
  if(Serial.available()){
    char ch = Serial.read();

    switch(ch){
      //read number
    case '0'...'9':
      v=v*10 +ch -'0';
      if( v < 0){//to test
        v=0;
      }
      if( v >180){
        v=180;
      }
      Serial.println(v);
      break;

      //horizontal servo
    case 'h'://send ok with h and val
      Hservo.write(v);
      Serial.println("ok");
      v=0;
      break;

      //vertical servo 70 to 110
    case 'v'://send o with v and val
      Vservo.write(v);
      Serial.print("ok");
      v=0;
      break;

      //detach servos
    case 'd': //send detached
      Hservo.detach();
      Vservo.detach();
      shoot.detach();
      Serial.println("ok");
      break;
      //attach servos
    case 'a'://send attached
      Hservo.attach(10);
      Vservo.attach(9);
      shoot.attach(11);
      Serial.println("ok");
      break;

      //reset
    case'r':
      Hservo.attach(10);
      Vservo.attach(9);
      shoot.attach(11);
      
      Hservo.write(90);
      Vservo.write(90);
      shoot.write(0);
      Serial.println("RESET");
      break;
    case 's'://shoot to test
      shoot.write(170);
      delay(2000);
      shoot.write(0);
      delay(500);
      Serial.println("ok");
      
    case 'z':
      v = 0;
      Serial.println("ok");
      break;
      
    default:
   // Serial.println("ERR");
      //error
      break;
    }
  }

}


