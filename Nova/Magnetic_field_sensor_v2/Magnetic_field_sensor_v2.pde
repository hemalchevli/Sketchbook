//#define TOMILLIGAUSS 1953L  // For A1301: 2.5mV = 1Gauss, and 1024 analog steps = 5V, so 1 step = 1953mG
#define TOMILLIGAUSS 3756L  // For A1302: 1.3mV = 1Gauss, and 1024 analog steps = 5V, so 1 step = 3756mG
#define NOFIELD 505L    // Analog output with no applied field, calibrate this
#define VERBOSE true // display status and maze information 
#define DATALOG !(VERBOSE) //To log data to kst, IMP VERBOSE must be false if DATALOG is true

byte serialByte;
//Sensor pin defs

const int sensor1 = A5;    // pin that the sensor is attached to 23 C0
const int sensor2 = A4;    // pin that the sensor is attached to 24 C1
const int sensor3 = A3;    // pin that the sensor is attached to 25 C2
const int sensor4 = A2;    // pin that the sensor is attached to 26 C3

const int tolerance_switch = A1; 		// pin that the sensor is attached to 27 C4

//LED pin defs variables
const int L1T = 2;        // pin that the LED is attached to 4 D2
const int L1L = 3;        // pin that the LED is attached to 5 D3

const int L2T = 4;        // pin that the LED is attached to 6 D4
const int L2L = 5;        // pin that the LED is attached to 11 D5

const int L3T = 6;        // pin that the LED is attached to 12 D6
const int L3L = 7;        // pin that the LED is attached to 13 D7

const int L4T = 8;        // pin that the LED is attached to 14 B0
const int L4L = 9;        // pin that the LED is attached to 15 B1

//Buzzers and switches
const int Buzzer = 10;			//attached to pin 16 via transistor 
const int CalibrateSwitch = 11;// pin that the LED is attached to 16 B2

//variables:

long compensated1,compensated2,compensated,gauss;
int i=0,rawmin,rawmax;
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

int max_tolerance=10,raw;

int tmp=0;
float emin,emax;
/********************SETUP START***************************/
void setup(){
	
	if (VERBOSE) {
		Serial.begin(115200);
		Serial.println("Initializing...");
	}

	setIO();

	 if (VERBOSE) Serial.println("IO setup complete.");
	//loop 5 times with delay of 250 ms
	 if (VERBOSE) Serial.println("Starting LED test...");
	TestLED(1,1000);

	 if (VERBOSE) Serial.println("Starting Magnet Strength test.");
	
	//Calibrate the value of gauss for no magnet present
	Test_Magnet(sensor1);
	Test_Magnet(sensor2);
	Test_Magnet(sensor3);
	Test_Magnet(sensor4);
	
	tmp=0;
	 if (VERBOSE) Serial.println("Magnet Strength Test Complete.");
	// turn on LED to signal the start of the calibration period:
	// calibrate during the first five seconds 
	 if (VERBOSE) Serial.println("Begining Calibration");
	Calibrate(sensor1);
	Calibrate(sensor2);
	Calibrate(sensor3);
	Calibrate(sensor4);
	
	 if (VERBOSE) Serial.println("Calibration complete.");
	LedOff();
	
	 if (VERBOSE) Serial.println("Entring Loop.");
}
/********************SETUP END***************************/

