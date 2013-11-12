#include <SoftwareSerial.h>

//Variables
char inchar; //Will hold the incoming character from the Serial Port.

char number[12]={'9','1','0','0','0','0','0','0','0','0','0','0'};// switching will happen only if msg is from this number. '91' is the country code

char temp[12]={}; //init all elements to 0 , to store the incoming number 

int ok=0; //number ok or not 
int do_switching=0; //switching format detected

char switch_status[11]={};
int i=0; //counter for 'for' loop

SoftwareSerial cell(2,3); //Create a 'fake' serial port. Pin 3 is the Rx pin, pin 4 is the Tx pin.

//r1d1  room 1 device 1
//room and devices defs
const int r1d1 = 4;
const int r1d2 = 5;
const int r1d3 = 6;
const int r2d1 = 7;
const int r2d2 = 8;
const int r2d3 = 9;
const int r3d1 = 10;
const int r3d2 = 11;
const int r3d3 = 12;

char inchar_sw;//tmp variable

void setup()
{
  for(i=4;i<=12;i++){
    pinMode(i,OUTPUT);
  }
  for(i=4;i<=12;i++){
    digitalWrite(i,LOW);
  }
  Serial.begin(9600);

  //Initialize GSM module serial port for communication.
  cell.begin(9600);

  Serial.println("Wait 5 seconds for gsm to initialize");
  delay(5000); // give 5s to 30s time for GSM module to register on network etc.

  cell.println("AT"); // Sync for baud rate
  delay(700);

  Serial.println("AT sent for sync purpose");

  cell.println("AT+CMGF=1"); // set SMS mode to text 
  delay(700);

  Serial.println("SMS mode set to text");

  cell.println("AT+CNMI=1,2,0,0"); // set module to send SMS data to serial out upon receipt 
  delay(700);

  Serial.println("SMS to serial output");
}

void loop() 
{
  //If a character comes in from the cellular module...
  if(cell.available() > 0)
  {
    inchar=cell.read(); 
    Serial.write(inchar);
     
     // number verification
	if (inchar =='"'){
	  delay(10);
	  inchar=cell.read(); 
	  delay(10);
	  if(inchar == '+'){//after the '+' sign the number will start, store next 12 chars in array for comparision
		for(i=0;i<=12;i++){
		  inchar = cell.read();
		  delay(10);
		  temp[i] = inchar;
		}
		ok=compare(number,temp,12);
		Serial.println("Number:");
		Serial.println(" ");

		for (i = 0; i <= 12; i++) {
		  Serial.print(temp[i]);
		}

		Serial.println(" ");
		Serial.println("ok: ");
		Serial.print(ok);
		Serial.println(" ");
	  }
	}
                
    //if number ok that is its verified
    if (ok==1){
       //msg formate 0#000#000#000 
	//to be sent from mobile phone
	//0# is to detect the start of message
    // use 0# to locate message
    //rest is 000#000#000 
	//r1d1 r1d2 r1d3# r2d1 r2d2 r2d3 # r3d1 r3d2 r3d3
     if (inchar=='0'){

        inchar = cell.read();
        Serial.print(inchar);
         delay(10);
       if (inchar=='#'){
	//read 000#000#000
          delay(10);
          //message for switching detected
          for(i=0;i<=11;i++){
              inchar = cell.read();
              delay(10);
	      switch_status[i] = inchar;
          }
          do_switching=1;
        }
      }
    }//ok end

  }//cell.available end

  if(do_switching == 1){ //message formate ok now parse msg 000#000#000
      doSwitching();
      do_switching = 0; //switch flag
   }//do switch end
   
  
}//loop end

//compare arrays      
int compare(char a1 [], char a2 [], int n)
{
  int i, result;
  for (i=0; i<n; ++i)
    if (a1[i] != a2[i]) result = 0;
    else result = 1;

  return (result);
}

void doSwitching(){
//  inchar=cell.read();
Serial.println("Switch array is:");
 for(i=0;i<=11;i++){
           
            Serial.println(switch_status[i]);
            delay(10);
   }
   Serial.println("");
   Serial.println("Swtich actions: ");
   Serial.println("");
   if(switch_status[0] == '1') {digitalWrite(r1d1,HIGH);Serial.println("11 on");} else {digitalWrite(r1d1,LOW);Serial.println("11 off");}
   if(switch_status[1] == '1') {digitalWrite(r1d2,HIGH);Serial.println("12 on");} else {digitalWrite(r1d2,LOW);Serial.println("12 off");}
   if(switch_status[2] == '1') {digitalWrite(r1d3,HIGH);Serial.println("13 on");} else {digitalWrite(r1d3,LOW);Serial.println("13 off");}
   if(switch_status[4] == '1') {digitalWrite(r2d1,HIGH);Serial.println("21 on");} else {digitalWrite(r2d1,LOW);Serial.println("21 off");}
   if(switch_status[5] == '1') {digitalWrite(r2d2,HIGH);Serial.println("22 on");} else {digitalWrite(r2d2,LOW);Serial.println("22 off");}
   if(switch_status[6] == '1') {digitalWrite(r2d3,HIGH);Serial.println("23 on");} else {digitalWrite(r2d3,LOW);Serial.println("23 off");}
   if(switch_status[8] == '1') {digitalWrite(r3d1,HIGH);Serial.println("31 on");} else {digitalWrite(r3d1,LOW);Serial.println("31 off");}
   if(switch_status[9] == '1') {digitalWrite(r3d2,HIGH);Serial.println("32 on");} else {digitalWrite(r3d2,LOW);Serial.println("32 off");}
  if(switch_status[10] == '1') {digitalWrite(r3d3,HIGH);Serial.println("33 on");} else {digitalWrite(r3d3,LOW);Serial.println("33 off");}

}


