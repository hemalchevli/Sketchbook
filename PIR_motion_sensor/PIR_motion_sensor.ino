/*************************************************** 
  This is an example for the Adafruit CC3000 Wifi Breakout & Shield

  Designed specifically to work with the Adafruit WiFi products:
  ----> https://www.adafruit.com/products/1469

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
 
 /*
This example does a test of the TCP client capability:
  * Initialization
  * Optional: SSID scan
  * AP connection
  * DHCP printout
  * DNS lookup
  * Optional: Ping
  * Connect to website and print out webpage contents
  * Disconnect
SmartConfig is still beta and kind of works but is not fully vetted!
It might not work on all networks!
*/
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2); // you can change this clock speed

#define WLAN_SSID       "HemalChevliOffice"           // cannot be longer than 32 characters!
#define WLAN_PASS       "blackcorp"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

// What page to grab!
#define WEBSITE      "www.black-electronics.com"
//#define WEBPAGE      "/save.php?m=1&p=p"

//PIR sensor,led and disconnect button
#define PIR 18 //A3
#define DISCONNECT 19//A5 pulled up
#define LED 16 //A2 high on

int val = 0;
int pirState = LOW;

char motion = '0'; //convert to int or not
char pass = 'p' ;

/**************************************************************************/
/*!
    @brief  Sets up the HW and the CC3000 module (called automatically
            on startup)
*/
/**************************************************************************/

uint32_t ip;

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Hello, CC3000!\n")); 

  displayFreeRam();
  //Init IO
  pinMode(PIR,INPUT);
  pinMode(DISCONNECT,INPUT);
  pinMode(LED,OUTPUT);
  
  digitalWrite(DISCONNECT,HIGH);
  
  
  /* Initialise the module */
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  
  // Optional SSID scan
  // listSSIDResults();
  
  cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY);
   
  Serial.println(F("Connected!"));
  
  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  /* Display the IP address DNS, Gateway, etc. */  
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  
}

void loop(void)
{
   ///////////////////////////////////

//add WDT

//  String m = String(motion);
//  String request = "save.php?m="+m+"&p=p" ;
 val = digitalRead(PIR);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    digitalWrite(LED, HIGH);  // turn LED ON
    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      motion='1';
      String m = String(motion);
      String request = "save.php?m="+m+"&p=p" ;
      sendRequest(request);
      // We only want to print on the output change, not state
      pirState = HIGH;
    }
  } else {
    digitalWrite(LED, LOW); // turn LED OFF
    if (pirState == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      motion = '0';
      String m = String(motion);
      String request = "save.php?m="+m+"&p=p" ;
      sendRequest(request);
      // We only want to print on the output change, not state
      pirState = LOW;
    }
  }
}

/**************************************************************************/
/*!
    @brief  Disconnects from the AP
*/
/**************************************************************************/
//ToDO add button to disconnect
void disconnect(void){
  /* You need to make sure to clean up after yourself or the CC3000 can freak out */
  /* the next time your try to connect ... */
   Serial.println(F("\n\nDisconnecting"));
  cc3000.disconnect();
}
/**************************************************************************/
/*!
    @brief  Sends a GET request to server and analyse the response
*/
/**************************************************************************/
void sendRequest(String request){ //to do int sendRequest

      // Transform to char
    char requestBuf[request.length()];
    request.toCharArray(requestBuf,request.length()); 
   
    ip = 0;
  // Try looking up the website's IP address
  Serial.print(WEBSITE); Serial.print(F(" -> "));
  while (ip == 0) {
    if (! cc3000.getHostByName(WEBSITE, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }

  cc3000.printIPdotsRev(ip);
   /* Try connecting to the website */
  Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 80);
  if (www.connected()) {
    www.fastrprint(F("GET "));
//    www.fastrprint(WEBPAGE);
    www.fastrprint(requestBuf);
    www.fastrprint(F(" HTTP/1.0\r\n"));
    www.fastrprint(F("Host: ")); www.fastrprint(WEBSITE); www.fastrprint(F("\n"));
    www.fastrprint(F("Connection: close\n"));
    www.fastrprint(F("\n"));
    www.println();
  } else {
    Serial.println(F("Connection failed"));    
    return;
  }

  Serial.println(F("-------------------------------------"));

  while (www.connected()) {
    while (www.available()) {
      char c = www.read();
      Serial.print(c);
    }
  }
  www.close();
  Serial.println(F("-------------------------------------"));
  free(requestBuf);
}

/**************************************************************************/
/*!
    @brief  Begins an SSID scan and prints out all the visible networks
*/
/**************************************************************************/

void listSSIDResults(void)
{
  uint8_t valid, rssi, sec, index;
  char ssidname[33]; 

  index = cc3000.startSSIDscan();

  Serial.print(F("Networks found: ")); Serial.println(index);
  Serial.println(F("================================================"));

  while (index) {
    index--;

    valid = cc3000.getNextSSID(&rssi, &sec, ssidname);
    
    Serial.print(F("SSID Name    : ")); Serial.print(ssidname);
    Serial.println();
    Serial.print(F("RSSI         : "));
    Serial.println(rssi);
    Serial.print(F("Security Mode: "));
    Serial.println(sec);
    Serial.println();
  }
  Serial.println(F("================================================"));

  cc3000.stopSSIDscan();
}

/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\r\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\r\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\r\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\r\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\r\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}