/********************LOOP START***************************/
void loop()
{
	//DoMeasurement(sensor1);
	//DoMeasurement(sensor2);
	//DoMeasurement(sensor3);
	//DoMeasurement(sensor4);
/***********Check Sensor1**********************/
//Sensor 1
	raw = analogRead(sensor1);   // Range : 0..1023

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
	
	//if %emin>2 
	if (emin > max_tolerance){
		digitalWrite(L1T, HIGH);
		BuzzerOn();
		//tight, led on, ring buzzer
	}	
	if (emax > max_tolerance){
		digitalWrite(L1L, HIGH);
		BuzzerOn();
		//loose, led on, ring buzzer
	}
	else{
		
		digitalWrite(L1L,LOW);
		digitalWrite(L1T,LOW);

		//led 0ff and buzzer off
	}
	
	emin = 0;
	emax = 0;
	raw = 0;
/************Sensor 1 end*********************/

/***********Check Sensor2**********************/
//Sensor 2
	raw = analogRead(sensor2);   // Range : 0..1023

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
	
	//if %emin>2 
	if (emin > max_tolerance){
		digitalWrite(L2T, HIGH);
		BuzzerOn();
		//tight, led on, ring buzzer
	}	
	if (emax > max_tolerance){
		digitalWrite(L2L, HIGH);
		BuzzerOn();
		//loose, led on, ring buzzer
	}
	else{
		
		digitalWrite(L2L,LOW);
		digitalWrite(L2T,LOW);

		//led 0ff and buzzer off
	}

/************Sensor 2 end*********************/


/***********Check Sensor3**********************/
//Sensor 3
	raw = analogRead(sensor3);   // Range : 0..1023

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
	
	//if %emin>2 
	if (emin > max_tolerance){
		digitalWrite(L3T, HIGH);
		BuzzerOn();
		//tight, led on, ring buzzer
	}	
	if (emax > max_tolerance){
		digitalWrite(L3L, HIGH);
		BuzzerOn();
		//loose, led on, ring buzzer
	}
	else{
		
		digitalWrite(L3L,LOW);
		digitalWrite(L3T,LOW);

		//led 0ff and buzzer off
	}
/************Sensor 3 end*********************/


/***********Check Sensor 4**********************/
//Sensor 4
	raw = analogRead(sensor4);   // Range : 0..1023

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
	
	//if %emin>2 
	if (emin > max_tolerance){
		digitalWrite(L4T, HIGH);
		BuzzerOn();
		//tight, led on, ring buzzer
	}	
	if (emax > max_tolerance){
		digitalWrite(L4L, HIGH);
		BuzzerOn();
		//loose, led on, ring buzzer
	}
	else{
		
		digitalWrite(L4L,LOW);
		digitalWrite(L4T,LOW);

		//led 0ff and buzzer off
	}
/************Sensor 4 end*********************/
}
/********************LOOP END***************************/


/********************Prototypes***************************/

