//#define TOMILLIGAUSS 1953L  // For A1301: 2.5mV = 1Gauss, and 1024 analog steps = 5V, so 1 step = 1953mG
#define TOMILLIGAUSS 3756L  // For A1302: 1.3mV = 1Gauss, and 1024 analog steps = 5V, so 1 step = 3756mG
#define NOFIELD 505L    // Analog output with no applied field, calibrate this
#define VERBOSE true // display status and information 
#define DATALOG !(VERBOSE) //To log data to kst, IMP VERBOSE must be false if DATALOG is true

byte serialByte;

//Sensor pin defs
const int sensorPinx = 12;    // pin that the sensor is attached to 23 C0
const int sensorPin1 = A5;    // pin that the sensor is attached to 23 C0
const int sensorPin2 = A4;    // pin that the sensor is attached to 24 C1
const int sensorPin3 = A3;    // pin that the sensor is attached to 25 C2
const int sensorPin4 = A2;    // pin that the sensor is attached to 26 C3

const int tolerance_switch = A4; 		// pin that the sensor is attached to 27 C4

//LED pin defs variables
const int ledPin1T = 2;        // pin that the LED is attached to 4 D2
const int ledPin1L = 3;        // pin that the LED is attached to 5 D3

const int ledPin2T = 4;       // pin that the LED is attached to 6 D4
const int ledPin2L = 5;        // pin that the LED is attached to 11 D5

const int ledPin3T = 6;        // pin that the LED is attached to 12 D6
const int ledPin3L = 7;        // pin that the LED is attached to 13 D7

const int ledPin4T = 8;        // pin that the LED is attached to 14 B0
const int ledPin4L = 9;        // pin that the LED is attached to 15 B1


//Buzzers and switches
const int Buzzer = 10;			//attached to pin 16 via transistor 
const int switchs = 11;		//attached to pin 17


//variables:

long compensated1,compensated2,compensated,gauss;
long i=0;
long sensorValue = 0;         // the sensor value
long sensorMin = 1023;        // minimum sensor value
long sensorMax = 0;           // maximum sensor value

long sensorMin1 = 1023;        // minimum sensor value
long sensorMax1 = 0;

long sensorMin2 = 1023;        // minimum sensor value
long sensorMax2 = 0;           // maximum sensor value

long sensorMin3 = 1023;        // minimum sensor value
long sensorMax3 = 0;           // maximum sensor value

long sensorMin4 = 1023;        // minimum sensor value
long sensorMax4 = 0;           // maximum sensor value

//
int LED1,LED2 = 0;

int LED_delay;
int LED_Blink_Frequency;

float max_tolerance = 10,raw;

int tmp=0;
float emin,emax;
/********************SETUP START***************************/
void setup(){

	if (VERBOSE) {
    Serial.begin(115200);
    Serial.println("Initializing...");
  }
	//Set LEDS as output
	pinMode(ledPin1T, OUTPUT);
	pinMode(ledPin1L, OUTPUT);
	pinMode(ledPin2T, OUTPUT);
	pinMode(ledPin2L, OUTPUT);
	pinMode(ledPin3T, OUTPUT);
	pinMode(ledPin3L, OUTPUT);
	pinMode(ledPin4T, OUTPUT);
	pinMode(ledPin4L, OUTPUT);

	digitalWrite(ledPin1T, HIGH);
	digitalWrite(ledPin1L, HIGH);
	digitalWrite(ledPin2T, HIGH);
	digitalWrite(ledPin2L, HIGH);
	digitalWrite(ledPin3T, HIGH);
	digitalWrite(ledPin3L, HIGH);
	digitalWrite(ledPin4T, HIGH);
	digitalWrite(ledPin4L, HIGH);

	//Sensors as input
	pinMode(sensorPin1, INPUT);
	pinMode(sensorPin2, INPUT);
	pinMode(sensorPin3, INPUT);
	pinMode(sensorPin4, INPUT);

	//Buzzer output and Calibrate Switch input
	pinMode(Buzzer, OUTPUT);
	//pinMode(CalibrateSwitch, INPUT);
	 if (VERBOSE) Serial.println("IO setup complete.");
	//loop 5 times with delay of 250 ms
	 if (VERBOSE) Serial.println("Starting LED test...");
	TestLED(1,1000);

	 if (VERBOSE) Serial.println("Starting Magnet Strength test.");
	
	//check the strength of magnet
	Test_Magnet(sensorPin1);
	Test_Magnet(sensorPin2);
	Test_Magnet(sensorPin3);
	Test_Magnet(sensorPin4);

	tmp=0;
	if (VERBOSE) Serial.println("Magnet Strength Test Complete.");
	// turn on LED to signal the start of the calibration period:
	// calibrate during the first five seconds 
	if (VERBOSE) Serial.println("Begining Calibration");
	Calibrate(sensorPin1);
	Calibrate(sensorPin2);
	Calibrate(sensorPin3);
	Calibrate(sensorPin4);
	
	if (VERBOSE) Serial.println("Calibration complete.");
	LedOff();
	
	if (VERBOSE) Serial.println("Entring Loop.");
}
/********************SETUP END***************************/

