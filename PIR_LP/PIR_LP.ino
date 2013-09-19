/*
Soft Rest , when avr is hanged during communication
 Set ID of each device, the GET
 Change sensor ip to interrupt pin, pin D2(4) use changePinInt lib
 WDT active only when AVR is awake
 Turn off ADC,UART,TWI
 Send battery low indication to server, and power down
 
 If room is booked send, from server side send booked for # days when the motion is detected
 AVR will power down for that many days.
 Response from server will be 
 $$OK - packet sent
 $$KO - error
 $$B4 - booked for 4 days -> sleep for 4 days
 
 */
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

//PIR sensor,led and disconnect button
#define PIR 2  //INT 0 PIN 4
#define DISCONNECT 19//A5 pulled up, not used for now
#define LED 16 //A2 high on

volatile int pirState = LOW;             // we start, assuming no motion detected
volatile char sleep;
int val = 0;                    // variable for reading the pin status

void static inline pwrDown(void);

void setup() {
  ADCSRA = 0; //Disable ADC
  // turn off various modules
  //PRR =0x81; //0b10000001; TWI and ADC off
  // 7  1     0
  //TWI uart  ADC 
  pinMode(LED, OUTPUT);      // declare LED as output
  pinMode(PIR, INPUT);     // declare sensor as input
  Serial.begin(9600);
  attachInterrupt(0, motion, RISING); 
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
  sleep = 1;
  cli();
  pwrDown(); 

   wdt_enable (WDTO_8S);
     Serial.println("reset");
}

void loop(){ 

  Serial.println(pirState);
  delay(100);
  digitalWrite(LED,pirState);
  cli();
  pwrDown(); 
  wdt_reset();


  
}

//ISR for INT0
void motion() {
  pirState = !pirState; 

}
void static inline pwrDown(void)
{   
  //PRR =0xFF; //turn off all modules 
  MCUCR = MCUCR | bit(BODSE) | bit(BODS); //timed sequence
  MCUCR = MCUCR & ~bit(BODSE) | bit(BODS);//to turn off BOD in sleep
  sei(); //enable int
  sleep_mode(); //sleeping set SE got sleep and clear SE when waking up
  //sleep_enable(); //slee_mode() does all three.
  //sleep_cpu();
  //sleep_disable();
  //PRR =0x81; //0b10000011; turn off all except uart and spi
  // sleep = 0;
} 
/*
void WDT_off(void)
 {
 cli();  
 __watchdog_reset();
 // Clear WDRF in MCUSR 
 MCUSR &= ~(1<<WDRF);
 // Write logical one to WDCE and WDE 
 // Keep old prescaler setting to prevent unintentional time-out 
 WDTCSR |= (1<<WDCE) | (1<<WDE);
 //Turn off WDT 
 WDTCSR = 0x00;
 __enable_interrupt();
 }*/




