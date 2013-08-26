import processing.serial.*;

Serial port;                     //Define the variable port as a Serial object.

int whichKey = -1;  // Variable to hold keystoke values
int inByte = -1;    // Incoming serial data
int PIN_A[]= {'A', '2', '3', '4'}; // our secret (!) number
int PIN_B[]= {'B', '5', '6', '7'}; // our secret (!) number
int PIN_C[]= {'C', '8', '9', '0'}; // our secret (!) number
int attempt[]= {0, 0, 0, 0}; // used for comparison
int z=0;
int flag = 0;

int[] serialInArray = new int[4];    // Where we'll put what we receive
int serialCount = 0;                 // A count of how many bytes we receive
int x, y, f;		             // Starting position of the ball
boolean firstContact = false;        // Whether we've heard from the microcontroller
char id;

PImage person_A;
PImage person_B;
PImage person_C;
void setup() {
  person_A=loadImage("mypicture.jpg");
  person_B=loadImage("mypicture1.jpg");
  person_C=loadImage("mypicture2.jpg");
  
  
  size(700, 300);//l x h
  println(Serial.list());

  // I know that the first port in the serial list on my mac
  // is always my  FTDI adaptor, so I open Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
  String portName = Serial.list()[0];
  port = new Serial(this, portName, 9600);
  background(100);
}

void draw() {
 
  text("SVNIT Final Year Project,2013", 500, 290);
    
}//loop end

void keyPressed() {
  // Send the keystroke out:
  port.write(key);
  whichKey = key;
}

void serialEvent(Serial port) {
  // read a byte from the serial port:
  int inByte = port.read();
  // if this is the first byte received, and it's an A,
  // clear the serial buffer and note that you've
  // had first contact from the microcontroller. 
  // Otherwise, add the incoming byte to the array:
  if (firstContact == false) {
    if (inByte == 'D') { 
      port.clear();          // clear the serial port buffer
      firstContact = true;     // you've had first contact from the microcontroller
      //myPort.write('C');       // ask for more
    }
  } 
  else {
    
      if(inByte=='A'){
        //valid card detected of person A
        //read pin of A
        //display image
         background(100);
        image(person_A, 100, 0, person_A.width/2, person_A.height/2);
        println("Person A");
        text("Mr. Abhilash",10,130);
        text("Enter PIN",10,150);
        id='A';
        
//        port.write('X'); //invalid
//        port.clear();
//        text("Invalid User, Access Denied",10,150);
//        delay(2000);
//        background(100);

      }
      if(inByte=='B'){
        //Vaild card detected of person B
        // read pin of B
        background(100);
        image(person_B, 100, 0, person_A.width/2, person_A.height/2);
        println("Person B");
        text("Mr. Subash Gurjar",10,130);
        id='B';

//      text("Enter PIN",10,150);
        port.write('X'); //invalid
        port.clear();
        text("Invalid User, Access Denied",10,150);
        delay(2000);
        background(100);

      }
      if(inByte=='C'){
        //Vaild card detected of person B
        // read pin of B
        background(100);
        image(person_C, 100, 0, person_A.width/2, person_A.height/2);
        println("Person C");
        text("Mr. Ashish Saraf",10,130);
        id='C';

        text("Enter PIN",10,150);
//        port.write('X'); //invalid
//        port.clear();
//        text("Invalid User, Access Denied",10,150);
//        delay(2000);
//        background(100);
      }
    if(serialCount==0)    text("*",10,160);
    if(serialCount==1)    text("**",10,160);
    if(serialCount==2)    text("***",10,160);
    if(serialCount==3)    text("****",10,160);
    // Add the latest byte from the serial port to array:
    serialInArray[serialCount] = inByte;
    attempt[serialCount] = inByte;
    serialCount++;
    
    // If we have 3 bytes:
    if (serialCount > 3 ) {
      x = serialInArray[0];
      y = serialInArray[1];
      f = serialInArray[2];
      int tmp = serialInArray[3];
      // print the values (for debugging purposes only):
      // println(x + "\t" + y + "\t" + f + "\t" + tmp);
      checkPIN();
     // Send a capital A to request new sensor readings:
     //    port.write('A');
     // Reset serialCount:
      serialCount = 0;
    }
  }
}


void correctPIN() // do this if correct PIN entered
{
  //send Y
  port.write('Y');
  port.clear();
  //clear image
  background(100);
  text("Access Granted",10,150);
  delay(2000);
 background(100); 
}

void incorrectPIN() // do this if incorrect PIN entered
{
  //send N
  port.write('N');
  port.clear();
  text("Incorrect Pin",10,170);
  delay(2000);
  background(100);
  
}

void checkPIN()
{
  int correct=0;
  for (int q=0; q<4; q++)
  {
     if(id=='A'){
          if (attempt[q]==PIN_A[q] )
          {
            correct++;
          }
     }
     if(id=='B'){
          if (attempt[q]==PIN_B[q] )
          {
            correct++;
          }
     }
     if(id=='C'){
          if (attempt[q]==PIN_C[q] )
          {
            correct++;
          }
     }
    
  }
  if (correct==4)
  {
    correctPIN();
  } 
  else
  {
    incorrectPIN();
  }
  for (int zz=0; zz<4; zz++) // wipe attempt
  {
    attempt[zz]=0;
  }
}

