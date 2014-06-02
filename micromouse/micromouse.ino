http://timsengineeringblog.weebly.com/micromouse.html

int leftping = 2;  //left ping pin
int frontping = 3;  //front ping pin
int rightping = 8;  //right ping pin
int M2Dirpin = 4; // Motor 2 Direction control
int M2Spdpin = 5; // Motor 2 PWM control
int M1Dirpin = 6; // Motor 1 PWM control
int M1Spdpin = 7; // Motor 1 Direction control
int M1Speed = 100; // PWM value (0-255)
int M2Speed = 100; // PWM value (0-255)

void setup() {
  int i;
  for(i=5;i<=8;i++) //For Arduino Motor Shield 
  pinMode(i, OUTPUT); //set pin 4,5,6,7 to output mode
  
  Serial.begin(9600); //go to tools/serial moniter 
}

void loop()
{
  long leftduration, leftinches, leftcm, fronttduration, 
       frontinches, frontcm, rightduration, rightinches, rightcm;
       
  char state;  //'l' left, 'r' right, 'f' forward, 's' stop

//***************************************************************
  //PING SENSORS
  //Left Sensor
  pinMode(leftping, OUTPUT);
  digitalWrite(leftping, LOW);
  delayMicroseconds(2);
  digitalWrite(leftping, HIGH);
  delayMicroseconds(5);
  digitalWrite(leftping, LOW);
  pinMode(leftping, INPUT);
  leftduration = pulseIn(leftping, HIGH);

  //Front Sensor
  pinMode(frontping, OUTPUT);
  digitalWrite(frontping, LOW);
  delayMicroseconds(2);
  digitalWrite(frontping, HIGH);
  delayMicroseconds(5);
  digitalWrite(frontping, LOW);
  pinMode(frontping, INPUT);
  fronttduration = pulseIn(frontping, HIGH);
  
  //Right Sensor
  pinMode(rightping, OUTPUT);
  digitalWrite(rightping, LOW);
  delayMicroseconds(2);
  digitalWrite(rightping, HIGH);
  delayMicroseconds(5);
  digitalWrite(rightping, LOW);
  pinMode(rightping, INPUT);
  rightduration = pulseIn(rightping, HIGH);

//***************************************************************
  // Convert the time into a distance
  //Left
  leftinches = microsecondsToInches(leftduration);
  //leftcm = microsecondsToCentimeters(duration);
  
  //Front
  frontinches = microsecondsToInches(fronttduration);
  //frontcm = microsecondsToCentimeters(fronttduration);
  
  //Right
  rightinches = microsecondsToInches(rightduration);
  //rightcm = microsecondsToCentimeters(rightduration);
  

//************************************************************
// Uncomment to display ping values in serial monitor
// To use serial monitor click tools/serialmonitor
// To uncomment, highlight code below and right click to 
// select uncomment
  
  Serial.print("Left: ");
  Serial.print(leftinches);
  Serial.print(", Front: ");
  Serial.print(frontinches);
  Serial.print(", Right: ");
  Serial.print(rightinches);
  Serial.println();
//  delay(100);

//*************************************************************
//MAZE CONTROL PROGRAM HERE

if (leftinches < 2)
  state = 'l';
if (rightinches < 2)
  state = 'r';  
if (frontinches < 2)
  state = 'f';
if (leftinches > 2 && rightinches > 2 && frontinches > 2)
  state = 's';
  
//*************************************************************
//MOTOR CONTROL
//The speed control is achieved through the conventional PWM 
//which can be obtained from Arduino’s PWM output Pins 5 and 6. 
//The enable/disable function of the motor control is signalled 
//by Arduino Digital Pins 7 and 8. 

//Digital 4: Motor 2 Direction control
//Digital 5: Motor 2 PWM control
//Digital 6: Motor 1 PWM control
//Digital 7: Motor 1 Direction control
//set pwm control, 0 for stop, and 255 for maximum speed
  switch(state) 
    { 
      case 'f': //forward
         Motor1(M1Speed,true);
         Motor2(M2Speed,true); 
         break; 
       
      case 'l'://turn left
         Motor1(M1Speed,false); 
         Motor2(M2Speed,true); 
         break; 
       
      case 'r'://turn right 
         Motor1(M1Speed,true); 
         Motor2(M2Speed,false); 
         break;
         
      case 's'://stop 
         Motor1(0,false); 
         Motor2(0,false); 
         break;
    }
    
 //END OF PROGRAM
 //********************************************************
}

void Motor1(int pwm, boolean reverse) 
  { 
    analogWrite(M1Spdpin,pwm); 
    if(reverse) 
      { 
        digitalWrite(M1Dirpin,HIGH); 
      } 
    else 
      { 
        digitalWrite(M1Dirpin,LOW); 
      } 
    } 

void Motor2(int pwm, boolean reverse) 
  { 
    analogWrite(M2Spdpin,pwm); 
    if(reverse) 
      { 
        digitalWrite(M2Dirpin,HIGH); 
      } 
    else 
      { 
        digitalWrite(M2Dirpin,LOW); 
      }
  }
  
long microsecondsToInches(long microseconds)
{
   return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}
