/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(A5, OUTPUT);     
  pinMode(A4, OUTPUT);     
  pinMode(A3, OUTPUT);     
      
}

void loop() {
  digitalWrite(A5, HIGH);   // set the LED on
  delay(1000);              // wait for a second
  digitalWrite(A5, LOW);    // set the LED off
  delay(1000);              // wait for a second
  
  digitalWrite(A4, HIGH);   // set the LED on
  delay(1000);              // wait for a second
  digitalWrite(A4, LOW);    // set the LED off
  delay(1000);              // wait for a second
  
  digitalWrite(A3, HIGH);   // set the LED on
  delay(1000);              // wait for a second
  digitalWrite(A3, LOW);    // set the LED off
  delay(1000);              // wait for a second
}