/********************LOOP START***************************/
void loop()
{

  Serial.println(" ");
	//DoMeasurement(sensorPin1);
	//DoMeasurement(sensorPin2);
	//DoMeasurement(sensorPin3);
	//DoMeasurement(sensorPin4);
/***********Check Sensor1**********************/
//Sensor 1
	raw = analogRead(sensorPin1);   // Range : 0..1023

	//  Uncomment this to get a raw reading for calibration of no-field point
	//  Serial.print("Raw reading: ");
	//  Serial.println(raw);

	compensated = raw - NOFIELD;                 // adjust relative to no applied field
	gauss = compensated * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

	//find error
	//%emin % emax
	//%emin % emax
	//error = ((measured-actual)/actual)*100
	emin = ((gauss - sensorMin1)/sensorMin1)*100;
	emax = ((gauss - sensorMax1)/sensorMax1)*100;
	
	
	//convert % to positive values
	if(emin < 0) emin = -emin;
	if(emax < 0) emax = -emax;
	if (VERBOSE){
		Serial.print("1-Emin ");
		Serial.print(emin);
		Serial.print(" 1-Emax ");
		Serial.println(emax);
	}

	//if %emin>2 
	if (emin > max_tolerance){
		digitalWrite(ledPin1T, LOW);
		BuzzerOn();
		//tight, led on, ring buzzer
	}	
	if (emax > max_tolerance){
		digitalWrite(ledPin1L, LOW);
		BuzzerOn();
		//loose, led on, ring buzzer
	}
	else{
		digitalWrite(ledPin1L,HIGH);
		digitalWrite(ledPin1T,HIGH);
        BuzzerOff();
		//led 0ff and buzzer off
	}
/************Sensor 1 end*********************/

/***********Check Sensor2**********************/
//Sensor 2
	raw = analogRead(sensorPin2);   // Range : 0..1023

	//  Uncomment this to get a raw reading for calibration of no-field point
	//  Serial.print("Raw reading: ");
	//  Serial.println(raw);

	compensated = raw - NOFIELD;                 // adjust relative to no applied field
	gauss = compensated * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

	//find error
	//%emin % emax
	//%emin % emax
	//error = ((measured-actual)/actual)*100
	emin = ((gauss - sensorMin2)/sensorMin2)*100;
	emax = ((gauss - sensorMax2)/sensorMax2)*100;
	
	
	//convert % to positive values
	if(emin < 0) emin = -emin;
	if(emax < 0) emax = -emax;
	
	 if (VERBOSE){
		Serial.print("2-Emin ");
		Serial.print(emin);
		Serial.print(" 2-Emax ");
		Serial.println(emax);
	}

	//if %emin>2 
	if (emin > max_tolerance){
		digitalWrite(ledPin2T, LOW);
		BuzzerOn();
		//tight, led on, ring buzzer
	}	
	if (emax > max_tolerance){
		digitalWrite(ledPin2L, LOW);
		BuzzerOn();
		//loose, led on, ring buzzer
	}
	else{
		digitalWrite(ledPin2L,HIGH);
		digitalWrite(ledPin2T,HIGH);
        BuzzerOff();
		//led 0ff and buzzer off
	}

/************Sensor 2 end*********************/


/***********Check Sensor3**********************/
//Sensor 3
	raw = analogRead(sensorPin3);   // Range : 0..1023

	//  Uncomment this to get a raw reading for calibration of no-field point
	//  Serial.print("Raw reading: ");
	//  Serial.println(raw);

	compensated = raw - NOFIELD;                 // adjust relative to no applied field
	gauss = compensated * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

	//find error
	//%emin % emax
	//%emin % emax
	//error = ((measured-actual)/actual)*100
	 emin = ((gauss - sensorMin3)/sensorMin3)*100;
	 emax = ((gauss - sensorMax3)/sensorMax3)*100;
	
	
	//convert % to positive values
	if(emin < 0) emin = -emin;
	if(emax < 0) emax = -emax;

	 if (VERBOSE){
		Serial.print("3-Emin ");
		Serial.print(emin);
		Serial.print(" 3-Emax ");
		Serial.println(emax);
	}
	
	//if %emin>2 
	if (emin > max_tolerance){
		digitalWrite(ledPin3T, LOW);
		BuzzerOn();
		//tight, led on, ring buzzer
	}	
	if (emax > max_tolerance){
		digitalWrite(ledPin3L, LOW);
		BuzzerOn();
		//loose, led on, ring buzzer
	}
	else{
		
		digitalWrite(ledPin3L,HIGH);
		digitalWrite(ledPin3T,HIGH);
		BuzzerOff();	
		//led 0ff and buzzer off
	}
/************Sensor 3 end*********************/


/***********Check Sensor 4**********************/
//Sensor 4
	raw = analogRead(sensorPin4);   // Range : 0..1023

	//  Uncomment this to get a raw reading for calibration of no-field point
	//  Serial.print("Raw reading: ");
	//  Serial.println(raw);

	compensated = raw - NOFIELD;                 // adjust relative to no applied field
	gauss = compensated * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

	//find error
	//%emin % emax
	//%emin % emax
	//error = ((measured-actual)/actual)*100
	 emin = ((gauss - sensorMin4)/sensorMin4)*100;
	 emax = ((gauss - sensorMax4)/sensorMax4)*100;
	
	
	//convert % to positive values
	if(emin < 0) emin = -emin;
	if(emax < 0) emax = -emax;
	
	 if (VERBOSE){
		Serial.print("4-Emin ");
		Serial.print(emin);
		Serial.print(" 4-Emax ");
		Serial.println(emax);
	}
	//if %emin>2 
	if (emin > max_tolerance){
		digitalWrite(ledPin4T, LOW);
		BuzzerOn();
		//tight, led on, ring buzzer
	}	
	if (emax > max_tolerance){
		digitalWrite(ledPin4L, LOW);
		BuzzerOn();
		//loose, led on, ring buzzer
	}
	else{
		digitalWrite(ledPin4L,HIGH);
		digitalWrite(ledPin4T,HIGH);
		BuzzerOff();
		//led 0ff and buzzer off
	}
/************Sensor 4 end*********************/

delay(1000);
}
/********************LOOP END***************************/


