/*
Motor on D8
RED LED D9
GREEN LED D10
BLUE LED D10
LDR A4
LM35 A5
*/

int LDR_Pin = A3

; //analog pin 4
int LM35_Pin = A5; //analog pin 5
int Motor = 8;
int Red = 9; 
int Green = 10;
int Blue = 11;
int cc_reset = 7;
char light;

void setup(){
	Serial.begin(38400);
	
//Setting pin modes
	pinMode(Motor,OUTPUT);
	pinMode(Red,OUTPUT);
	pinMode(Green,OUTPUT);
	pinMode(Blue,OUTPUT);
        pinMode(cc_reset,OUTPUT);	
	//Pulling all output low
	digitalWrite(Motor,LOW);
	digitalWrite(Red,LOW);
	digitalWrite(Green,LOW);
	digitalWrite(Blue,LOW);

        //reset the cc2500 module so that it can send and receive the addresses
	digitalWrite(cc_reset,LOW);
        delay(250);
	digitalWrite(cc_reset,HIGH);
        delay(250);
      
	Serial.print("2");	//Self Address
	Serial.print("1");	//Remote Address
	Serial.print("3");	//Channel
	
	
}
void loop(){
	int LDRReading = analogRead(LDR_Pin); 
	int temp = analogRead(LM35_Pin); 
	temp = (5.0 * temp * 100.0)/1024.0;  //convert the analog data to temperature

	if(temp >=35){//hot
		//glow red led
		//run dc motor
		//send temp to serial
		digitalWrite(Motor,HIGH);
		digitalWrite(Red,HIGH);
		digitalWrite(Green,LOW);
		digitalWrite(Blue,LOW);
	}
	else if (temp <=30){//cold
		//glow blue led
		//send to serial
		
		digitalWrite(Motor,LOW);
		digitalWrite(Red,LOW);
		digitalWrite(Green,LOW);
		digitalWrite(Blue,HIGH);
	}
	else if(temp>30 && temp<35) {//medium
		//glow greeled 
		
		digitalWrite(Motor,LOW);
		digitalWrite(Red,LOW);
		digitalWrite(Green,HIGH);
		digitalWrite(Blue,LOW);
	}
	
	//if else for ldr low meduin and high intensity of light
	//low <100 high>800 , meduim >100 and <800
	if (LDRReading >500){
		//write high intensity
		light='H';
	}
	else if(LDRReading<100){
		light='L';
	}
	else if (LDRReading >=100 && LDRReading <=500){
		light='M';
	}
        Serial.println(temp);
       
	delay(1000); //just here to slow down the output for easier reading
}


