/*
harrdware pinouts
 0 serial rx
 1 serial tx
 2 soft serial rx
 3 soft serial tx
 4 buzzer and led
 5 panic button
 6 a/c motor
 7 relay 1 ignition and bonet
 8 relay 2 door unlock
 9 window motor 1
 10 window motor2
 11 window up/down
 12 usr in
 13 d7
 a0 d6
 a1 d5
 a2 d4
 a3 e
 a4 rs
 a5 lm35
 */
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>


char inchar; //Will hold the incoming character from the Serial Port.
//LM35 sensor variables
float tempC;

// Inputs and outputs

SoftwareSerial gsm(2,3); //Create a 'fake' serial port. D2 is the Rx pin, D3 is the Tx pin.
const int buzzer_led = 4;
const int panic = 5;
const int ac_motor = 6;
const int ignition_bonet = 7;
const int door_unlock = 8;
const int window_motor_1 = 9;
const int window_motor_2 = 10;
const int window_sw = 11;
const int usr_in = 12;
//lcd(rs,e,d4,d5,d6,d7);
LiquidCrystal lcd(A4,A3,A2,A1,A0,13);
const int tempPin =A5;

int panic_flag=0;
int usr_in_flag=0;
int window_status=1; // -1 noaction, 0 closed ,1 opened
int sms_sent=0; 
int window_button;
int panic_button;
int usr_in_button;

void setup(){
  Init();
  
  gsm.begin(9600);
 //    hw serial
  Serial.begin(9600);

  Serial.println("Waiting for gms to init");
  delay(5000); // give 5s time for GSM module to register on network etc.

  gsm.println("AT"); // Sync for baud rate
  delay(700);

  Serial.println("AT sent for sync");

  gsm.println("AT+CMGF=1"); // set SMS mode to text 
  delay(700);

  Serial.println("SMS mode set to text");

  gsm.println("AT+CNMI=1,2,0,0"); // set module to send SMS data to serial out upon receipt 
  delay(700);

  Serial.println("SMS to serial output");
  usr_in_flag=1;
}
void loop(){
  panic_button = digitalRead(panic);
  usr_in_button = digitalRead(usr_in);
  window_button = digitalRead(window_sw);

  if(panic_button == LOW){
    usr_in_flag = 0;
    panic_flag = 1;
    sms_sent=0;
    Serial.println("PANIC");
  }
  if(usr_in_button == LOW){
    panic_flag =0;
    usr_in_flag =1;
    Serial.println("OK");

  }

  if(gsm.available() >0)
  {
    inchar=gsm.read(); 
    Serial.print(inchar);
    //add code
    if (inchar=='*'){
      delay(10);
      panic_flag = 0;
      usr_in_flag =1;       
    }
  }

  if (panic_flag==1){
    panicState();
  }

  if (usr_in_flag == 1){
    relax();
     // contorol window open or close
  //write on lcd window open or close
  if (window_button == LOW){
   switch(window_status){
    case 0: 
      openWindow();
      window_status = 1;
      break;
    case 1:
     closeWindow();
     window_status = 0;
     break;
     
   } 
  }
    //contro windows
  }

}
void Init(){
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Initializing");

  pinMode(buzzer_led,OUTPUT); 
  pinMode(panic,INPUT); //active low
  pinMode(ac_motor,OUTPUT);
  pinMode(ignition_bonet,OUTPUT);
  pinMode(door_unlock,OUTPUT);
  pinMode(window_motor_1,OUTPUT);
  pinMode(window_motor_2,OUTPUT);
  pinMode(window_sw,INPUT); //active low
  pinMode(usr_in,INPUT);

  //turn off every thing

  digitalWrite(ac_motor,LOW);
  digitalWrite(ignition_bonet,LOW);
  digitalWrite(door_unlock,LOW);
  digitalWrite(window_motor_1,LOW);
  digitalWrite(window_motor_2,LOW);

}

void panicState(){ //when car in trouble
  blinkBuzzerLED();
  digitalWrite(door_unlock,HIGH);
  digitalWrite(ignition_bonet,HIGH);
  digitalWrite(ac_motor,LOW);

  //Display on LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Unautrorized");
  lcd.setCursor(0,2);
  lcd.print("Entry");
  //window close
  if (window_status == 1){// if stattus  open
      //close
    closeWindow();
    window_status = 0; //closed
    digitalWrite(window_motor_1,HIGH);
    digitalWrite(window_motor_2,HIGH);

  }

  //send SMS
  if(sms_sent == 0){ //sms not sent
    gsm.print("AT+CMGS=");
    gsm.write((byte)34);// ASCII equivalent of "
    gsm.print("8866379683");
    gsm.write((byte)34); // ASCII equivalent of "
    gsm.println();
    delay(300);
    //  gsm.println("Unauthorized Entry in car\x1A");
    gsm.print("Unauthorized Entry in car");
    gsm.println();
    delay(50);
    gsm.write((byte)26); // ASCII equivalent of Ctrl-Z
    sms_sent = 1;
  }
  panic_flag=-1;

}

void relax(){ //when user inside car
  digitalWrite(buzzer_led,LOW);
  digitalWrite(door_unlock,LOW);
  digitalWrite(ignition_bonet,LOW);
  ReadTemp();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp.");
  lcd.setCursor(0,2);
  lcd.print(tempC);
  //add ac motor pwm based on temp
  
  if(tempC >=35){//hot
    analogWrite(ac_motor,255);
  }
  else if (tempC <=30){//cold
    analogWrite(ac_motor,150);
  }
  else if(tempC>30 && tempC<35) {//medium
    analogWrite(ac_motor,200);
  }
 
}

void closeWindow(){
  digitalWrite(window_motor_1,HIGH);
  digitalWrite(window_motor_2,LOW);
  delay(300);
  digitalWrite(window_motor_1,HIGH);
  digitalWrite(window_motor_2,HIGH);
}

void openWindow(){
  digitalWrite(window_motor_1,LOW);
  digitalWrite(window_motor_2,HIGH);
  delay(300);
  digitalWrite(window_motor_1,HIGH);
  digitalWrite(window_motor_2,HIGH);
}

void blinkBuzzerLED(){
  digitalWrite(buzzer_led,!digitalRead(buzzer_led)); //Read the LED state and NOT (!) it, write it back to the LED. This toggles
  delay(500);

}

void ReadTemp(){
  float  tempC_raw = analogRead(tempPin);           //read the value from the sensor
  tempC = (5.0 * tempC_raw * 100.0)/1024.0;  //convert the analog data to temperature
  //Serial.println(tempC);             //send the data to the computer
 // Serial.println(tempC);
  delay(200);                           //wait one second before sending new data
}





