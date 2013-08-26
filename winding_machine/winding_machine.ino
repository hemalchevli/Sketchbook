#include <Stepper.h>
#include <LiquidCrystal.h>

#define VERBOSE true

//LiquidCrystal lcd(RS,E,d4,d5,d6,d7);
LiquidCrystal lcd(A1, A0, 13,12,11,10);

int stepsPerRevolution =200;
Stepper DriveStepper(stepsPerRevolution, 7,8);   
Stepper SlideStepper(stepsPerRevolution, 5,6);   

const int dc_pot = A5;
const int stepper_pot = A4;
const int dc_motor = 9;
const int left_limit = 2;
int left;	//stor the starus of the switch
const int right_limit = 3;
int right;
const int thread = 4;
int thread_cut;


int drive_stepCount = 0;  // number of steps the motor has taken
int slide_stepCount = 0;  // number of steps the motor has taken
int dc_speed;      //speed of dc motor in rpm
int drive_speed; //speed of stepper in rpm
int slide_speed; //speed of stepper in rpm
int warning = 0; //Warning Flag

int i=1;
void setup()
{
	
  if(VERBOSE){ Serial.begin(9600);}
  lcd.begin(16,2);
  
  pinMode(dc_pot,INPUT);
  pinMode(stepper_pot,INPUT);
  pinMode(dc_motor,OUTPUT);
  lcd.clear();
  /*
 read profile setting
 load profile
 display on lcd
 
 on lcd display:
 RPM:60__M:100
 PRO:PA,DC RPM:12
 */
}

void loop() 
{
//  if thread is not cut
	thread_cut=digitalRead(thread);
	if(thread_cut == HIGH){
	//lcd.clear();
    set_dc_speed(); 
    set_drive_speed();
		//count algo
		//calculate dist 
		//display
	left=digitalRead(left_limit);
	if (left==HIGH){ // left limit reached reverse the motor
		i=1;
		if(VERBOSE){Serial.print(" L ");}
	}
	right=digitalRead(right_limit);
	if(right ==HIGH){
		i=-1;
		if(VERBOSE){Serial.println(" R ");}
	}
	slide_speed =drive_speed-20;
	slide_speed = max(slide_speed, 1);
	slide_speed = min(slide_speed, 100);
	SlideStepper.setSpeed(slide_speed);
	if(VERBOSE){
		Serial.print(" SL ");
		Serial.println(slide_speed);
		}
	SlideStepper.step(i*stepsPerRevolution/100);

    warning = 0; //thread ok, will present warning if thread is cut
    
  }//thread not cut
  else{
    //turn off all motors
    digitalWrite(dc_motor,LOW);
    SlideStepper.setSpeed(0);
    DriveStepper.setSpeed(0);
    digitalWrite(5,LOW); //all stepper lines low
    digitalWrite(6,LOW);
    digitalWrite(7,LOW);
    digitalWrite(8,LOW);
    //write warning on lcd 
    if (warning==0){
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Thread Cut!!");
		if(VERBOSE ){Serial.println("CUT");}
		warning = 1;
		delay(5000);
		lcd.clear();
	}
  }//thread cut
} //loop end

void set_dc_speed(){
   //read the sensor value:
  dc_speed = analogRead(dc_pot);
  dc_speed = dc_speed/4;
  dc_speed = max(dc_speed, 1);
  dc_speed = min(dc_speed, 255);
  // map it to a range from 0 to 100:
  //dc_speed = map(dc_speed, 0, 1023, 0, 255);
//  dc_speed = max(dc_speed, 90);
//  dc_speed = min(dc_speed, 255);
	if(VERBOSE){
		Serial.print("dc ");
		Serial.print(dc_speed);
		}
	analogWrite(dc_motor,dc_speed);

}

void set_drive_speed(){
  // read the sensor value:
	drive_speed = analogRead(stepper_pot);
	// map it to a range from 0 to 100:
	drive_speed = map(drive_speed, 0, 1023, 0, 100);

	// set the motor speed:
	if (drive_speed > 0) {
    DriveStepper.setSpeed(drive_speed);
    if(VERBOSE){
		Serial.print(" Dri ");
		Serial.print(drive_speed);
	}
    // step 1/100 of a revolution:
	DriveStepper.step(stepsPerRevolution/100);
 }
}
