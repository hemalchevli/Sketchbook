/**
Author: Joshua Mahendran
Date: 16-DEC-2012

Program to identify RPM of motor using 
Bipolar Hall Effect Sensor (Allegro - A3290XUA)

LED is connected to Analog Pin 2. 
LED is ON while readind Sensor.

The Hall Effect sensor is powered with Analog Pin 1.
Prior to taking reading, the sensor is turned on and 
turned off once the reading is taken.

Circuit:

Sensor 
  VCC - Analog Pin 1
  GND - GND
  O/P - Analog Pin 0
  
  10K resistor between VCC and O/P of Sensor.

LED
 Cathode - Connects to GND via suitable Resistor. (In this case using led in protoshield).
 Anode - Analog Pin 2. 
  
**/

long startTime = 0;
int state, prevState = 0;
int sensor = 0;
long counter = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(A2, OUTPUT);
  pinMode(A1,OUTPUT);
  
}

void loop()
{
  //reset variables.
  sensor = 0;
  state = 0;
  prevState = 0;
  counter = 0;
  startTime = millis();
  
  //enable sensor and turn on LED, LCD.
  digitalWrite(A1, HIGH);
  digitalWrite(A2,HIGH);
 Serial.println("Measuring...");

  //Loop for sample duration of 1 minute
  while((millis() - startTime) < 10000)
  {
    sensor =  digitalRead(3);
    if (sensor ==HIGH)
    state = 1;
    else
    state = 0;
    
    //On change of state increment counter.
    //A change in state twice represents one revolution
    if(state != prevState)
    {
      counter++;
      prevState = state;
    }
  }
  
 counter = counter * 6;
    
  //Trun off Sensor and LED
  digitalWrite(A2,LOW);
  digitalWrite(A1, LOW);
  

  Serial.print("RPM = "); Serial.println(counter);
  delay(1000); // delay 10 seconds
}

