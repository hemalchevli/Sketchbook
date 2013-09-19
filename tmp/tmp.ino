#include <avr/wdt.h>
int pin = A2;
volatile int state = LOW;

void setup()
{
  pinMode(pin, OUTPUT);
  Serial.begin(9600);
  attachInterrupt(0, blink, RISING);
  Serial.println("reset");
  wdt_enable (WDTO_8S);
}

void loop()
{
  Serial.println(state);
  digitalWrite(pin, state);
  delay(500);
  wdt_reset();
}

void blink()
{
  state = !state;
}

