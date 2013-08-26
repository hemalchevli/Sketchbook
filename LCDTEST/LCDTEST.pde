#include <LiquidCrystal.h>

LiquidCrystal lcd(16,14,12,11,10,9);
const int buttonPin = 2; 

uint8_t candleon[8] = {0x04,0x00,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E};  //a custom HEX character
uint8_t candleoff[8] = {0x00,0x00,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E};  //a custom HEX character
uint8_t smile[8] = {0x00,0x0A,0x00,0x11,0x0E,0x00,0x00,0x00};  //a custom HEX character
uint8_t heart[8] = {0x00,0x0A,0x1F,0x1F,0x0E,0x04,0x00,0x00};  //a custom HEX character
char Line1[42] = "May all your dreams and wishes come true!";
char Line2[49] = "Wishing you miles of smiles in the coming years.";
char Line3[63] = "Wishing you all the best for your life and the things to come.";
char Line4[34] = "Wishing you an awesome day ahead.";
char Line5[36] = "Once again Happy Birthday Sunshine.";
char Line6[59] = "May this day be filled with everything your heart desires.";
char SecretLine1[67] = "You are beautiful with a beautiful heart,you are one in a billion.";
char SecretLine2[87] = "To have you in my life is a special gift; I hope we can treasure each other forever.";
char SecretLine3[72] = "P.S : I will not discuss about this again, till you are not willing to.";

int count=0;
int bytei,i;

int buttonPushCounter = 0;
int buttonState = 0;      
int lastButtonState = 0; 


