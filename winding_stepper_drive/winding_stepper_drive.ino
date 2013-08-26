//Generates pulse train of specified pulse number, frequency, and duty cycle
//Pin 7 output 

int pulseNumber = 25000;          // Number of pulses in pulse train                 
double frequency = 20000;            //frequency in Hz                              
double dutyCycle = .25;          //duty cycle                                        
unsigned long seconds = 1;        //delay between pulse sets                         

int on;
int off;                    
double period;  
const int dc_motor = 9;
int dc_speed;      //speed of dc motor in rpm
const int dc_pot = A5;

#define fastWrite(_pin_, _state_) ( _pin_ < 8 ? (_state_ ?  PORTD |= 1 << _pin_ : PORTD &= ~(1 << _pin_ )) : (_state_ ?  PORTB |= 1 << (_pin_ -8) : PORTB &= ~(1 << (_pin_ -8)  )))
// the macro sets or clears the appropriate bit in port D if the pin is less than 8 or port B if between 8 and 13


void setup() {
  pinMode(7, OUTPUT);          // set outPin pin as output
period = (1 / frequency) * 1000000;
on = dutyCycle * period;
off = period * (1-dutyCycle);
}

void loop() {
  set_dc_speed();

  // call Pulse function for n = pulseNumber
 for (int i=1; i<=pulseNumber; i++) {  
    Pulse(on, off);
  }
  
  //delay(seconds * 1000UL);  // delay between pulse sets
} 

void Pulse(int on, int off) {
  fastWrite(6, HIGH);       // set Pin high
  delayMicroseconds(on);      // waits "on" microseconds
  fastWrite(6, LOW);        // set pin low
  delayMicroseconds(off);      //wait "off" microseconds
}
void set_dc_speed(){
   //read the sensor value:
  dc_speed = analogRead(dc_pot);
  dc_speed = dc_speed/4;
  dc_speed = max(dc_speed, 1);
  dc_speed = min(dc_speed, 255);
  // map it to a range from 0 to 100:
  //dc_speed = map(dc_speed, 0, 1023, 0, 255);
//  dc_speed = max(dc_speed, 90);
//  dc_speed = min(dc_speed, 255);	
  analogWrite(dc_motor,dc_speed);

}
