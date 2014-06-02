/*
 * Tachometer
 *
 * M.G.McKinley
 * 6/1/2009
 * Adjust  
 * 6/27/13 - Added RPM based output control. by: firepinto
 * http://open-source-energy.org/?tid=1259
 */

//Define Variables
  int ticsPerRev = 1;       // define number of tics per rev of code wheel
  float rpm = 0.0;  // I prefer float for rpm
  volatile int rpmcount =0;  // volitile because the variable is manipulated during the interrupt
  unsigned long timeold = 0; // used to calculate d_t= millis()-timeold;
  int d_t;

  int statusPin = 9;             // LED connected to digital pin (changes state with each tic of code wheel)
  int led10 = 10;
volatile byte status= LOW;    // set initial state of status LED

 
void setup()
 {
   Serial.begin(9600);
   //Interrupt 0 is digital pin 2, so that is where the IR detector is connected
   //Triggers on FALLING (change from HIGH to LOW)
   attachInterrupt(0, rpm_fun, FALLING);
     
   
   pinMode(statusPin, OUTPUT); //Use statusPin to flash along with interrupts
   pinMode(led10, OUTPUT);
 }

 void loop()
 {
   //Update RPM every second
   
   //Don't process interrupts during calculations
   detachInterrupt(0);
   d_t=millis()-timeold;
   
   if (d_t >= 1000)
    {
     rpm = float(60.0*1000.0)/float((d_t))*float(rpmcount)/ticsPerRev;
       
      timeold = millis();
      d_t=0; //reset d_t
     if (rpm == 0){
      digitalWrite(led10, LOW);
      }
      else {
        if (rpm > 20){
        digitalWrite(led10, HIGH);
      }
      else {
        digitalWrite(led10, LOW);
      }  
      }    
      //Serial Port Output
      Serial.print("Time(ms) ");
      Serial.print(timeold); //time at end of interval (hence timeold=millis(); above )
      Serial.print(" TicsPerInterval ");
      Serial.print(rpmcount);
      Serial.print(" RPM ");
      Serial.println(rpm);
     
      rpmcount = 0; //reset rpmcount
     
  }
  //Restart the interrupt processing
  attachInterrupt(1, rpm_fun, FALLING);
 }


void rpm_fun()
 {
   //This interrupt is run at each codewheel tic
   detachInterrupt(0); //im not sure if this is necessary here
   
   rpmcount++; //update rpmcount
     
   //Toggle status LED  
   if (status == LOW) {
     status = HIGH;
   } else {
     status = LOW;
   }
   digitalWrite(statusPin, status);
   attachInterrupt(1, rpm_fun, FALLING);
 }
