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
  // read the value of the potentiometer:
 if (Serial.available()){ 
    x=Serial.read();
 }

  //if the analog value is high enough, turn on the LED:
  if (x == '1') {
    pickup ();
    grab(0);
    base.write(100);
    drop();
    release();
    home1();
    x=0;
     } 
  else if (x=='2'){
    pickup ();
    grab(1);
    base.write(100);
    drop();
    release();
    home1();
    x=0;
   
  }
 else if (x=='3'){
    pickup ();
    grab(2);
    base.write(100);
    drop();
    release();
    home1();
    x=0;
   
  }
 else if (x=='4'){
    pickup ();
    grab(3);
    base.write(100);
    drop();
    release();
    home1();
    x=0;
   
  }
  delay(100);                    
} 

// Movement of the arm reaching the object
void pickup (void){
  Serial.println("Pick Up!");
  shoulder.write(180);
  delay(15);  
  wrist.write(0);
  delay(15);  
  toolrotate.write(90);
  delay(15);    

} 
 
//Movement of the the gripper
void grab (int a){
    Serial.println("Grabbing");
  if(a==0){
    toolgrip.write(10);
  }
  else if(a==1){
   toolgrip.write(20);
  }
  else if(a==2){
   toolgrip.write(30);
  }
  else if (a==3){
   toolgrip.write(40);
  }

}


//Movement of the arm to the put object down
void drop (void){
  Serial.println("The Dropsss");
  shoulder.write(180);
  delay(15);  
  wrist.write(0);
  delay(15);  
  toolrotate.write(90);
  delay(15);    
}
//Movement of the gripper to open wide
void release (void){
  Serial.println("Object realeased");
  toolgrip.write(0);
}

// Default position
void home1 (void){
Serial.println("@home");
base.write(90);
shoulder.write(180);
wrist.write(0);
toolrotate.write(90);
toolgrip.write(0);
}