void setIO (void){
	//Set LEDS as output
	pinMode(L1T, OUTPUT);
	pinMode(L1L, OUTPUT);
	pinMode(L2T, OUTPUT);
	pinMode(L2L, OUTPUT);
	pinMode(L3T, OUTPUT);
	pinMode(L3L, OUTPUT);
	pinMode(L4T, OUTPUT);
	pinMode(L4L, OUTPUT);

	//Sensors as input
	pinMode(sensor1, INPUT);
	pinMode(sensor2, INPUT);
	pinMode(sensor3, INPUT);
	pinMode(sensor4, INPUT);

	//Buzzer output and Calibrate Switch input
	pinMode(Buzzer, OUTPUT);
	pinMode(CalibrateSwitch, INPUT);
}
//ring buzzer and blink led 
void Error_Occured(void){
//ring buzzer on off 

	if (VERBOSE)Serial.println("Error Occured");
	for(i=0;i<5;i++){
		//Buzzers on
		digitalWrite(Buzzer, HIGH);
		//LEDS on
		digitalWrite(LED1, HIGH);
		digitalWrite(LED2, HIGH);

		delay(400);
		//Buzzers off
		digitalWrite(Buzzer, LOW);
		//LEDS off
		digitalWrite(LED1, LOW);
		digitalWrite(LED2, LOW);
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
	if (SensorNo == sensor1) {
		LED1 = L1T;
		LED2 = L1L;
	}
	else if (SensorNo == sensor2){
		LED1 = L2T;
		LED2 = L2L;
	}
	else if(SensorNo == sensor3){
		LED1 = L3T;
		LED2 = L3L;
	}
	else if(SensorNo == sensor4){
		LED1 = L4T;
		LED2 = L4L;
	}
//Check Gauss values
if(gauss < 100 && gauss >-100)
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
	if (SensorNo == sensor1) {
		LED1 = L1T;
		LED2 = L1L;
	}
	else if (SensorNo == sensor2){
		LED1 = L2T;
		LED2 = L2L;
	}
	else if(SensorNo == sensor3){
		LED1 = L3T;
		LED2 = L3L;
	}
	else if(SensorNo == sensor4){
		LED1 = L4T;
		LED2 = L4L;
	}
	//Calibration start

	LedOff();
	digitalWrite(LED1, HIGH);
	digitalWrite(LED2, HIGH);


	Serial.println("Entering calibration loop");

	unsigned long start = millis();
	while (millis() - start <= 5000) {       
	while (millis() < 5000) {
		sensorValue = analogRead(SensorNo);

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
	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);


	//Store the values of min and max
	if (SensorNo == sensor1) {
		rawmin = sensorMin;
		rawmax = sensorMax;

		compensated1 = rawmin - NOFIELD;                 // adjust relative to no applied field
		compensated2 = rawmax - NOFIELD;                 // adjust relative to no applied field

		sensorMin1 = compensated1 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
		sensorMax1 = compensated2 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
	}
	else if (SensorNo == sensor2){
		rawmin = sensorMin;
		rawmax = sensorMax;

		compensated1 = rawmin - NOFIELD;                 // adjust relative to no applied field
		compensated2 = rawmax - NOFIELD;                 // adjust relative to no applied field

		sensorMin2 = compensated1 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
		sensorMax2 = compensated2 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
	}
	else if(SensorNo == sensor3){
		rawmin = sensorMin;
		rawmax = sensorMax;

		compensated1 = rawmin - NOFIELD;                 // adjust relative to no applied field
		compensated2 = rawmax - NOFIELD;                 // adjust relative to no applied field

		sensorMin3 = compensated1 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
		sensorMax3 = compensated2 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss;
	}
	else if(SensorNo == sensor4){
		rawmin = sensorMin;
		rawmax = sensorMax;

		compensated1 = rawmin - NOFIELD;                 // adjust relative to no applied field
		compensated2 = rawmax - NOFIELD;                 // adjust relative to no applied field

		sensorMin4 = compensated1 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
		sensorMax4 = compensated2 * TOMILLIGAUSS / 1000;   // adjust scale to Gauss
	}
	 if (VERBOSE) Serial.println(sensorMin);
	 if (VERBOSE) Serial.println(sensorMax);
}


/**********************************************************/
//Test if all LEDs are Working
void TestLED(int times,int Delay){
	 if (VERBOSE) Serial.println("LED test Start.");
	for(i=0;i<times;i++){
		digitalWrite(L1T, HIGH);  delay(Delay);  digitalWrite(L1T, LOW);
		digitalWrite(L1L, HIGH);  delay(Delay);  digitalWrite(L1L, LOW);
		digitalWrite(L2T, HIGH);  delay(Delay);  digitalWrite(L2T, LOW);
		digitalWrite(L2L, HIGH);  delay(Delay);  digitalWrite(L2L, LOW);
		digitalWrite(L3T, HIGH);  delay(Delay);  digitalWrite(L3T, LOW);
		digitalWrite(L3L, HIGH);  delay(Delay);  digitalWrite(L3L, LOW);
		digitalWrite(L4T, HIGH);  delay(Delay);  digitalWrite(L4T, LOW);
		digitalWrite(L4L, HIGH);  delay(Delay);  digitalWrite(L4L, LOW);
	}
	 if (VERBOSE) Serial.println("LED test Successful.");
}

/**********************************************************/
//To turn off all the LEDS
void LedOff(void){
	  digitalWrite(L1T, LOW);
	  digitalWrite(L1L, LOW);
	  digitalWrite(L2T, LOW);
	  digitalWrite(L2L, LOW);
	  digitalWrite(L3T, LOW);
	  digitalWrite(L3L, LOW);
	  digitalWrite(L4T, LOW);
	  digitalWrite(L4L, LOW);
}

void BuzzerOff(void){
  if (VERBOSE) Serial.println("Buzzer off");
  digitalWrite(Buzzer, LOW);
  
}
void BuzzerOn(void){
	if (VERBOSE) Serial.println("Buzzer on");
	for(i=0;i<5;i++){
		//Buzzers on
		digitalWrite(Buzzer, HIGH);
		delay(400);
		//Buzzers off
		digitalWrite(Buzzer, LOW);
	}
}
