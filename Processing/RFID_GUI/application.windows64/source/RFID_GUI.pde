import controlP5.*;              //import the Serial, and controlP5 libraries.
import processing.serial.*;

ControlP5 controlP5;             //Define the variable controlP5 as a ControlP5 type.
DropdownList ports;              //Define the variable ports as a Dropdownlist.
Serial port;                     //Define the variable port as a Serial object.
int Ss;                          //The dropdown list will return a float value, which we will connvert into an int. we will use this int for that).
String[] comList ;               //A string to hold the ports in.
boolean serialSet;               //A value to test if we have setup the Serial port.
boolean Comselected = false;     //A value to test if you have chosen a port in the list.

int whichKey = -1;  // Variable to hold keystoke values
int inByte = -1;    // Incoming serial data
int PIN[]= {'A', '2', '3', '4'}; // our secret (!) number
int attempt[]= {0, 0, 0, 0}; // used for comparison
int z=0;
int flag = 0;


int[] serialInArray = new int[4];    // Where we'll put what we receive
int serialCount = 0;                 // A count of how many bytes we receive
int x, y, f;		             // Starting position of the ball
boolean firstContact = false;        // Whether we've heard from the microcontroller
char id;

PImage person_A;
void setup() {
  person_A=loadImage("mypicture.jpg");
  
  size(700, 300);//l x h
  //I dont fully understand the library and the thing with this, but monkey see monky do ;) (oh yeah and it needs this line).
  controlP5 = new ControlP5(this);
  //Make a dropdown list calle ports. Lets explain the values: ("name", left margin, top margin, width, height (84 here since the boxes have a height of 20, and theres 1 px between each item so 4 items (or scroll bar).
  ports = controlP5.addDropdownList("list-1", 10, 25, 100, 84);
  //Setup the dropdownlist by using a function. This is more pratical if you have several list that needs the same settings.
  customize(ports);
    background(100);
}

//The dropdown list returns the data in a way, that i dont fully understand, again mokey see monkey do. However once inside the two loops, the value (a float) can be achive via the used line ;).
///////////////////////////////////////////////

void controlEvent(ControlEvent theEvent) {
  if (theEvent.isGroup()) 
  {
    //Store the value of which box was selected, we will use this to acces a string (char array).
    float S = theEvent.group().value();
    //Since the list returns a float, we need to convert it to an int. For that we us the int() function.
    Ss = int(S);
    //With this code, its a one time setup, so we state that the selection of port has been done. You could modify the code to stop the serial connection and establish a new one.
    Comselected = true;
  }
}

//here we setup the dropdown list.
void customize(DropdownList ddl) {
  //Set the background color of the list (you wont see this though).
  ddl.setBackgroundColor(color(200));
  //Set the height of each item when the list is opened.
  ddl.setItemHeight(20);
  //Set the height of the bar itself.
  ddl.setBarHeight(15);
  //Set the lable of the bar when nothing is selected.
  ddl.captionLabel().set("Select COM port");
  //Set the top margin of the lable.
  ddl.captionLabel().style().marginTop = 3;
  //Set the left margin of the lable.
  ddl.captionLabel().style().marginLeft = 3;
  //Set the top margin of the value selected.
  ddl.valueLabel().style().marginTop = 3;
  //Store the Serial ports in the string comList (char array).
  comList = port.list();
  //We need to know how many ports there are, to know how many items to add to the list, so we will convert it to a String object (part of a class).
  String comlist = join(comList, ",");
  //We also need how many characters there is in a single port name, we´ll store the chars here for counting later.
  String COMlist = comList[0];
  //Here we count the length of each port name.
  int size2 = COMlist.length();
  //Now we can count how many ports there are, well that is count how many chars there are, so we will divide by the amount of chars per port name.
  int size1 = comlist.length() / size2;
  //Now well add the ports to the list, we use a for loop for that. How many items is determined by the value of size1.
  for (int i=0; i< size1; i++)
  {
    //This is the line doing the actual adding of items, we use the current loop we are in to determin what place in the char array to access and what item number to add it as.
    ddl.addItem(comList[i], i);
  }
  //Set the color of the background of the items and the bar.
  ddl.setColorBackground(color(60));
  //Set the color of the item your mouse is hovering over.
  ddl.setColorActive(color(255, 128));
}

void startSerial(String[] theport)
{
  //When this function is called, we setup the Serial connection with the accuried values. The int Ss acesses the determins where to accsess the char array. 
  port = new Serial(this, theport[Ss], 9600);
  //Since this is a one time setup, we state that we now have set up the connection.
  serialSet = true;
}
//////////////////////////////////


void draw() {
  
  
  //So when we have chosen a Serial port but we havent yet setup the Serial connection. Do this loop
  while (Comselected == true && serialSet == false)
  {
    //Call the startSerial function, sending it the char array (string[]) comList
    startSerial(comList);
  }
  //set the background color of the sketch
//  background(100);

  //Do some stuff, for example, since the serial connection is set up (once the user has selected a port), sending/receive serial data ;).
  /////////////////////////////////////////////////
  //text("Last Received: " + inByte, 10, 130);
  //text("Last Sent: " + whichKey, 10, 100);
  text("SVNIT Final Year Project,2013", 500, 290);

  /////////////////////////////////////////////////////////////
  // if(flag==1){
  //     getPIN();
  //}
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
    if (inByte == 'C') { 
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
        id='A';

      }
      if(inByte=='B'){
        //Vaild card detected of person B
        // read pin of B
      }
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
      println(x + "\t" + y + "\t" + f + "\t" + tmp);
      checkPIN();

      // Send a capital A to request new sensor readings:
  //    port.write('A');
      // Reset serialCount:
      serialCount = 0;
    }
  }
}

//void keyPressed() {
//  // Send the keystroke out:
//  port.write(key);
//  whichKey = key;
//}

//Show image
//PImage img = loadImage("mypicture.jpg");
//image(img, 20, 10);
//img.x = 300; // set the x position of the image
//img.y = 300; // set the y position of the image

//////////////////////////



void correctPIN() // do this if correct PIN entered
{
  //send Y
  port.write('Y');
  port.clear();
  //clear image
  background(100);
  text("Access Granted",10,150);
  
 
}

void incorrectPIN() // do this if incorrect PIN entered
{
  //send N
  port.write('N');
  port.clear();
  text("Incorrect Pin",10,150);
  
}

void checkPIN()
{
  int correct=0;
  for (int q=0; q<4; q++)
  {
    if (attempt[q]==PIN[q])
    {
      correct++;
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

