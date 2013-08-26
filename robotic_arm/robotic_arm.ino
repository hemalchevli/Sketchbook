#include <Servo.h> 
 
Servo base;  // create servo object to control a servo 
Servo shoulder;//signal to be given to two servos
Servo elbow;
Servo wrist;
Servo toolrotate;
Servo toolgrip;// a maximum of eight servo objects can be created 
 
int basepos = 0;    // variable to store the servo position 
int shoulderpos = 0;    // variable to store the servo position 
int elbowpos = 0;    // variable to store the servo position 
int wristpos = 0;    // variable to store the servo position 
int toolrotatepos = 0;    // variable to store the servo position 
int toolgrippos = 0;    // variable to store the servo position 
int x;
void setup() {
    
// initialize the LED pin as an output:
    base.attach(9);
    shoulder.attach(9);
    elbow.attach(9);
    wrist.attach(9);
    toolrotate.attach(9);
    toolgrip.attach(9);
            
  // initialize serial communications:
  Serial.begin(9600);
}

void loop() {
  // read the value of the potentiometer:
 if (Serial.available()){ 
    x=Serial.read();
 }

  //if the analog value is high enough, turn on the LED:
  if (x == 0) {
    pickup ();
    grab(1);
    base.write(100);
    drop();
    release();
    home();
     } 
  else if (x==1){
    pickup ();
    grab(2);
    base.write(100);
    drop();
    release();
    home();
   
  }
 else if (x==2){
    pickup ();
    grab(3);
    base.write(100);
    drop();
    release();
    home();
   
  }
 else if (x==3){
    pickup ();
    grab(4);
    base.write(100);
    drop();
    release();
    home();
   
  }
  else{
    home();
  }

  delay(100);                    
} 
void pickup (void){}
void grab (int a){}
void drop (void){}
void release (void){}
void home1 (void){}
