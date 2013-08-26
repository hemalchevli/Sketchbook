/*
An open-source binary clock for Arduino. 
http://www.instructables.com/id/LED-Binary-Clock-1/
http://www.instructables.com/member/emihackr97/
*/



#define AA 8
#define AB 4
#define AC 9
#define AD 15
#define BA 7
#define BB 3
#define BC 10
#define BD 14
#define CA 6
#define CB 2
#define CC 11
#define DA 5
#define DC 12
#define MOSFET 3
#define LDR 19
#define MinButton 19
#define HourButton 18
#define ModeSwitch 17

static unsigned long lastTick = 0; // (static variables are initialized once and keep their values between function calls)
static int second=0, minute=0, haur=0; //start the time on 00:00:00
static int hour = 0;
static int munit = 0;
static int hunit = 0;
static int valm=0;
static int valh=0;
static int ledstats = 0;
static int i= 0;
static int brightness = 255;
static int light = 0;
static int switchState = 0;

void setup() { //set outputs and inputs
pinMode(AA, OUTPUT);pinMode(AB, OUTPUT);pinMode(AC, OUTPUT);pinMode(AD, OUTPUT);pinMode(BA, OUTPUT);
pinMode(BB, OUTPUT);pinMode(BC, OUTPUT);pinMode(BD, OUTPUT);pinMode(CA, OUTPUT);pinMode(CB, OUTPUT);
pinMode(CC, OUTPUT);pinMode(DA, OUTPUT);pinMode(DC, OUTPUT);

pinMode(MOSFET, OUTPUT);
pinMode(MinButton, INPUT);
pinMode(HourButton, INPUT);
pinMode(LDR, INPUT);
pinMode(ModeSwitch, INPUT);

digitalWrite(ModeSwitch, HIGH);
digitalWrite(MinButton, HIGH);
digitalWrite(HourButton, HIGH);
digitalWrite(LDR, HIGH);
}


void loop() {

 KeepTime();
 PrintTime();
 CheckButtons();
 CheckLight();
 CheckSwitch();

}

void KeepTime(){
  
  // move forward one second every 1000 milliseconds
  if (millis() - lastTick >= 1000) { 
  lastTick = millis();
  second++;

}

// move forward one minute every 60 seconds
  if (second >= 60) {
  minute++;
  second = 0; // reset seconds to zero
}

// move forward one hour every 60 minutes
if (minute >=60) {
  haur++;
  minute = 0; // reset minutes to zero
}

if (haur >=24) {
  haur=0;
  minute = 0; // reset minutes to zero
}

}
 
void PrintTime(){
  
   munit = minute%10; //sets the variable munit and hunit for the unit digits
  hunit = hour%10;

  //minutes units
  if(munit == 1 || munit == 3 || munit == 5 || munit == 7 || munit == 9) {  digitalWrite(AA, LOW);} else {  digitalWrite(AA, HIGH);}
  if(munit == 2 || munit == 3 || munit == 6 || munit == 7) {digitalWrite(BA, LOW);} else {digitalWrite(BA,HIGH);}
  if(munit == 4 || munit == 5 || munit == 6 || munit == 7) {digitalWrite(CA, LOW);} else {digitalWrite(CA,HIGH);}
  if(munit == 8 || munit == 9) {digitalWrite(DA, LOW);} else {digitalWrite(DA,HIGH);}

  //minutes 
  if((minute >= 10 && minute < 20) || (minute >= 30 && minute < 40) || (minute >= 50 && minute < 60))  {digitalWrite(AB, LOW);} else {digitalWrite(AB,HIGH);}
  if(minute >= 20 && minute < 40)  {digitalWrite(BB, LOW);} else {digitalWrite(BB,HIGH);}
  if(minute >= 40 && minute < 60) {digitalWrite(CB, LOW);} else {digitalWrite(CB,HIGH);}

  //hour units
  if(hunit == 1 || hunit == 3 || hunit == 5 || hunit == 7 || hunit == 9) {digitalWrite(AC, LOW);} else {digitalWrite(AC,HIGH);}
  if(hunit == 2 || hunit == 3 || hunit == 6 || hunit == 7) {digitalWrite(BC, LOW);} else {digitalWrite(BC,HIGH);}
  if(hunit == 4 || hunit == 5 || hunit == 6 || hunit == 7) {digitalWrite(CC, LOW);} else {digitalWrite(CC,HIGH);}
  if(hunit == 8 || hunit == 9) {digitalWrite(DC, LOW);} else {digitalWrite(DC,HIGH);}

  //hour
  if(hour >= 10 && hour < 20)  {digitalWrite(AD, LOW);} else {digitalWrite(AD,HIGH);}
  if(hour >= 20 && hour < 24)  {digitalWrite(BD, LOW);} else {digitalWrite(BD,HIGH);}
}

void CheckButtons(){
  
  valm = digitalRead(MinButton);    // add one minute when pressed
   if(valm== LOW) {
   minute++;
   second=0;
   delay(250);
  }
  
  valh = digitalRead(HourButton);    // add one hour when pressed
   if(valh==LOW) {
   haur++;
   second=0;
   delay(250);
  }
}



void CheckLight(){
  light = analogRead(LDR);
  light=constrain(light, 0, 900);
  brightness= map(light, 0, 900, 0, 254);
 // brightness=255;
 analogWrite(MOSFET, brightness);
}

void CheckSwitch(){
 
  switchState=digitalRead(ModeSwitch);
  
  if(switchState==0){
    hour=haur;
    if(haur>=12){
      hour=haur-12;
    }
  }
  if(switchState==1){
    hour=haur;
  }

}

