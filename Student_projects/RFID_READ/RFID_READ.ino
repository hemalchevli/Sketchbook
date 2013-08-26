
    #include <SoftwareSerial.h>
    //rx,tx
    SoftwareSerial mySerial(2, 3);

    void setup() 
    {
      Serial.begin(9600);
      Serial.println("Scan RFID Tag!");

      // set the data rate for the NewSoftSerial port
      mySerial.begin(9600);
    }

    void loop()                     // run over and over again
    {

      if (mySerial.available()) {
          Serial.print((char)mySerial.read());
      }
      if (Serial.available()) {
          mySerial.print(Serial.read());
      }
    }
