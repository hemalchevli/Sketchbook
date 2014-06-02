//
// This example shows one way of creating an optoswitch
// using an IR LED as emiter and an IR LED receiver as
// light sensor.
// On this case it acts as a tachometer to count the
// revolutions per second of an aeromodelism plane's
// propeller.
//
//           + GROUND                                 +GROUND          
//           |                                        |  
//           <                                        < 
//           > 220 ohm resistor                       > 220 omh resistor
//           <                                        <      
//           |                                        |  
//           |                                        |
//         -----                                    -----
//          / \    >>IR LED emiter >>>>>>>>>>>>>>>>  / \   IR LED receiver
//         -----                                    -----
//           |                                        |
//           |                                        |
//           + +5VCD                                  +  ANALOG INPUT 0
//


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
void setup()
{
  Serial.begin(115200);
 
}

void loop()
{
  val=digitalRead(2);
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
    // Serial.println(count);
     double rps=((double)count/sprockets)/2.0*1000.0/milisegundos;
     double rpm=((double)count/sprockets)/2.0*60000.0/(milisegundos);
     Serial.print((count/2.0));Serial.print("  RPS ");Serial.print(rps);
     Serial.print(" RPM");Serial.print(rpm);Serial.print("  VAL ");Serial.println(val);
     count=0;
     last=millis();
   }
}
