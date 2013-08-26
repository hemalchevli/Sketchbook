#define NOFIELD 508L    // Analog output with no applied field, calibrate this

// Uncomment one of the lines below according to device in use A1301 or A1302
// This is used to convert the analog voltage reading to milliGauss
//#define TOMILLIGAUSS 1953L  // For A1301: 2.5mV = 1Gauss, and 1024 analog steps = 5V, so 1 step = 1953mG
#define TOMILLIGAUSS 3756L  // For A1302: 1.3mV = 1Gauss, and 1024 analog steps = 5V, so 1 step = 3756mG

float fanalog0,fanalog1;
byte serialByte;
float i =0;
void setup()
{
  Serial.begin(115200);
  Serial.println("Press 'C' to Start logging,and 'F' to stop");
}

void DoMeasurement()
{
 int raw = analogRead(0);   // Range : 0..1024

//  Uncomment this to get a raw reading for calibration of no-field point
//  Serial.print("Raw reading: ");
//  Serial.println(raw);

  long compensated = raw - NOFIELD;                 // adjust relative to no applied field
  long gauss = compensated * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

  //Serial.print(raw);


    Serial.print(i);
    Serial.print(',',BYTE);
    Serial.println(gauss);
    i++;
    //Serial.print(';',BYTE);
    //Serial.println(fanalog1,DEC);
  
}

void loop()
{
    while (Serial.available()>0){  
            serialByte=Serial.read();
            if (serialByte=='C'){	  
        	while(1){
                  DoMeasurement();
                  //delay(200);
               	  if (Serial.available()>0){
        	    serialByte=Serial.read();
                    if (serialByte=='R') i=0;
        	    if (serialByte=='F'){
                      i=0;
                      break;
                     }
          }
       }
     }
  }
}
