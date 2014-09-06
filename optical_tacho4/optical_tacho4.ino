//Circuit Diagram
//
//           + GROUND                                 + 5VDC        
//           |                                        |  
//           <                                        < 
//           > 220 ohm resistor                       > 10k ohm resistor
//           <                                        <      
//           |                                        |  
//           |                                        |---------D2 on arduino
//         -----                                    -----
//          / \    >>IR LED emiter >>>>>>>>>>>>>>>>  / \   IR LED receiver
//         -----                                    -----
//           |                                        |
//           |                                        |
//           + +5VCD                                  +  GROUND
//
//When IR is incedent on the receiver, the state of the D2 pin is GND, when the beam is broken
//state of D2 is 5v.


//**Caution this may not work outdoors, especially in sunlight.

int val;
long last=0;
int stat=LOW;
int stat2;
int count=0;


int sprockets=1; // the number of blades of the propeller, change this according to your need

int sampling=2000; // the time it takes each reading
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
   digitalWrite(13,stat); //as iR light is invisible for us, the led on pin 13 
                          //indicate the state of the circuit.
                          //status led

   if(stat2!=stat){  //counts when the state change, thats from (dark to light) or 
                     //from (light to dark)
     count++;
     stat2=stat;
   }
   if(millis()-last>=sampling){
    // Serial.println(count);
     double rps=((double)count/sprockets)/2.0*1000.0/sampling;
     double rpm=((double)count/sprockets)/2.0*60000.0/(sampling);
     Serial.print((count/2.0));Serial.print("  RPS ");Serial.print(rps);
     Serial.print(" RPM");Serial.print(rpm);Serial.print("  VAL ");Serial.println(val);
     count=0;
     last=millis();
   }
}
