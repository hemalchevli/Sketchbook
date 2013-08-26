#include <SoftwareSerial.h>
char inchar; //Will hold the incoming character from the Serial Port.
SoftwareSerial cell(7,8); //Create a 'fake' serial port. Pin 2 is the Rx pin, pin 3 is the Tx pin.

int relay1 = A5;
int relay2 = A4;
int relay3 = A3;


void setup()
{
  // prepare the digital output pins
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
 
  //Serial.begin(9600);
  //Initialize GSM module serial port for communication.
  cell.begin(9600);

  delay(5000); // give 30s time for GSM module to register on network etc.
  cell.println("AT"); // set SMS mode to text 
  delay(700);
  
  cell.println("AT+CMGF=1"); // set SMS mode to text 
  delay(700);
  cell.println("AT+CNMI=1,2,0,0"); // set module to send SMS data to serial out upon receipt 
  delay(700);
}

void loop() 
{
  //If a character comes in from the cellular module...
  if(cell.available() >0)
  {
    
    inchar=cell.read(); 
    Serial.write(inchar);
    if (inchar=='#')
    {
      delay(10);
      inchar=cell.read(); 
    //  Serial.write(inchar);
      if (inchar=='a')
      {
        delay(10);
        inchar=cell.read();
      //  Serial.write(inchar);
        if (inchar=='0')
        {
          digitalWrite(relay1, LOW);
        } 
        else if (inchar=='1')
        {
          digitalWrite(relay1, HIGH);
        }
        delay(10);
        inchar=cell.read(); 
        //Serial.write(inchar);
        if (inchar=='b')
        {
          inchar=cell.read();
          //Serial.write(inchar);
          if (inchar=='0')
          {
            digitalWrite(relay2, LOW);
          } 
          else if (inchar=='1')
          {
            digitalWrite(relay2, HIGH);
          }
          delay(10);
          inchar=cell.read(); 
          //Serial.write(inchar);
          if (inchar=='c')
          {
            inchar=cell.read();
            //Serial.write(inchar);
            if (inchar=='0')
            {
              digitalWrite(relay3, LOW);
            } 
            else if (inchar=='1')
            {
              digitalWrite(relay3, HIGH);
            }
            delay(10);
            
          }
         // cell.println("AT+CMGD=1,4"); // delete all SMS
        }
      }
    }
  }
}

