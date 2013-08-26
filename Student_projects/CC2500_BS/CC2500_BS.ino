#include <LiquidCrystal.h>

int cc_reset=7;
//lcd(rs,e,d4,d5,d6,d7);
LiquidCrystal lcd(A5,A4,A3,A2,A1,A0);

void setup(){
  Serial.begin(38400);
  lcd.begin(16,2);
  lcd.clear();
  //colxrow
  lcd.setCursor(0,0);
  lcd.print("Initializing");
  //setup cc2500
  digitalWrite(cc_reset,LOW);
  delay(250);
  digitalWrite(cc_reset,HIGH);
  delay(250);

  Serial.print("1");	//Self Address
  Serial.print("2");	//Remote Address
  Serial.print("3");	//Channel
  Serial.flush();
  delay(500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp. at R.S.");
  lcd.setCursor(1,0);
}

void loop(){
  // when characters arrive over the serial port...
  if (Serial.available()) {
    lcd.setCursor(0,2);
    lcd.print("                  ");
    lcd.setCursor(0,2);
    // wait a bit for the entire message to arrive
    delay(100);
    // read all the available characters
    while (Serial.available() > 0) {
      // display each character to the LCD
      lcd.write(Serial.read());
    }
    //col 0,row 2 
    
  }
}








