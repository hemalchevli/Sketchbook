/* Motorized Camera Dolly
Controls three DC motors and the switch changes the direction and the pot changes the speed
*/
#define LED A1
#define SWITCH A3
#define POT A5

//Motor 1
const int M1P=3;
const int M1N=5;
//Motor 2
const int M2P=6;
const int M2N=9;
//Motor3
const int M3P=10;
const int M3N=11;
int speed;
int direction;
int pot;

void setup(){
  Serial.begin(9600);
  pinMode(M1P,OUTPUT);
  pinMode(M1N,OUTPUT);

  pinMode(M2P,OUTPUT);
  pinMode(M2N,OUTPUT);

  pinMode(M3P,OUTPUT);
  pinMode(M3N,OUTPUT);

  pinMode(SWITCH,INPUT);
  pinMode(LED,OUTPUT);  //Indicate execution of sketch
  digitalWrite(LED,HIGH); 
}

void loop(){
  pot = analogRead(A5);
  direction = digitalRead(SWITCH); //High->CW , LOW->CCW
  speed = map(pot,0,940,0,255); //940 because of 1k resistor in series with pot
  
  if (speed >=255)speed=255; //safeguard

  Serial.print("Direction:");
  Serial.print(direction);
  Serial.print(" Speed:");
  Serial.println(speed);
  if (direction == HIGH){
    //Motor 1
    //analogWrite(M1P,speed);
    digitalWrite(M1P,HIGH);
    digitalWrite(M1N,LOW);

    //Motor2
//    analogWrite(M1P,speed);
    digitalWrite(M2P,HIGH);
    digitalWrite(M2N,LOW);

    //Motor 3
//    analogWrite(M1P,speed);
    digitalWrite(M3P,HIGH);
    digitalWrite(M3N,LOW);
    //turn motor cw
   // set speed
    //Motor 1 3,5
    
    //Motor 2 6,9
    //Motor 3 10,11
    
  }
  else{
  //  turn motor ccw 
   // set speed
    //Motor 1
    //analogWrite(M1P,speed);
    digitalWrite(M1P,LOW);
    digitalWrite(M1N,HIGH);

    //Motor2
//    analogWrite(M1P,speed);
    digitalWrite(M2P,LOW);
    digitalWrite(M2N,HIGH);

    //Motor 3
//    analogWrite(M1P,speed);
    digitalWrite(M3P,LOW);
    digitalWrite(M3N,HIGH);
  }
}
