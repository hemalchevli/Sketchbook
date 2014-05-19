/*
Node sketch for loggin temp and rpm of sizing machine with LCD display
 */
#include <RFM69.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal595.h>



#define NODEID        2    //unique for each node on same network
#define NETWORKID     100  //the same on all nodes that talk to each other
#define GATEWAYID     1
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
#define FREQUENCY   RF69_433MHZ
//#define FREQUENCY   RF69_868MHZ
//#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ACK_TIME      30 // max # of ms to wait for an ack
#define SERIAL_BAUD   115200

int TRANSMITPERIOD = 10000; //transmit a packet to gateway so often (in ms)
byte sendSize=0;
boolean requestACK = false;
RFM69 radio;

//My vars
int temp;
int speeds;

char str[50];
//smooth
const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
int inputPin = A0;

//Speed vars
volatile float time = 0;
volatile float time_last = 0;

float rpm;
int rpm_average;
float rps;

int val;
long last=0;
int stat=LOW;
int stat2;
int count=0;

int sens=75;  // this value indicates the limit reading between dark and light,
// it has to be tested as it may change acording on the 
// distance the leds are placed.
int sprockets=1; // the number of blades of the propeller

int milisegundos=2000; // the time it takes each reading
///////

// initialize the library with the numbers of the interface pins + the row count
// datapin, latchpin, clockpin, num_lines
LiquidCrystal595 lcd(A2,A1,A0);

void setup() {
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  lcd.begin(16,2);

#ifdef IS_RFM69HW
  radio.setHighPower(); //uncomment only for RFM69HW!
#endif
  radio.encrypt(ENCRYPTKEY);
  char buff[50];
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);

  //array elements zero
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0; 

}

long lastPeriod = -1;
void loop() {
  if (radio.receiveDone())
  {
    Serial.print('[');
    Serial.print(radio.SENDERID, DEC);
    Serial.print("] ");
    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);
    Serial.print("   [RX_RSSI:");
    Serial.print(radio.RSSI);
    Serial.print("]");

    if (radio.ACK_REQUESTED)
    {
      radio.sendACK();
      Serial.print(" - ACK sent");
      delay(10);
    }

    Serial.println();
  }
  /////////////////////////////
  val=digitalRead(3);
  if(val == HIGH)
    stat=HIGH;
  else
    stat=LOW;
  //   digitalWrite(13,stat); //as iR light is invisible for us, the led on pin 13 
  //indicate the state of the circuit.

  if(stat2!=stat){  //counts when the state change, thats from (dark to light) or 
    //from (light to dark), remmember that IR light is invisible for us.
    count++;
    stat2=stat;
  }
  if(millis()-last>=milisegundos){
    //Serial.println(count);
    rps=((float)count/sprockets)/2.0*1000.0/milisegundos;
    rpm=((float)count/sprockets)/2.0*60000.0/(milisegundos);
    //  Serial.print((count/2.0));Serial.print("  RPS ");Serial.print(rps);
    // Serial.print(" RPM");Serial.print(rpm);Serial.print("  VAL ");Serial.println(val);
    count=0;
    rpm_average =(int)rpm;
    last=millis();
  }
  /////////////////////////////////////
  ////////////////////////////////////
  //Process Temp
  temp = analogRead(A5);
  //Moving average
  average = ((numReadings - 1)*average + temp)/numReadings;
  //////////////////////////////////////

  int currPeriod = millis()/TRANSMITPERIOD;
  // sprintf(str,"Temp: %d RPM: %f",average,rpm);   
  // Serial.println(str);
  //delay(100);
  if (currPeriod != lastPeriod)
  {

    lastPeriod=currPeriod;
    sprintf(str,"#%d#%d#",average,rpm_average);
    //   Serial.println();
    Serial.println(str);
    if (radio.sendWithRetry(GATEWAYID, str, 10))
      Serial.print(" ok!");
    else Serial.print(" nothing...");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temp: ");
    lcd.print(average);
    lcd.setCursor(0,1);
    lcd.print("RPM: ");
    lcd.print(rpm_average);
  }
}


void printDouble( double val, unsigned int precision){
  // prints val with number of decimal places determine by precision
  // NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
  // example: printDouble( 3.1415, 100); // prints 3.14 (two decimal places)

  Serial.print (int(val));  //prints the int part
  Serial.print("."); // print the decimal point
  unsigned int frac;
  if(val >= 0)
    frac = (val - int(val)) * precision;
  else
    frac = (int(val)- val ) * precision;
  Serial.println(frac,DEC) ;
} 


