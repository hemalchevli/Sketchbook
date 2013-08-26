/*	
Software reset
« Reply #5 on: July 02, 2009, 10:07:36 am »
	Bigger Bigger Smaller Smaller Reset Reset
Thanks guys for your answers. I'd like to test the watchdog solution though. Not sure about my bootloader.

I found that:
CAUTION! On newer AVRs, once the watchdog is enabled, then it stays enabled, even after a reset! For these newer AVRs a function needs to be added to the .init3 section (i.e. during the startup code, before main()) to disable the watchdog early enough so it does not continually reset the AVR.

Here is some example code that creates a macro that can be called to perform a soft reset:
*/
#include <avr/wdt.h>
#define soft_reset() do{wdt_enable(WDTO_15MS);for(;;){}} while(0);  

// Function Pototype
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

// Function Implementation
void wdt_init(void)
{
   MCUSR = 0;
   wdt_disable();

   return;
}
void setup(){
}
void loop(){
}
////////////////////////////////////////////////////////////
// Based on: http://arduino.cc/playground/Main/PinChangeIntExample
// More info on uA's: http://www.rocketscream.com/blog/2011/07/04/lightweight-low-power-arduino-library/
#include <Ports.h>
#include <RF12.h>
#include <avr/sleep.h>
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>
 
ISR(WDT_vect) { Sleepy::watchdogEvent(); }
 
#define PIR               5
#define BUTTON            4
 
void setup() {
  Serial.begin(57600);
  Serial.println("Interrupt example:");
 
  pinMode(PIR, INPUT);
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH);
 
  PRR = bit(PRTIM1);                           // only keep timer 0 going
  ADCSRA &= ~ bit(ADEN); bitSet(PRR, PRADC);   // Disable the ADC to save power
  PCintPort::attachInterrupt(PIR, wakeUp, CHANGE);
  PCintPort::attachInterrupt(BUTTON, wakeUp, CHANGE);
}
 
void wakeUp(){}
 
void loop() {
  if (digitalRead(PIR) == HIGH) {
    Serial.println("Motion detected");
  }
  else if (digitalRead(BUTTON) == LOW) {
    Serial.println("Button pressed");
  }
  Sleepy::powerDown();
}
