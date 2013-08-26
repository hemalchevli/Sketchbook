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
        LedOff();

	
}
/********************SETUP END***************************/

/********************LOOP START***************************/
void loop()
{
TestLED(1,1000);
delay(2000);
LedOff();
}
/********************LOOP END***************************/


//Test if all LEDs are Working
void TestLED(int times,int Delay){
	 if (VERBOSE) Serial.println("LED test Start.");
	for(i=0;i<times;i++){
		digitalWrite(L1T, LOW);  delay(Delay);  digitalWrite(L1T, HIGH);
		digitalWrite(L1L, LOW);  delay(Delay);  digitalWrite(L1L, HIGH);
		digitalWrite(L2T, LOW);  delay(Delay);  digitalWrite(L2T, HIGH);
		digitalWrite(L2L, LOW);  delay(Delay);  digitalWrite(L2L, HIGH);
		digitalWrite(L3T, LOW);  delay(Delay);  digitalWrite(L3T, HIGH);
		digitalWrite(L3L, LOW);  delay(Delay);  digitalWrite(L3L, HIGH);
		digitalWrite(L4T, LOW);  delay(Delay);  digitalWrite(L4T, HIGH);
		digitalWrite(L4L, LOW);  delay(Delay);  digitalWrite(L4L, HIGH);
	}
	 if (VERBOSE) Serial.println("LED test Successful.");
}

/**********************************************************/
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

void LedOff(void){
	  digitalWrite(L1T, HIGH);
	  digitalWrite(L1L, HIGH);
	  digitalWrite(L2T, HIGH);
	  digitalWrite(L2L, HIGH);
	  digitalWrite(L3T, HIGH);
	  digitalWrite(L3L, HIGH);
	  digitalWrite(L4T, HIGH);
	  digitalWrite(L4L, HIGH);
}

