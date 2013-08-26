//Osciloscope and logic analyser output on KST
//Six analog channels used as input
//TODO SD card support


const int sensor0 = A0;    // pin that the sensor is attached to 23 C0
const int sensor1 = A1;    // pin that the sensor is attached to 24 C1
const int sensor2 = A2;    // pin that the sensor is attached to 25 C2
const int sensor3 = A3;    // pin that the sensor is attached to 26 C3
const int sensor4 = A4;    // pin that the sensor is attached to 26 C3
const int sensor5 = A5;    // pin that the sensor is attached to 26 C3

byte serialByte;

int raw[6];
int milliseconds = 200; //delay between two scans, set according to your need
                     
void setup()
{
  setIO();    // set pins as inputs
  Serial.begin(115200);
  Serial.println("Press 'S' to Start logging,and 'F' to stop or pause datalogging ");//to start and pause

}
void loop()
{
  while (Serial.available()>0){  
    serialByte=Serial.read();
      if (serialByte=='S'){	  
        
        while(1){
          logdata();
          //delay(milliseconds);
          if (Serial.available()>0){
            serialByte=Serial.read();
            if (serialByte=='F') break;
          }
        }
        
        
      }
  }
}

void logdata(){

  raw[0] = analogRead(sensor0);   
  raw[1] = analogRead(sensor1);   
  raw[2] = analogRead(sensor2);   
  raw[3] = analogRead(sensor3);   
  raw[4] = analogRead(sensor4);   
  raw[5] = analogRead(sensor5);   
  
  //If needed do processing of data here eg averaging
  
  //send the values as CSV
  Serial.print(raw[0]);
  Serial.print(',',BYTE);

  Serial.print(raw[1]);
  Serial.print(',',BYTE);

  Serial.print(raw[2]);
  Serial.print(',',BYTE);
  
  Serial.print(raw[3]);
  Serial.print(',',BYTE);

  Serial.print(raw[4]);
  Serial.print(',',BYTE);

  Serial.println(raw[5]);

}


void setIO (void){
  //Sensors as input
  pinMode(sensor0, INPUT);
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);
  pinMode(sensor5, INPUT);
}


