/*
This file contains functions to take the tag and look it up on O'Reilly's database
 */

// the URL of the PHP script that passes the tag to the database:
String myUrl = "http://tigoe.net/workshops/etech09/lookupTag.php?tag=";

PImage photo;        // String containing the photo URL
String[] profile;    // String array containing the HTML of the profile
String username;     // String of the username
String affiliation;  // String of the affiliation
String twitter;      // String of the twitter username
String tagNumber;    // String of the user tag number
String country;      // String of the country

int profileX = 350;  // horizontal position of the profile
int profileY = 150;  // vertical position of the profile

// This method makes the HTTP request to the PHP script
// that calls the O'Reilly database and stores it 
// in a file:
void makeRequest(String whichTag) {
  // make HTTP call:
  String thisUrl = myUrl + whichTag;
  // save the resulting file in an array:
  String[] httpRequest = loadStrings(thisUrl); 
  saveStrings("person.xml", httpRequest);
  // parse the results:
  parseRecord("person.xml");
} 

void parseRecord(String filename) {
  // get the first line, make sure it's an XML file.
  // if not, skip the rest of the substring and return:
  String firstLine = loadStrings(filename)[0];
  if (!(firstLine.substring(0, 5).equals("<?xml"))) {
    // clear the profile variables:
    photo = null;
    profile = null;
    username = null;
    affiliation= null;
    twitter = null;
    tagNumber = null;
    country = null;
    return; 
  }

  // open the XML record:
  XMLElement xml = new XMLElement(this, filename);
  int lines = xml.getChildCount();
  // parse the record line by line:
  for (int i = 0; i < lines; i++) {
    XMLElement thisRecord = xml.getChild(i);
    String fieldName = thisRecord.getName(); 
    String content = thisRecord.getContent();

    if (fieldName.equals("photo")) {
      photo = loadImage(content);
    }   

    if (fieldName.equals("profile")) {
      profile = loadStrings(content);
    }    

    if (fieldName.equals("name")) {
      username = content;
    }      

    if (fieldName.equals("affiliation")) {
      affiliation = content;
    }   

    if (fieldName.equals("twitter")) {
      twitter = content;
    }         
    if (fieldName.equals("rfid")) {
      tagNumber = content;
    }    
    if (fieldName.equals("country")) {
      country = content;
    }        
  }
}
// this method displays the profile onscreen:
void showProfile() {
    // text color:
  fill(0);
  int  lineNumber = profileY;
  textAlign(LEFT);

  // show profile results if the profile variables are populated:
  if (photo != null) {
    image(photo, profileX, lineNumber);
    lineNumber = lineNumber + 130;
  }

  // not displaying profile because it's all HTML and I'm too lazy to 
  // strip out  the bio div. It's a good goal for someone else
  if (username != null) {
    text(username, profileX, lineNumber);
    // increment the line vertical position:
    lineNumber = lineNumber + fontHeight+4;
  }
  if (affiliation != null) {
    text(affiliation, profileX, lineNumber, width - profileX, 40);
    // increment the line vertical position:
    lineNumber = lineNumber + 3*(fontHeight+4);
  }
  if (twitter != null) {
    text(twitter, profileX, lineNumber);
    // increment the line vertical position:
    lineNumber = lineNumber + fontHeight+4;
  }
  if (tagNumber != null) {
    text(tagNumber, profileX, lineNumber);
    // increment the line vertical position:
    lineNumber = lineNumber + fontHeight+4;
  }
  if (country != null) {
    text(country, profileX, lineNumber);
    // increment the line vertical position:
    lineNumber = lineNumber + fontHeight+4;
  }
}


