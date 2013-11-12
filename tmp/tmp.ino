/*
 Set ID of each device, the GET
 Turn off ADC,UART,TWI
 
 
 ToDO:
 If room is booked send, from server side send booked for # days when the motion is detected
 AVR will power down for that many days.
 Response from server will be 
 $$OK - packet sent
 $$KO - error
 $$B4 - booked for 4 days -> sleep for 4 days
 Send battery low indication to server, and power down 
 */
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <PinChangeInt.h> //for disconnect button

//PIR sensor,led and disconnect button
#define PIR 2  //INT 0 PIN 4
#define DISCONNECT 19//A5 pulled up, not used for now
#define LED 16 //A2 high on
#define CCPWR 9 //gate of mosfet

#define ID 1 //ID of device, that will be linked to a room no. not being sent yet
/***************WIFI defines ************/
// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5 //pin 11
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
SPI_CLOCK_DIV2); // you can change this clock speed

#define WLAN_SSID       "HemalChevliOffice"           // cannot be longer than 32 characters!
#define WLAN_PASS       "blackcorp"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2


#define WEBSITE      "www.black-electronics.com"

/***************WIFI defines end ************/

volatile int pirState = LOW;             // we start, assuming no motion detected
volatile char sleep;
uint32_t ip;

char motion ='0';
char pass = 'p' ;

void static inline pwrDown(void);

void setup() {
  //wdt_enable(WDTO_8S);

Serial.begin(115200);
  Serial.println("RESET");
  ADCSRA = 0; //Disable ADC
  // turn off various modules
  //PRR =0x81; //0b10000001; TWI and ADC off
  // 7  1     0
  //TWI uart  ADC 
  pinMode(LED, OUTPUT);      // declare LED as output
  pinMode(PIR, INPUT);     // declare sensor as input
  pinMode(DISCONNECT,INPUT); //Disconnect button
  digitalWrite(DISCONNECT,HIGH); //pulled up

//  connectToAP();

  Serial.println("Motion detector Ready!");
  delay(100);
  attachInterrupt(0, motiondetect, RISING); 
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); 

  //   wdt_enable (WDTO_8S);
  //    Serial.println("reset");
  cli();
  pwrDown();
}

void loop(){ 
  Serial.println("start");
  if (pirState == 1){

   /* //cli();
    digitalWrite(LED,HIGH);
    //motion detected
    pirState=0;
    wdt_reset();
    delay(1000);
    Serial.println("Motion detected!");

    connectToAP();
    //send packet only when motin is detected
    motion='1';
    String m = String(motion);
    String request = "/save.php?m=1&p=p" ;
    sendRequest(request);
    digitalWrite(LED,LOW);
    disconnect();*/
    Serial.println("Sent - Ready");
    wdt_reset();
    sei();
  }
  else{
    pwrDown();
  }
  Serial.println("stop");
  delay(100);
}

//ISR for INT0 when motion is detected
void motiondetect() {
  pirState = 1;
}

//Put AVR to sleep
void static pwrDown(void)
{   
  //PRR =0xFF; //turn off all modules 
  MCUCR = MCUCR | bit(BODSE) | bit(BODS); //timed sequence
  MCUCR = MCUCR & ~bit(BODSE) | bit(BODS);//to turn off BOD in sleep
  //Disable WDT
  wdt_disable();
  sei(); //enable int
  sleep_mode(); //sleeping set SE got sleep and clear SE when waking up

  //sleep_enable(); //slee_mode() does all three.
  //sleep_cpu();
  //sleep_disable();

  //PRR =0x81; //0b10000011; turn off all except uart and spi
  // sleep = 0;
  //Enable WDT as soon as it wakes up
  wdt_enable(WDTO_8S);
} 
/*
void WDT_off(void)
 {
 cli();  
 __watchdog_reset();
 // Clear WDRF in MCUSR 
 MCUSR &= ~(1<<WDRF);
 // Write logical one to WDCE and WDE 
 // Keep old prescaler setting to prevent unintentional time-out 
 WDTCSR |= (1<<WDCE) | (1<<WDE);
 //Turn off WDT 
 WDTCSR = 0x00;
 __enable_interrupt();
 }*/


/**************************************************************************/
/*!
 @brief  Disconnects from the AP
 */
/**************************************************************************/
void disconnect(void){
  /* You need to make sure to clean up after yourself or the CC3000 can freak out */
  /* the next time your try to connect ... */
  Serial.println(F("\n\nDisconnecting"));
  cc3000.disconnect();
wlan_stop();
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
  Serial.print(WEBSITE); 
  Serial.print(F(" -> "));
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
    //    www.fastrprint(requestBuf);
    www.fastrprint("/save.php?m=1&p=p");
    www.fastrprint(F(" HTTP/1.0\r\n"));
    www.fastrprint(F("Host: ")); 
    www.fastrprint(WEBSITE); 
    www.fastrprint(F("\n"));
    www.fastrprint(F("Connection: close\n"));
    www.fastrprint(F("\n"));
    www.println();
  } 
  else {
    Serial.println(F("Connection failed"));    
    return;
  }

  Serial.println(F("\n-------------------------------------"));

  while (www.connected()) {
    while (www.available()) {
      char c = www.read();
      Serial.print(c);
    }
  }
  www.close();
  Serial.println(F("\n-------------------------------------"));
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

  Serial.print(F("Networks found: ")); 
  Serial.println(index);
  Serial.println(F("================================================"));

  while (index) {
    index--;

    valid = cc3000.getNextSSID(&rssi, &sec, ssidname);

    Serial.print(F("SSID Name    : ")); 
    Serial.print(ssidname);
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
    Serial.print(F("\r\nIP Addr: ")); 
    cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\r\nNetmask: ")); 
    cc3000.printIPdotsRev(netmask);
    Serial.print(F("\r\nGateway: ")); 
    cc3000.printIPdotsRev(gateway);
    Serial.print(F("\r\nDHCPsrv: ")); 
    cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\r\nDNSserv: ")); 
    cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}

void connectToAP(void){
  wdt_reset();
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  wdt_reset();
  // Optional SSID scan
  // listSSIDResults();

  cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY);
  wdt_reset();
  Serial.println(F("Connected!"));

  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  
  wdt_reset();
  Serial.println(F("DONE"));
  delay(100);
  /* Display the IP address DNS, Gateway, etc. */
  while (! displayConnectionDetails()) {
    delay(1000);
  }
  wdt_reset();

}







