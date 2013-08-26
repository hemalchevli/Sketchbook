#define NOFIELD1 514L    // Analog output with no applied field, calibrate this
#define NOFIELD2 511L    // Analog output with no applied field, calibrate this
#define NOFIELD3 510L    // Analog output with no applied field, calibrate this
#define NOFIELD4 495L    // Analog output with no applied field, calibrate this

// Uncomment one of the lines below according to device in use A1301 or A1302
// This is used to convert the analog voltage reading to milliGauss
//#define TOMILLIGAUSS 1953L  // For A1301: 2.5mV = 1Gauss, and 1024 analog steps = 5V, so 1 step = 1953mG
#define TOMILLIGAUSS 3756L  // For A1302: 1.3mV = 1Gauss, and 1024 analog steps = 5V, so 1 step = 3756mG

//

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
//
byte serialByte;
int j=1;
int raw[3];
long gauss[3],compensated[3];
void setup()
{
  setIO();
  LedOff();
  TestLED(1,500);
  Serial.begin(115200);
  Serial.println("Press 'C' to Start logging,and 'F' to stop, ");
  Serial.println("Press 'R' to get raw values, or press any key to continue");
//   while (Serial.available()>0){  
//            serialByte=Serial.read();
//            if(serialByte == 'R') 
//            {
//              j=10;
//               Serial.println("Raw Values");
//            }
//            else {
//               Serial.println("Gauss Values");
//              j=1;
//            
//            }
//   }
}
void loop()
{
  while (Serial.available()>0){  
    serialByte=Serial.read();
    if (serialByte=='R') j != j;
    if (serialByte=='C'){	  
    while(1){
      logdata();
      delay(200);
      if (Serial.available()>0){
        serialByte=Serial.read();
        if (serialByte=='F') break;
        }
      }
    }
  }
}

void logdata()
{
 raw[0] = analogRead(2);   // Range : 0..1024
 raw[1] = analogRead(3);   // Range : 0..1024
 raw[2] = analogRead(4);   // Range : 0..1024
 raw[3] = analogRead(5);   // Range : 0..1024

//  Uncomment this to get a raw reading for calibration of no-field point
//  Serial.print("Raw reading: ");
//  Serial.println(raw);

  compensated[0] = raw[0] - NOFIELD1;                 // adjust relative to no applied field
  gauss[0] = compensated[0] * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

  compensated[1] = raw[1] - NOFIELD2;                 // adjust relative to no applied field
  gauss[1] = compensated[1] * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

  compensated[2] = raw[2] - NOFIELD3;                 // adjust relative to no applied field
  gauss[2] = compensated[2] * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

  compensated[3] = raw[3] - NOFIELD4;                 // adjust relative to no applied field
  gauss[3] = compensated[3] * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

 if (j==1){
    Serial.print(gauss[0]);
    Serial.print(',',BYTE);
    Serial.print(gauss[1]);
    Serial.print(',',BYTE);
    Serial.print(gauss[2]);
    Serial.print(',',BYTE);
    Serial.println(gauss[3]);
}
 if (j==0){

    Serial.print(raw[0]);
    Serial.print(',',BYTE);
    Serial.print(raw[1]);
    Serial.print(',',BYTE);
    Serial.print(raw[2]);
    Serial.print(',',BYTE);
    Serial.println(raw[3]);
    
    
 }
    //Serial.print(';',BYTE);
    //Serial.println(fanalog1,DEC);
  
}
///////////////////////////////////////////////

//Test if all LEDs are Working
void TestLED(int times,int Delay){
	
	for(int i=0;i<times;i++){
		digitalWrite(L1T, LOW);  delay(Delay);  digitalWrite(L1T, HIGH);
		digitalWrite(L1L, LOW);  delay(Delay);  digitalWrite(L1L, HIGH);
		digitalWrite(L2T, LOW);  delay(Delay);  digitalWrite(L2T, HIGH);
		digitalWrite(L2L, LOW);  delay(Delay);  digitalWrite(L2L, HIGH);
		digitalWrite(L3T, LOW);  delay(Delay);  digitalWrite(L3T, HIGH);
		digitalWrite(L3L, LOW);  delay(Delay);  digitalWrite(L3L, HIGH);
		digitalWrite(L4T, LOW);  delay(Delay);  digitalWrite(L4T, HIGH);
		digitalWrite(L4L, LOW);  delay(Delay);  digitalWrite(L4L, HIGH);
	}
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