/********************Prototypes***************************/
void LogData(){
float fanalog0,fanalog1;
byte serialByte;
float j = 0;
int raw[4];
long gauss[4],compensated[4];
 raw[0] = analogRead(5);   // Range : 0..1024
 raw[1] = analogRead(4);   // Range : 0..1024
 raw[2] = analogRead(3);   // Range : 0..1024
 raw[3] = analogRead(2);   // Range : 0..1024

//  Uncomment this to get a raw reading for calibration of no-field point
//  Serial.print("Raw reading: ");
//  Serial.println(raw);

  compensated[0] = raw[0] - NOFIELD;                 // adjust relative to no applied field
  gauss[0] = compensated[0] * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

  compensated[1] = raw[1] - NOFIELD;                 // adjust relative to no applied field
  gauss[1] = compensated[1] * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

  compensated[2] = raw[2] - NOFIELD;                 // adjust relative to no applied field
  gauss[2] = compensated[2] * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

  compensated[3] = raw[3] - NOFIELD;                 // adjust relative to no applied field
  gauss[3] = compensated[3] * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

 
    Serial.print(j);
    Serial.print(',',BYTE);
    Serial.print(gauss[0]);
    Serial.print(',',BYTE);
    Serial.print(gauss[1]);
    Serial.print(',',BYTE);
    Serial.print(gauss[2]);
    Serial.print(',',BYTE);
    Serial.println(gauss[3]);
    
    j++;
    //Serial.print(';',BYTE);
    //Serial.println(fanalog1,DEC);
  
}

//ring buzzer and blink led 
void Error_Occured(void){
//ring buzzer on off 

	if (VERBOSE)Serial.println("Error Occured");
	for(i=0;i<5;i++){
		//Buzzers on
		digitalWrite(Buzzer, HIGH);
		//LEDS on
		digitalWrite(LED1, LOW);
		digitalWrite(LED2, LOW);

		delay(400);
		//Buzzers off
		digitalWrite(Buzzer, LOW);
		//LEDS off
		digitalWrite(LED1, HIGH);
		digitalWrite(LED2, HIGH);
	}


}

/**********************************************************/

