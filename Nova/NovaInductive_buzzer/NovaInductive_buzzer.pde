// constants won't change. They're used here to 
// set pin numbers:
//Sensor input from collector of 547
#define debug true

const int sensor1 = A5;     // inductive sensor
const int sensor2 = A4;     // the number of the pushbutton pin
const int sensor3 = A3;     // the number of the pushbutton pin
const int sensor4 = A2;     // the number of the pushbutton pin

const int led1 =  5;      // the number of the LED pin
const int led2 =  6;      // the number of the LED pin
const int led3 =  7;      // the number of the LED pin
const int led4 =  8;      // the number of the LED pin

const int buzzer = 2;

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
  pinMode(led1, OUTPUT);

  pinMode(sensor2, INPUT);
  pinMode(led2, OUTPUT);

  pinMode(sensor3, INPUT);
  pinMode(led3, OUTPUT);

  pinMode(sensor4, INPUT);
  pinMode(led4, OUTPUT);

  pinMode(buzzer, OUTPUT);

  digitalWrite(led1, HIGH); //Active Low logic
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  digitalWrite(led4, HIGH);

  if(debug){
    Serial.begin(9600);
    Serial.println("init");
  }
}

void loop() {

  count = 1;
  readsensor(sensor1,led1);
  delay(100);

  count++;
  readsensor(sensor2,led2);
  delay(100);

  count++;
  readsensor(sensor3,led3);
  delay(100);

  count++;
  readsensor(sensor4,led4);
  delay(100);

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
    //Reading OK
    buttonState = !reading;//because of the active low logic, input high(sensor ok) LED is off, input low(sensor detected) LED ON.
  }

  // set the LED using the state of the button:
  digitalWrite(ledPin, buttonState);

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = !reading;

  //Buzzer
  if(buttonState == LOW){

    digitalWrite(buzzer, HIGH);
  }
  else{
    digitalWrite(buzzer,LOW);
  }


  if(debug){
    Serial.print("reading sensor ");
    //  Serial.print(buttonPin);
    Serial.print(count);
    Serial.print(" led ");
    Serial.print(ledPin);
    Serial.print(" State ");
    Serial.println(lastButtonState);
  }

}