void setup() {
  pinMode(buttonPin, INPUT);
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  lcd.createChar(1, candleon);
  lcd.createChar(2, candleoff);
  lcd.createChar(3, smile);
  lcd.createChar(4, heart);
  lcd.setCursor(1,0);
  // Print a message to the LCD.
     
  lcd.print("wazza");
   lcd.leftToRight(); 
for(i=0;i<9;i++){
  lcd.print("a");
  // wait a second:
  delay(500);
  // increment the letter:
   }
 delay(2000);
   lcd.setCursor(0,0);
   lcd.print("*Happy Birthday*");
   lcd.setCursor(4,1);
   lcd.print("Pratiksha");
for(i=0;i<5;i++){   
   lcd.noDisplay();
   delay(500);
   // Turn on the display:
   lcd.display();
   delay(500);
}
delay(700);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Some candles");
delay(1000);
lcd.setCursor(0,1);
//candles array
lcd.print(1,BYTE);delay(200);lcd.print(1,BYTE);lcd.print(1,BYTE);
lcd.print(1,BYTE);delay(200);lcd.print(1,BYTE);lcd.print(1,BYTE);
lcd.print(1,BYTE);delay(200);lcd.print(1,BYTE);lcd.print(1,BYTE);
lcd.print(1,BYTE);delay(200);lcd.print(1,BYTE);lcd.print(1,BYTE);
lcd.print(1,BYTE);delay(200);lcd.print(1,BYTE);lcd.print(1,BYTE);
lcd.print(1,BYTE);
delay(2000);
lcd.setCursor(0,0);
lcd.print("Now Blow them");
delay(2000);
lcd.setCursor(0,1);
lcd.print(2,BYTE);delay(200);lcd.print(2,BYTE);delay(200);lcd.print(2,BYTE);delay(200);
lcd.print(2,BYTE);delay(200);lcd.print(2,BYTE);delay(200);lcd.print(2,BYTE);delay(200);
lcd.print(2,BYTE);delay(200);lcd.print(2,BYTE);delay(200);lcd.print(2,BYTE);delay(200);
lcd.print(2,BYTE);delay(200);lcd.print(2,BYTE);delay(200);lcd.print(2,BYTE);delay(200);
lcd.print(2,BYTE);delay(200);lcd.print(2,BYTE);delay(200);lcd.print(2,BYTE);delay(200);
lcd.print(2,BYTE);delay(200);
delay(2000);

lcd.clear();
lcd.print("Sent on:");
lcd.setCursor(0,1);
lcd.print("13 May 2012");
delay(1000);
//////////////////////////////////////////////////
//Display Line 1.....
lcd.noDisplay();
delay(800);
lcd.clear();
lcd.display();
lcd.print("Dear Pratiksha,");

//scroll msg
lcd.setCursor(0,1);
lcd.print("May all your dre");
delay(1500);
while (Line1[bytei+15] != '\0') {
      // scrolly scrolly 
       lcd.setCursor(0,1);
      for ( i= 0; i < 16; i++) {
        lcd.write(Line1[bytei + i]);
      }
      bytei++;
      delay(300);
    } 
delay(1500);
/////////////////////////////////////////////////
//Display Line 2.....
//lcd.noDisplay();
//delay(800);
//lcd.clear();
//lcd.display();
//lcd.print("Dear Pratiksha,");

lcd.setCursor(0,1);
//scroll msg
lcd.print("Wishing you mile");
delay(1500);

bytei=0;
while (Line2[bytei+15] != '\0') {
      // scrolly scrolly 
       lcd.setCursor(0,1);
      for ( i= 0; i < 16; i++) {
        lcd.write(Line2[bytei + i]);
      }
      bytei++;
      delay(300);
    }
delay(1500);
////////////////////////////////////////////////
/////////////////////////////////////////////////
//Display Line 3.....
//lcd.noDisplay();
//delay(800);
//lcd.clear();
//lcd.display();
//lcd.print("Dear Pratiksha,");

lcd.setCursor(0,1);
//scroll msg
lcd.print("Wishing you all ");
delay(1500);

bytei=0;
while (Line3[bytei+15] != '\0') {
      // scrolly scrolly 
       lcd.setCursor(0,1);
      for ( i= 0; i < 16; i++) {
        lcd.write(Line3[bytei + i]);
      }
      bytei++;
      delay(300);
    }
delay(1500);    
////////////////////////////////////////////////
/////////////////////////////////////////////////
//Display Line 4.....
//lcd.noDisplay();
//delay(800);
//lcd.clear();
//lcd.display();
//lcd.print("Dear Pratiksha,");

lcd.setCursor(0,1);
//scroll msg
lcd.print("Wishing you an a");
delay(1500);

bytei=0;
while (Line4[bytei+15] != '\0') {
      // scrolly scrolly 
       lcd.setCursor(0,1);
      for ( i= 0; i < 16; i++) {
        lcd.write(Line4[bytei + i]);
      }
      bytei++;
      delay(300);
    }
delay(1500);
////////////////////////////////////////////////
/////////////////////////////////////////////////
//Display Line 6.....
//lcd.noDisplay();
//delay(800);
//lcd.clear();
//lcd.display();
//lcd.print("Dear Pratiksha,");

lcd.setCursor(0,1);
//scroll msg
lcd.print("May this day be ");
delay(1500);

bytei=0;
while (Line6[bytei+15] != '\0') {
      // scrolly scrolly 
       lcd.setCursor(0,1);
      for ( i= 0; i < 16; i++) {
        lcd.write(Line6[bytei + i]);
      }
      bytei++;
      delay(300);
    }
delay(1500);
////////////////////////////////////////////////
/////////////////////////////////////////////////
//Display Line 5.....
//lcd.noDisplay();
//delay(800);
//lcd.clear();
//lcd.display();
//lcd.print("Dear Pratiksha,");

lcd.setCursor(0,1);
//scroll msg
lcd.print("Once again Happy");
delay(1500);

bytei=0;
while (Line5[bytei+15] != '\0') {
      // scrolly scrolly 
       lcd.setCursor(0,1);
      for ( i= 0; i < 16; i++) {
        lcd.write(Line5[bytei + i]);
      }
      bytei++;
      delay(300);
    }
delay(1500);
////////////////////////////////////////////////
delay(1000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("End Of Message");
lcd.setCursor(7,1);
lcd.print(3,BYTE);

}  
void loop() {
    // read the pushbutton input pin:
  buttonState = digitalRead(buttonPin);

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button
      // wend from off to on:
      buttonPushCounter++;
//      Serial.println("on");
//      Serial.print("number of button pushes:  ");
//      Serial.println(buttonPushCounter, DEC);
         
    }
    else {
      // if the current state is LOW then the button
      // wend from on to off:
     // Serial.println("off");
    }
  }
  lastButtonState = buttonState;
  // save the current state as the last state,
  //for next time through the loop
  if(buttonPushCounter>=8){
        buttonPushCounter = 0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Secret Message");
        //display secret msg here once
        /////////////////////////////////////////////////
        //Display SecretLine 1.....
        delay(2000);
        lcd.clear();
        lcd.display();
        lcd.print("Dear Pratiksha,");
        
        lcd.setCursor(0,1);
        //scroll msg
        lcd.print("You are beautifu");
        delay(1500);
        
        bytei=0;
        while (SecretLine1[bytei+15] != '\0') {
              // scrolly scrolly 
               lcd.setCursor(0,1);
              for ( i= 0; i < 16; i++) {
                lcd.write(SecretLine1[bytei + i]);
              }
              bytei++;
              delay(300);
            }
        delay(1500);
        ////////////////////////////////////////////////
        /////////////////////////////////////////////////
        //Display SecretLine2.....
        //lcd.noDisplay();
        //delay(800);
        //lcd.clear();
        //lcd.display();
        //lcd.print("Dear Pratiksha,");
        
        lcd.setCursor(0,1);
        //scroll msg
        lcd.print("To have you in m");
        delay(1500);
        
        bytei=0;
        while (SecretLine2[bytei+15] != '\0') {
              // scrolly scrolly 
               lcd.setCursor(0,1);
              for ( i= 0; i < 16; i++) {
                lcd.write(SecretLine2[bytei + i]);
              }
              bytei++;
              delay(300);
            }
        delay(1500);
    ////////////////////////////////////////////////
     // i heart you
         lcd.clear();
         lcd.setCursor(6,0);
         lcd.print('I');
         delay(500);
         lcd.print(4,BYTE);
         delay(500);
         lcd.print("U");
         delay(500);
         lcd.setCursor(3,1);
         lcd.print("As a friend");
         delay(2000);
         lcd.clear();
         lcd.setCursor(6,0);
         lcd.print('I');
         lcd.print(4,BYTE);
         lcd.print("U");
         lcd.setCursor(3,1);
         lcd.print("and More");
         delay(3000);
       
         lcd.clear();
         ///////////////////////////
          /////////////////////////////////////////////////
        //Display SecretLine3.....
        //lcd.noDisplay();
        //delay(800);
        //lcd.clear();
        //lcd.display();
        //lcd.print("Dear Pratiksha,");
        
        lcd.setCursor(0,1);
        //scroll msg
        lcd.print("P.S : I will not");
        delay(1500);
        
        bytei=0;
        while (SecretLine3[bytei+15] != '\0') {
              // scrolly scrolly 
               lcd.setCursor(0,1);
              for ( i= 0; i < 16; i++) {
                lcd.write(SecretLine3[bytei + i]);
              }
              bytei++;
              delay(300);
            }
        delay(1500);
    ////////////////////////////////////////////////
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("End Of Message");
        lcd.setCursor(7,1);
      lcd.print(3,BYTE);
        /////////////////////////////////////////////////
      }
        //////////////////
   
}

 






