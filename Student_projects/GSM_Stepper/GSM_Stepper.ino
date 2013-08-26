#include <SoftwareSerial.h>
#include <Stepper.h>

//Variables
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
char inchar; //Will hold the incoming character from the Serial Port.

char number[12]={'9','1','9','8','9','8','0','0','8','1','1','6'};// Stepper will only operate if msg is from this number.

char temp[12]={}; //init all elements to 0 , to store the incoming number
int ok; //number ok or not
int i=0; //counter for 'for' loop

//create object myStepper of class Stepper
Stepper myStepper(stepsPerRevolution, 17,16);      


SoftwareSerial cell(7,8); //Create a 'fake' serial port. Pin 13 is the Rx pin, pin 14 is the Tx pin.

//Initializations
void setup()
{
  Serial.begin(9600);

  //Initialize GSM module serial port for communication.
  cell.begin(9600);

  Serial.println("Waiting for gms to init");
  delay(5000); // give 5s time for GSM module to register on network etc.

  cell.println("AT"); // Sync for baud rate
  delay(700);
  
  Serial.println("AT sent for sync");

  cell.println("AT+CMGF=1"); // set SMS mode to text 
  delay(700);
  
  Serial.println("SMS mode set to text");

  cell.println("AT+CNMI=1,2,0,0"); // set module to send SMS data to serial out upon receipt 
  delay(700);
  
  Serial.println("SMS to serial output");
  myStepper.setSpeed(60);
}

void loop() 
{
  //If a character comes in from the cellular module...
  if(cell.available() >0)
  {
    inchar=cell.read(); 
    Serial.write(inchar);
    if (inchar == '"'){
      delay(10);
      inchar=cell.read(); 
      delay(10);
      if(inchar == '+'){
        for(i=0;i<=12;i++){
          inchar = cell.read();
          delay(10);
          temp[i] = inchar;
        }
        ok=compare(number,temp,12);
        Serial.println("Number:");
        Serial.println(" ");

        for (i = 0; i <= 12; i++) {
          Serial.println(temp[i]);
        }

        Serial.println(" ");
        Serial.println("ok ");
        Serial.println(ok);
        Serial.println(" ");
      }
    }
    if (ok==1){
      if (inchar=='*')
      {

        delay(10);
        inchar=cell.read(); 
        Serial.write(inchar);
        if (inchar=='o')
        {
          delay(10);
          inchar=cell.read();
          Serial.write(inchar);
          if (inchar=='f')
          {
            //Open gate fast
            Serial.println("open fast");
            myStepper.setSpeed(60);
//            myStepper.step(stepsPerRevolution);
            myStepper.step(stepsPerRevolution*15);
            ok=0;

          } 
          else if (inchar=='s')
          {
            //Open gate Slow      
            Serial.println("open slow");
            myStepper.setSpeed(30);  
            myStepper.step(stepsPerRevolution);
            ok=0;

          }
        }
        else if(inchar=='c')
        {
          delay(10);
          inchar=cell.read();
          Serial.write(inchar);
          if(inchar=='f')
          {
            //close gate fast
            Serial.println("close fast");            
            myStepper.setSpeed(60);
            myStepper.step(-stepsPerRevolution);
            ok=0;

          }
          else if(inchar=='s')
          {
            //close gate slow
            Serial.println("close slow");            
            myStepper.setSpeed(30); 
            myStepper.step(-stepsPerRevolution);
            ok=0;
          }
        }
      }

    }
  }
}

//compare arrays      
int compare(char a1 [], char a2 [], int n)
{
  int i, result;
  for (i=0; i<n; ++i)
    if (a1[i] != a2[i]) result = 0;
    else result = 1;

  return (result);
}




