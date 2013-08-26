void setup() 
{
  Serial.begin(9600);
  ADMUX = 0xC8; // turn on internal reference, right-shift ADC buffer, ADC channel = internal temp sensor
  delay(10);  // wait a sec for the analog reference to stabilize
}

void loop() 
{
  Serial.println(averageTemperature()); // so we can debug
  delay(500); // just to slow things down a bit
}

int readTemperature() 
{
  ADCSRA |= _BV(ADSC); // start the conversion
  while (bit_is_set(ADCSRA, ADSC)); // ADSC is cleared when the conversion finishes
  return (ADCL | (ADCH << 8)) - 342; // combine bytes & correct for temp offset (approximate)} 
}

float averageTemperature()
{
  readTemperature(); // discard first sample (never hurts to be safe)

  float averageTemp; // create a float to hold running average
  for (int i = 1; i < 1000; i++) // start at 1 so we dont divide by 0
    averageTemp += ((readTemperature() - averageTemp)/(float)i); // get next sample, calculate running average

  return averageTemp; // return average temperature reading
} 