int tolerance_value(void){
	//get raw value from A4
	tmp = analogRead(tolerance_switch);
		
	/*POS   value
	 * 1	25
	 * 2	128
	 * 3	176
	 * 4	232
	 * 5	252
	 * 6	254
	 * 7	250
	 * 8	255
	 */
	if(tmp < 30){
		if (VERBOSE)Serial.println("Tolerance : 1%");
		 return 1;
	}
	if(tmp >30 && tmp < 130){
		if (VERBOSE)Serial.println("Tolerance : 2%");
		return 2;
	}
	
	if(tmp > 130 && tmp < 180){
		if (VERBOSE)Serial.println("Tolerance : 3%");
		return 3;
	}
	
	if(tmp > 180 && tmp < 235){
		if (VERBOSE)Serial.println("Tolerance : 4%");
		return 4;
	}

	if(tmp > 235 && tmp < 253){
		if (VERBOSE)Serial.println("Tolerance : 5%");
		return 5;
	}
	
	if(tmp > 253) {
		if (VERBOSE)Serial.println("Tolerance : 5%");
		return 5;
	}

	
}

void Test_Magnet(int SensorNo){
	//test the strength of the magnetic field
	// measure magnetic field
	raw = analogRead(SensorNo);   // Range : 0..1023

	//  Uncomment this to get a raw reading for calibration of no-field point
	//  Serial.print("Raw reading: ");
	//  Serial.println(raw);

	compensated = raw - NOFIELD;                 // adjust relative to no applied field
	gauss = compensated * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

	//gauss>0 north pole
	//gauss<0 south pole
	if (SensorNo == sensorPin1) {
		LED1 = ledPin1T;
		LED2 = ledPin1L;
	}
	else if (SensorNo == sensorPin2){
		LED1 = ledPin2T;
		LED2 = ledPin2L;
	}
	else if(SensorNo == sensorPin3){
		LED1 = ledPin3T;
		LED2 = ledPin3L;
	}
	else if(SensorNo == sensorPin4){
		LED1 = ledPin4T;
		LED2 = ledPin4L;
	}
//Check Gauss values

	if(gauss > -100 && gauss < 100){
		//fail replace magnet
		//stop program and blink led of that sensor continuously
		//ring buzzer
		while(1){
			Error_Occured();
		}
	}
	//Add else to indicate
	//all systems go...
	//indicate with some pattern think later
	
	
	
	 if (VERBOSE)Serial.println("Magnet OK");

}


/**********************************************************/
void DoMeasurement(int SensorNo)
{
	// measure magnetic field
	int raw = analogRead(SensorNo);   // Range : 0..1023

	//  Uncomment this to get a raw reading for calibration of no-field point
	//  Serial.print("Raw reading: ");
	//  Serial.println(raw);

	long compensated = raw - NOFIELD;                 // adjust relative to no applied field
	long gauss = compensated * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
/************************
	//find error
	//%emin % emax
	//error = ((measured-actual)/actual)*100
	int emin = ((gauss - sensorMin1)/sensorMin1)*100;
	int emax = ((gauss - sensorMax1)/sensorMax1)*100;
	
	//convert % to positive values
	if <0 %e= -%e
	
	//if %emin>2 
		tight, led on, ring buzzer
		
	if %emax >2 
		loose, led on, ring buzzer

	else
		led 0ff and buzzer off
***************/
	Serial.print(gauss);
	Serial.print(" Gauss ");

	if (gauss > 0){
		 if (VERBOSE) Serial.println("(South pole)");
	}
	else if(gauss < 0){
		 if (VERBOSE) Serial.println("(North pole)");
	}
	else{
		 if (VERBOSE) Serial.println();
	}
}

