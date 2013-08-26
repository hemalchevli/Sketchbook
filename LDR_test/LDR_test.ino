/*
  Analog Input
 Demonstrates analog input by reading an analog sensor on analog pin 0 and
 turning on and off a light emitting diode(LED)  connected to digital pin 13. 
 The amount of time the LED will be on and off depends on
 the value obtained by analogRead(). 
 
 The circuit:
 * Potentiometer attached to analog input 0
 * center pin of the potentiometer to the analog pin
 * one side pin (either one) to ground
 * the other side pin to +5V
 * LED anode (long leg) attached to digital output 13
 * LED cathode (short leg) attached to ground
 

 */

int sensorPin = A3;    // select the input pin for the potentiometer
int ledPin = 9;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  // declare the ledPin as an OUTPUT:
Serial.begin(9600);
  pinMode(ledPin, OUTPUT);  

}

void loop() {
  // read the value from the sensor:
	sensorValue = analogRead(sensorPin);    
	  // turn the ledPin on
	analogWrite(ledPin,sensorValue);
	Serial.println(sensorValue);

	delay(250);
}

