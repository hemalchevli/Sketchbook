// include the library code:
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
//soft serial tx,rx

// initialize the library with the numbers of the interface pins

SoftwareSerial mySerial(8, 9); 
//rs e d4 - d7
LiquidCrystal lcd(A5,A4,A3,A2,A1,A0);

char temp[16]={
};
int i;
char inchar;
void setup() {
  Serial.begin(9600);//PC


  mySerial.begin(9600);//barcode reader

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Ready!");
}

void loop() {
  if (mySerial.available()){
    //Serial.write(mySerial.read());

    delay(100);
    // clear the screen
    lcd.clear();

    // read all the available characters
    while (mySerial.available() > 0) {
      // inchar = mySerial.read();
      //use while and read till 0x0d
      //while(mySerial.read() == 13)
      //validate data to be ascii
      do{
        inchar = mySerial.read();
        delay(10);
        if(inchar != 0x0D){
          temp[i] = inchar;
        }
        i++;
      }      
      while(inchar != 0x0D);

      /* for(i=0;i<=15;i++){
       inchar = mySerial.read();
       delay(10);
       temp[i] = inchar;
       }*/
      lcd.print(temp);
      Serial.print(temp);

      //reset temp

        for(i=0;i<=15;i++){ 
        temp[i]=0;
      }
      i=0;
    }
  }


  if (Serial.available()){
    //Serial.write(mySerial.read());

    delay(100);
    // clear the screen


    // read all the available characters
    while (Serial.available() > 0) {
      // inchar = mySerial.read();
      //use while and read till 0x0d
      //while(mySerial.read() == 13)
      //validate data to be ascii
      do{
        inchar = Serial.read();
        delay(10);
        if(inchar != 0x0D){
          temp[i] = inchar;
        }
        i++;
      }      
      while(inchar != 0x0D);

      /* for(i=0;i<=15;i++){
       inchar = mySerial.read();
       delay(10);
       temp[i] = inchar;
       }*/
      lcd.setCursor(0,1);
      lcd.print("                   ");
      lcd.setCursor(0,1);
      lcd.print(temp);

      //reset temp

        for(i=0;i<=15;i++){ 
        temp[i]=0;
      }
      i=0;
    }
  }
}