/**********************************************************/
//Raw Calibration
void Calibrate(int SensorNo){
	//Which LED to blink
	if (SensorNo == sensorPin1) {
		LED1 = ledPin1T;
		LED2 = ledPin1L;
	}
	else if (SensorNo == sensorPin2){
		LED1 = ledPin2T;
		LED2 = ledPin2L;
	}
	else if(SensorNo == sensorPin3){
		LED1 = ledPin3T;
		LED2 = ledPin3L;
	}
	else if(SensorNo == sensorPin4){
		LED1 = ledPin4T;
		LED2 = ledPin4L;
	}
	//Calibration start

	LedOff();
	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);


	Serial.println("Entering calibration loop");

	unsigned long start = millis();
	while (millis() - start <= 5000) {       
		sensorValue = analogRead(SensorNo);
		//other method
		//max1 = max(analogIn(pin), max1); 
		// record the maximum sensor value
		if (sensorValue > sensorMax) {
			sensorMax = sensorValue;
		}
		// record the minimum sensor value
		if (sensorValue < sensorMin) {
			sensorMin = sensorValue;
		}
	}
    //LED off, end of calibration
	digitalWrite(LED1, HIGH);
	digitalWrite(LED2, HIGH);


	//Store the values of min and max
	if (SensorNo == sensorPin1) {
		compensated1 = sensorMin - NOFIELD;                 // adjust relative to no applied field
		compensated2 = sensorMax - NOFIELD;                 // adjust relative to no applied field

		sensorMin1 = compensated1 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
		sensorMax1 = compensated2 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
		if (VERBOSE) Serial.println(sensorMin1);
		if (VERBOSE) Serial.println(sensorMax1);
	}
	else if (SensorNo == sensorPin2){
		compensated1 = sensorMin - NOFIELD;                 // adjust relative to no applied field
		compensated2 = sensorMax - NOFIELD;                 // adjust relative to no applied field

		sensorMin2 = compensated1 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
		sensorMax2 = compensated2 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
                if (VERBOSE) Serial.println(sensorMin2);
                if (VERBOSE) Serial.println(sensorMax2);
	}
	else if(SensorNo == sensorPin3){
		compensated1 = sensorMin - NOFIELD;                 // adjust relative to no applied field
		compensated2 = sensorMax - NOFIELD;                 // adjust relative to no applied field

		sensorMin3 = compensated1 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
		sensorMax3 = compensated2 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss;
                if (VERBOSE) Serial.println(sensorMin3);
                if (VERBOSE) Serial.println(sensorMax3);
	}
	else if(SensorNo == sensorPin4){
		compensated1 = sensorMin - NOFIELD;                 // adjust relative to no applied field
		compensated2 = sensorMax - NOFIELD;                 // adjust relative to no applied field

		sensorMin4 = compensated1 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
		sensorMax4 = compensated2 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
                if (VERBOSE) Serial.println(sensorMin4);
                if (VERBOSE) Serial.println(sensorMax4);
	}
	// if (VERBOSE) Serial.println(sensorMin);
	// if (VERBOSE) Serial.println(sensorMax);
	sensorMin = 1023;
	sensorMax = 0;
}


/**********************************************************/
//Test if all LEDs are Working
void TestLED(int times,int Delay){
	 if (VERBOSE) Serial.println("LED test Start.");
	for(i=0;i<times;i++){
		  digitalWrite(ledPin1T, LOW);  delay(Delay);  digitalWrite(ledPin1T, HIGH);
		  digitalWrite(ledPin1L, LOW);  delay(Delay);  digitalWrite(ledPin1L, HIGH);
		  digitalWrite(ledPin2T, LOW);  delay(Delay);  digitalWrite(ledPin2T, HIGH);
		  digitalWrite(ledPin2L, LOW);  delay(Delay);  digitalWrite(ledPin2L, HIGH);
		  digitalWrite(ledPin3T, LOW);  delay(Delay);  digitalWrite(ledPin3T, HIGH);
		  digitalWrite(ledPin3L, LOW);  delay(Delay);  digitalWrite(ledPin3L, HIGH);
		  digitalWrite(ledPin4T, LOW);  delay(Delay);  digitalWrite(ledPin4T, HIGH);
		  digitalWrite(ledPin4L, LOW);  delay(Delay);  digitalWrite(ledPin4L, HIGH);
	}
	 if (VERBOSE) Serial.println("LED test Successful.");
}

/**********************************************************/
//To turn off all the LEDS
void LedOff(void){
	  digitalWrite(ledPin1T, HIGH);
	  digitalWrite(ledPin1L, HIGH);
	  digitalWrite(ledPin2T, HIGH);
	  digitalWrite(ledPin2L, HIGH);
	  digitalWrite(ledPin3T, HIGH);
	  digitalWrite(ledPin3L, HIGH);
	  digitalWrite(ledPin4T, HIGH);
	  digitalWrite(ledPin4L, HIGH);
}

void BuzzerOff(void){
  //if (VERBOSE) Serial.println("Buzzer off");
  digitalWrite(Buzzer, LOW);
  
}
void BuzzerOn(void){
	//if (VERBOSE) Serial.println("Buzzer on");
	for(i=0;i<5;i++){
		//Buzzers on
		digitalWrite(Buzzer, HIGH);
		delay(400);
		//Buzzers off
		digitalWrite(Buzzer, LOW);
	}
}
