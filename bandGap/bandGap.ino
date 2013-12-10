void setup(void)
  {
  Serial.begin(115200);
  }
    
void loop(void)
  {
  Serial.println (getBandgap ());
  delay(1000);
  }

const long InternalReferenceVoltage = 1062;  // Adjust this value to your board's specific internal BG voltage
 
// Code courtesy of "Coding Badly" and "Retrolefty" from the Arduino forum
// results are Vcc * 100
// So for example, 5V would be 500.
int getBandgap () 
  {
  // REFS0 : Selects AVcc external reference
  // MUX3 MUX2 MUX1 : Selects 1.1V (VBG)  
   ADMUX = _BV (REFS0) | _BV (MUX3) | _BV (MUX2) | _BV (MUX1);
   ADCSRA |= _BV( ADSC );  // start conversion
   while (ADCSRA & _BV (ADSC))
     { }  // wait for conversion to complete
   int results = (((InternalReferenceVoltage * 1024) / ADC) + 5) / 10; 
   return results;
  } // end of getBandgap
