#include <Keypad.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

//lcd(rs,e,d4,d5,d6,d7);
LiquidCrystal lcd(A5,A4,A3,A2,A1,A0);

//Keypad
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {    '1','2','3'    }
  ,
  {    '4','5','6'    }
  ,
  {    '7','8','9'    }
  ,
  {    '*','0','#'    }
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = {12,11,10,9 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = {8,7,6 }; 

// Create the Keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int key;//to store key values

//RFID only RX is used ie D2
SoftwareSerial rfid(2,3); //Create a 'fake' serial port. D2 is the Rx pin, D3 is the Tx pin.
/////////////////////////
int data1 = 0;
int ok=-1;
// define the tag numbers that can have access
int tagA[12] = {56,52,48,48,56,49,48,51,52,54,52,48}; //  Person A valid
int tagB[12] = {54,69,48,48,48,53,49,68,69,69,57,56}; //  Person B valid
int tagC[12] = {54,69,48,48,48,53,49,56,69,56,57,66}; // Person B, invalid

int newtag[12] = {0,0,0,0,0,0,0,0,0,0,0,0}; // used for read comparisons
/////////////////////////
byte tagfound=0;

char attempt[4]={
  0,0,0,0}; // used for comparison
int z=0;

char a;//from processing

void setup(){

  Serial.begin(9600);
  Serial.flush();

  rfid.begin(9600);//change if different
  rfid.flush();

  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Scan Card");
  establishContact();
}

void loop(){

  readTag();//access denied or granted
  //if serial from processing == 'k' read keypad
  readKeypad();
  if(Serial.available()>0){
    a=Serial.read();
    if(a=='Y'){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Access Granted");   
      delay(2000);
      lcd.clear();
      lcd.setCursor(0,0);        
      lcd.print("Scan Card");   
    }
    if(a=='N'){
      //pin incorrect
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Incorrect PIN");   
      delay(2000);
      lcd.clear();
      lcd.setCursor(0,0);        
      lcd.print("Scan Card");   
    }
    if(a=='X'){ //card invalid
      //pin incorrect
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Access Denied");   
      delay(2000);
      lcd.clear();
      lcd.setCursor(0,0);        
      lcd.print("Scan Card");   
    }
  }
}

/********FUNCTION PROTOTYPES*******************/
boolean comparetag(int aa[12], int bb[12])
//  compares two arrrays, returns true if identical - good for comparing tags
{
  boolean ff=false;
  int fg=0;
  for (int cc=0; cc<12; cc++)
  {
    if (aa[cc]==bb[cc])
    {
      fg++;
    }
  }
  if (fg==12)
  {
    ff=true;
  }
  return ff;
}
void checkmytags()
//compares each tag against the tag just read
{
  ok=0; // this variable helps decision making, if it is 1, we have a match, zero - a read but no match, -1, no read attempt made
  if (comparetag(newtag,tagA)==true)
  {
    ok++;
    tagfound=1;
    //send id to gui
  }
  if (comparetag(newtag,tagB)==true)
  {
    ok++;
    tagfound=2;
  }
  if (comparetag(newtag,tagC)==true){
    ok++;
    tagfound=3;

  }

}

void readTag() 
// poll serial port to see if tag data is coming in (i.e. a read attempt)
{
  ok=-1;
  if (rfid.available() > 0) // if a read has been attempted
  {
    // read the incoming number on serial RX
    delay(100);  // Needed to allow time for the data to come in from the serial buffer. 
    for (int z=0; z<12; z++) // read the rest of the tag
    {
      data1=rfid.read();
      newtag[z]=data1;
    }
    rfid.flush(); // stops multiple reads
    // now to match tags up
    checkmytags(); // compare the number of the tag just read against my own tags' number
  }
  //now do something based on tag type
  if (ok>0 == true) // if we had a match
  {
    approved();
    ok=-1;
  } 
  else if (ok == 0) // if we didn't have a match
  {
    notApproved();
    ok=-1;
  }

}

void approved()
// when an approved card is read
{
  if(tagfound==1){
    Serial.print("A");
  }
  if(tagfound==2){
    Serial.print("B");
  }
  if(tagfound==3){
    Serial.print("C");
  }
  tagfound=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter PIN");   
}

void notApproved()
// when an unlisted card is read
{
 // Serial.println("N");
  //Serial.println("Acces Denied");
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("Access Denied");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Scan Card");
}
////////////////////////////////////

void readKeypad()
{
  char key = keypad.getKey();
  if (key != NO_KEY)
  {
    switch(key)
    {
    case '*':
      z=0;
      Serial.print('*');//reset
      break;
    case '#':
      delay(100); // for extra debounce
      Serial.print('#'); //enter
      break;
    default:
      attempt[z]=key;
      Serial.print(key);
      z++;
    }
  }
}


//Wait for Processing to respond
void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print('D');   // send a capital D
    delay(300);
  }
}



