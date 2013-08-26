

// constants won't change. They're used here to 
// set pin numbers:
const int sensor1 = A5;     // the number of the pushbutton pin
const int sensor2 = A4;     // the number of the pushbutton pin
const int sensor3 = A3;     // the number of the pushbutton pin
const int sensor4 = A2;     // the number of the pushbutton pin

const int ledop1 =  5;      // the number of the LED pin
const int ledop2 =  6;      // the number of the LED pin
const int ledop3 =  7;      // the number of the LED pin
const int ledop4 =  8;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
int count = 0;
void setup() {
  pinMode(sensor1, INPUT);
  pinMode(ledop1, OUTPUT);
  
  pinMode(sensor2, INPUT);
  pinMode(ledop2, OUTPUT);
  
  pinMode(sensor3, INPUT);
  pinMode(ledop3, OUTPUT);
  
  pinMode(sensor4, INPUT);
  pinMode(ledop4, OUTPUT);
  
  digitalWrite(ledop1, HIGH);
  digitalWrite(ledop2, HIGH);
  digitalWrite(ledop3, HIGH);
  digitalWrite(ledop4, HIGH);
  
  Serial.begin(9600);
  Serial.println("init");
}

void loop() {

 count =1;
 readsensor(sensor1,ledop1);
 delay(500);

 count =2;
 readsensor(sensor2,ledop2);
 delay(500);

 count =3;
 readsensor(sensor3,ledop3);
 delay(500);

 count =4;
 readsensor(sensor4,ledop4);
 delay(500);
  
}

void readsensor(int buttonPin, int ledPin){
	// read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    buttonState = !reading;
  }
  
  // set the LED using the state of the button:
  digitalWrite(ledPin, buttonState);

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = !reading;


  Serial.print("reading sensor ");
//  Serial.print(buttonPin);
  Serial.print(count);
  Serial.print(" led ");
  Serial.print(ledPin);
  Serial.print(" State ");
  Serial.println(lastButtonState);


}
