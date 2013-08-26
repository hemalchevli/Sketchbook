int analog0;
int analog1;
int analog2;
int analog3;
int analog4;
int analog5;
int analog6;

unsigned long time;
byte serialByte;
float i;
//////////////

////////////////
void setup() {

  Serial.begin(115200);
  Serial.println("Press 'C' to Start logging,and 'F' to stop");
}

void loop()
{

  while (Serial.available()>0){  
    serialByte=Serial.read();
    if (serialByte=='C'){	  
      while(1){
        analog0=analogRead(0);
        analog1=analogRead(1);
        analog2=analogRead(2);
        analog3=analogRead(3);
        analog4=analogRead(4);
        analog4=analogRead(5);
        analog4=analogRead(6);

        Serial.print(analog0,DEC);
        Serial.print(',',BYTE);
        Serial.print(analog1,DEC);
        Serial.print(',',BYTE);
        Serial.print(analog2,DEC);
        Serial.print(',',BYTE);
        Serial.print(analog3,DEC);
        Serial.print(',',BYTE);
        Serial.println(analog4,DEC);
        Serial.print(',',BYTE);
        Serial.print(analog5,DEC);
        Serial.print(',',BYTE);
        Serial.print(analog6,DEC);


        if (Serial.available()>0){
          serialByte=Serial.read();
          if (serialByte=='F')  {
          break;
          }    
        }
      }
    }
  }
}


