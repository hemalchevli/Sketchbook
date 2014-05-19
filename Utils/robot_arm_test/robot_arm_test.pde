#include <Servo.h> 
 
Servo base;  //PIN 11
Servo shoulder;//PIN 10
Servo wrist;    //PIN 9
Servo toolrotate; //PIN 6
Servo toolgrip; //PIN 5
 
int basepos = 0;    // variable to store the servo position 
int shoulderpos = 0;    // variable to store the servo position 
int wristpos = 0;    // variable to store the servo position 
int toolrotatepos = 0;    // variable to store the servo position 
int toolgrippos = 0;    // variable to store the servo position 
int x;
int pos;
void setup() {
    
// initialize the LED pin as an output:
    base.attach(11);
    shoulder.attach(10);
    wrist.attach(9);
    toolrotate.attach(6);
    toolgrip.attach(5);
            
  // initialize serial communications:
  Serial.begin(9600);
  home1();
}

void loop() {
                
} 

// Movement of the arm reaching the object
void pickup (void){

  for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    base.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    base.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
}
//Movement of the the gripper
void grab (int a){
}

//Movement of the arm to the put object down
void drop (void){
}
//Movement of the gripper to open wide
void release (void){
}

// Default position
void home1 (void){
base.write(90);
shoulder.write(180);
wrist.write(0);
toolrotate.write(90);
toolgrip.write(0);
}
