/*
   SonMicro RFID Reader uploader example
 Language: Processing
 
 This sketch reads serial data from a microcontroller
 attached to a  SonMicroReader RFID reader. It can read tags, clear the
 controller's database, or upload the tags to a URL.
 
 created 5 Feb 2009
 by Tom Igoe
 */

// import the serial library:
import processing.serial.*;

Serial myPort;                // the serial port
int fontHeight = 14;          // font for drawing on the screen
String messageString;         // the main display string
int lineCount = 0;            // count of lines in messageString
int maxLineCount = 5;         // largest that lineCount can be

String tagsToUpload = "";     // CSV string of hex-encoded RFID tags

boolean identifyingSelf = false;  // whether you're scanning your own tag
String myTagId = "";              // your tag ID 

void setup() {
  // set  the window size:
  size(600,400);
  // list all the serial ports:
  println(Serial.list()); 

  // based on the list of serial ports printed from the 
  //previous command, change the 0 to your port's number:
  String portnum = Serial.list()[0];
  // initialize the serial port:
  myPort = new Serial(this, portnum, 9600);
  // clear the serial buffer:
  myPort.clear();
  // only generate a serialEvent() when you get a newline:
  myPort.bufferUntil('\n');

  // create a font with the second font available to the system:
  PFont myFont = createFont(PFont.list()[2], fontHeight);
  textFont(myFont);

  // initalize the message string:
  messageString = "waiting for reader to reset\n"; 
  // make the UI buttons:
  makeButtons();

  // get the user's ID if it's saved:
  String[] savedData = loadStrings("yourID.txt");
  if (savedData != null) {
    if (savedData.length > 0) {
      myTagId = savedData[0]; 
    }
  }
}

void draw() {
  // clear the screen:
  background(0);  
  // draw the UI buttons:
  drawButtons(); 
  // show the last obtained user profile:
  showProfile();
  // draw the message string and tags to upload:
  textAlign(LEFT);
  text("Your tag ID:" + myTagId, 10, 30);
  text(messageString, 10, 50,300, 130);
  text("Tags to upload:", 10, 220);
  text(tagsToUpload, 10, 240,500, 130);
}

void serialEvent(Serial myPort) {
  // read the serial buffer:
  String inputString = myPort.readStringUntil('\n');

  // if there's something there, act:
  if (inputString != null) {
    // see if the newest line contains an RFID tag:
    String newTag = parseForTag(inputString);

    // if you got a tag, add it to the list to upload:
    if (newTag != null) {
      if (identifyingSelf) {
        myTagId = newTag;
        identifyingSelf = false;
        String[] dataToSave = new String[1];
        dataToSave[0] = myTagId;
        saveStrings("yourID.txt", dataToSave);
      } 
      else {
        // add a comma if there's already text in the string:
        if (tagsToUpload != "") {
          tagsToUpload += ",";
        }
        tagsToUpload += newTag; 
      }   
    }

    // display the incoming lines, 5 lines at a time:
    if (lineCount < maxLineCount) {
      messageString +=  inputString;
      lineCount++;
    } 
    else {
      messageString = inputString;
      lineCount = 0;
    }
  }
}

// tag strings come in the format 1:AA4D3C67, where
// the first number is the record number in reader memory
// and the string after the colon is the tag ID

String parseForTag(String thisString) {
  String thisTag = null;
  // separate the string on the colon:
  String[] tagElements = split(thisString, ":");
  // if you have at least 2 elements, extract the parts:
  if (tagElements.length > 1) {
    // get the record number:
    int recordNumber = int(tagElements[0]);

    // if the tag ID is not "0000", get it:
    if (!tagElements[1].substring(0,4).equals("0000")) {
      thisTag = tagElements[1];
      thisTag = trim(thisTag);
    }
  }
  return thisTag;
}

void buttonPressed(RectButton thisButton) {
  // get the button number from the button passed to you:
  int buttonNumber = buttons.indexOf(thisButton);

  // do different things depending on the button number:
  switch (buttonNumber) {
  case 0: // get tags from reader:
    tagsToUpload = "";
    myPort.write("p");
    break;
  case 1:  // upload tags to net:
    if (tagsToUpload.equals("")) {
      messageString = "No tags to upload."; 
    } 
    else {
      String[] theseTags = split(tagsToUpload, ",");
      for (int thisTag = 0; thisTag< theseTags.length; thisTag++) {
        makeRequest(theseTags[thisTag]);
      }

      // after you upload, clear tagsToUpload:
      tagsToUpload = "";
    }
    break; 
  case 2:   // delete tags from reader:
    messageString = "deleting reader database\n";
    myPort.write("c");
    break;
  case 3:    // scan your own tag
    identifyingSelf = true;
    messageString = "Waiting for your personal tag";
  }
}

