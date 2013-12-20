#include <EEPROM.h>
#include <MP3.h>
#include <Wire.h>
#include <gLCD.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

//type 0 = Prototype Full Flash buffering
//type 1 = Full Flash buffering
//type 2 = FeRAM semibuffering
//type 4 = Prototype FeRAM semibuffering
#define MEMTYPE 2 //Must be defined, and defined BEFORE the variables.h

#define TIMEOUT 60 //Number of seconds before timeout.

//Contains all the variables used in this program - there are ALOT and doing this makes it easier to find stuff
#include <variables.h> //It is stored in the /MP3/ library folder
  
gLCD graphic(8,9,7,6,1); //variables: RS pin,CS pin,CLK pin,DATA pin, use fast digitalWrite
//Offset for co-ordinate system, as some pixels are not visible. These allow (0,0) to be the first visible pixel
char Xzero = 0;
char Yzero = 2;

//Startup Code -------------------------------------------------------------------------------------------------------------
void setup() {
  //Screen Setup-------------------------------------------
  pinMode(BacklightOnOffPin,OUTPUT);
  pinMode(BacklightPWMPin,OUTPUT);
  graphic.Init(Xzero,Yzero,1);
  Serial.begin(115200);
  //Read the last contrast and brightness settings
  brightness = EEPROM.read(1); //Retreive PWM duty cycle for normal brightness
  dimmedBrightness = EEPROM.read(2); //Retreive PWM duty cycle for power saving
  contrast = EEPROM.read(3) - 61;//Value was stored in EEPROM with 61 added to it to preserve the '-' sign
  //Check that contrast is in range. if not change it to be - This also acts to program default values into a virgin EEPROM
  if (contrast == 194){ //255 - 61 = 194, so if EEPROM blank, then the default value is used
    if(graphic._Phillips){
      contrast = 0x30;
    } else {
      contrast = 0x2B;
    } 
    EEPROM.write(3, contrast + 61);
  } else if (contrast > 61){
    contrast = 61;
    EEPROM.write(3, contrast + 61);
  } else if ((graphic._Phillips)&&(contrast < -61)){
    contrast = -61;
    EEPROM.write(3, contrast + 61);
  } else if ((!graphic._Phillips)&&(contrast < 0)){
    contrast = 0;
    EEPROM.write(3, contrast + 61);
  }
  
  //Set contrast and brightness
  graphic.Contrast(contrast);
  digitalWrite(BacklightOnOffPin,LOW); //Enable the voltage boost circuit (Active LOW)
  analogWrite(BacklightPWMPin,brightness); //Normal brightness to begin with.
  
  //--------------------------------------------------------
  if(!initialiseComms()){ //If initialisation failed
    initFailure(); //BSOD then shut down
  }
}
//--------------------------------------------------------------------------------------------------------------------------

//Initialisation Code-------------------------------------------------------------------------------------------------------
void initFailure(){
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[22])));
  graphic.Print(stringRecover,0,88,4);
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[23])));
  graphic.Print(stringRecover,0,96,4);
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[24])));
  graphic.Print(stringRecover,0,104,4);
  delay(10000);
  goToSleep(); //Shutdown
}

boolean initialiseComms(){
  byte line = 0;
  
  //SPI Select Outputs
  pinMode(DecoderPin,OUTPUT);
  digitalWrite(DecoderPin,HIGH);
  delay(10);
  digitalWrite(DecoderPin,LOW);
  pinMode(SDCardPin,OUTPUT);
  digitalWrite(SDCardPin,HIGH);
  pinMode(CardDetectPin,INPUT);

  //Interrupt Input Pins
  pinMode(DataIntPin,INPUT);
  pinMode(PhonesIntPin,INPUT);
  
  //Tracker Ball Outputs
  pinMode(A9,OUTPUT);
  digitalWrite(A9,LOW); //GND - For prototyping only
  pinMode(A8,OUTPUT);
  digitalWrite(A8,HIGH); //VCC - For prototyping only
  pinMode(LedPin,OUTPUT);
  digitalWrite(LedPin,LOW); //LED

  //Tracker Ball Inputs
  pinMode(ButtonPin,INPUT);
  pinMode(RightPin,INPUT);
  pinMode(LeftPin,INPUT);
  pinMode(DownPin,INPUT);
  pinMode(UpPin,INPUT);

  //Mask out pin change interrupts for the trackerball
  PCMSK2 = 0;
  PCMSK2 |= (1 << PCINT18); //Button
  PCMSK2 |= (1 << PCINT19); //Right
  PCMSK2 |= (1 << PCINT20); //Left
  PCMSK2 |= (1 << PCINT21); //Down
  PCMSK2 |= (1 << PCINT22); //Up

  //Initialise all components, and check that they have worked correctly
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[0])));
  graphic.Print(stringRecover,0,line,4);
  line += 8;

  delay(100);
  digitalWrite(DecoderPin,HIGH);

  //Firstly we need to check that the MP3 decoder IC is with us and connected. For this we need to enable I2C communication
  decoder.Read(1,1); //Reads decoder ID

  if((decoder.Buffer[0] != 0xAC)||(decoder.Buffer[8] > 1)||(decoder.Buffer[8] == 0)){
    //These are cases which mean the decoder is either not present or not working properly
    //0xAC should be returned when reading address 1. Only ONE byte should be returned.
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[1])));
    graphic.Print(stringRecover,0,line,4);
    line += 8;
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[2])));
    graphic.Print(stringRecover,0,line,4);
    line += 8;
    return 0; //Setup failure
  }
  //Decoder is there, so we need to configure it
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[3])));
  graphic.Print(stringRecover,0,line,4);
  line += 8;
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[4])));
  graphic.Print(stringRecover,0,line,4);
  line += 8;
  if (!decoder.Config()){
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[5])));
    graphic.Print(stringRecover,0,line,4);
    line += 8;
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[2])));
    graphic.Print(stringRecover,0,line,4);
    line += 8;
    return 0; //Setup failure
  }
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[6])));
  graphic.Print(stringRecover,0,line,4);
  line += 8;

  char strPrint[30];
  
  decoder.Read(15,1); //Reads decoder ID
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[7])));
  sprintf(strPrint,"%s%d",stringRecover,decoder.Buffer[0]);
  
  decoder.Read(0,1); //Reads decoder ID
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[15])));
  sprintf(strPrint,"%s%s%d",strPrint,stringRecover,decoder.Buffer[0]);
  decoder.Read(13,1); //Reads decoder ID
  sprintf(strPrint,"%s%s%d",strPrint,stringRecover,decoder.Buffer[0]);
  decoder.Read(85,1); //Reads decoder ID
  sprintf(strPrint,"%s%s%d",strPrint,stringRecover,decoder.Buffer[0]);
  decoder.Read(113,1); //Reads decoder ID
  sprintf(strPrint,"%s%s%d",strPrint,stringRecover,decoder.Buffer[0]);
  graphic.Print(strPrint,0,line,4);
  line += 8;

  //Sets the decoder clock running (mute), but its not playing (stop).
  decoder.Buffer[0] = 0; //Data to send: Stop
  decoder.Buffer[8] = 1; //How many bytes: 1
  decoder.Write(19); //Write data to register
  decoder.Buffer[0] = 1; //Data to send: Mute
  decoder.Buffer[8] = 1; //How many bytes: 1
  decoder.Write(20); //Write data to register
  memory.writeCLK(0);
  memory.writeDataEn(1);//Disable data input of the decoder

  if(digitalRead(CardDetectPin)){
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[8])));
    graphic.Print(stringRecover,0,line,4);
    while(digitalRead(CardDetectPin));
    delay(2000);
  }

  //Next all the SPI bus components need to be checked, starting with out data source, the SD Card
  if(!SD.begin(SDCardPin)){
    //The SD card cannot be found, therefore starting the media player would be pointless
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[9])));
    graphic.Print(stringRecover,0,line,4);
    line += 8;
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[2])));
    graphic.Print(stringRecover,0,line,4);
    line += 8;
    return 0; //Setup failure
  }
  //If an SD card is present, then we can continue
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[10])));
  graphic.Print(stringRecover,0,line,4);
  line += 8;

  //Now we need to make sure there is flash memory available to store frame data too.
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[11])));
  graphic.Print(stringRecover,0,line,4);
  line += 8;
  memory.begin(FlashPin,Flash2Pin,HoldPin,Hold2Pin,DataEnable,SPI_CLOCK_DIV2,MEMTYPE); //CS Pin 1, CS Pin 2, Hold Pin 1, Hold Pin 2, Decoder CS Pin, Clock speed, 
                                                                                       //Memory Type (0 = S25FL064P, 1 = SST25VF064C, 2 = SRAM)
#if (MEMTYPE == 0) || (MEMTYPE == 100)
  memory.Command(FLASH_RDSR,0);
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[12])));
  sprintf(strPrint,"%s%d",stringRecover,memory.flashData[256]);

  memory.Command(FLASH_RDSR,1);
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[13])));
  sprintf(strPrint,"%s%s%d",strPrint,stringRecover,memory.flashData[256]);

  graphic.Print(strPrint,0,line,4);
  line += 8;
  delay(500);

  memory.Command(FLASH_RDID,0);
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[14])));
  sprintf(strPrint,"%s%d",stringRecover,memory.flashData[0]);

  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[15])));
  sprintf(strPrint,"%s%s%d%s%d%s%d%s%d",strPrint,stringRecover,memory.flashData[1],stringRecover,memory.flashData[2],stringRecover,memory.flashData[3],stringRecover,memory.flashData[5]);

  graphic.Print(strPrint,0,line,4);
  line += 8;
  delay(500);
#endif
#if MEMTYPE == 100
  //The NOR flash's ID is always 0xBF. If no response is seen on the SPI, then the command will return 0, so we know whether it has responed or not
  if (memory.flashData[0] != 0xBF){
    //If it cant be found, something is wrong and the media player cannot start.
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[16])));
    graphic.Print(stringRecover,0,line,4);
    line += 8;
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[2])));
    graphic.Print(stringRecover,0,line,4);
    line += 8;
    return 0; //Setup failure
  }
#elif MEMTYPE == 0
  //The NOR flash's ID is always 0x01. If no response is seen on the SPI, then the command will return 0, so we know whether it has responed or not
  if (memory.flashData[0] != 0x01){
    //If it cant be found, something is wrong and the media player cannot start.
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[16])));
    graphic.Print(stringRecover,0,line,4);
    line += 8;
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[2])));
    graphic.Print(stringRecover,0,line,4);
    line += 8;
    return 0; //Setup failure
  }
#endif
  memory.Command(FLASH_RDID,1);
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[14])));
  sprintf(strPrint,"%s%d",stringRecover,memory.flashData[0]);

  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[15])));
  sprintf(strPrint,"%s%s%d%s%d%s%d%s%d",strPrint,stringRecover,memory.flashData[1],stringRecover,memory.flashData[2],stringRecover,memory.flashData[3],stringRecover,memory.flashData[5]);

  graphic.Print(strPrint,0,line,4);
  line += 8;
  delay(500);

#if (MEMTYPE == 0) || (MEMTYPE == 4)
  //The NOR flash's ID is always 0x01. If no response is seen on the SPI, then the command will return 0, so we know whether it has responed or not
  if (memory.flashData[0] != 0x01){
    //If it cant be found, something is wrong and the media player cannot start.
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[18])));
    graphic.Print(stringRecover,0,line,4);
    line += 8;
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[2])));
    graphic.Print(stringRecover,0,line,4);
    line += 8;
    return 0; //Setup failure
  }
#else
  //The NOR flash's ID is always 0xBF. If no response is seen on the SPI, then the command will return 0, so we know whether it has responed or not
  if (memory.flashData[0] != 0xBF){
    //If it cant be found, something is wrong and the media player cannot start.
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[18])));
    graphic.Print(stringRecover,0,line,4);
    line += 8;
    strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[2])));
    graphic.Print(stringRecover,0,line,4);
    line += 8;
    return 0; //Setup failure
  }
#endif
  //Everything started up well, so we can go to the main program.
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[19])));
  graphic.Print(stringRecover,0,line,4);
  line += 8;
  delay(500);
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[20])));
  graphic.Print(stringRecover,0,line,4);
  line += 8;
  strcpy_P(stringRecover,(char*)pgm_read_word(&(startupStrings[21])));
  graphic.Print(stringRecover,0,line,4);
    
  return 1; //Setup success
}
//----------------------------------------------------------------------------------------------------------------------------------

void loop() {
  //Read the last volume setting
  Volume = EEPROM.read(0);
  //Check that it is in range. if not change it to be
  if (Volume > 7){
    Volume = 7;
    EEPROM.write(0, Volume);
  }
  decoder.Buffer[0] = pgm_read_byte(&(volume[Volume])); //Data to send: Pause
  decoder.Buffer[8] = 1; //How many bytes: 1
  decoder.Write(0x46); //Write data to sequential registers
  decoder.Write(0x48); //Write data to sequential registers

  //Update the library file, check for new songs, and add them to correct artist playlist.
  if(!libraryWizard()){
    goToSleep();
  }
  //Loop until it is turned off.
  while(1){
    if(!songSelector()){
      goToSleep();//Shutdown if there is an error.
    }
    if(!mediaPlayer()){
      goToSleep();//mediaPlayer called shutdown
    }
  }
}

//Put the arduino into an unending sleep-------------------------------------------------------------------
void goToSleep(){
  //Shut Down player
  noInterrupts();
  graphic.SetBackColour(0,0,15);
  graphic.SetForeColour(15,15,15);
  graphic.Clear();
  int line = 0;
  strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[15])));
  graphic.Print(stringRecover,0,line,4);
  
  delay(2500);
  
  //Put the memory chips into hibernation
  memory.Command(FLASH_DP,0);
  memory.Command(FLASH_DP,1);

  //Turn off the display
  graphic.displayOff();
  digitalWrite(BacklightPWMPin,LOW); //Disable the PWM to turn the backlight off
  digitalWrite(BacklightOnOffPin,HIGH); //Disable the voltage boost circuitry to save power
  
  //Turn off the trackerball LED
  digitalWrite(LedPin,LOW);
  
  //Put Arduino to sleep
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable(); 
  sleep_mode(); 
  //As there is no interrupt set to wake the chip up from sleep, it will never wake up until it is reset. 
  //This effectively ends the program but also saves power.
}
//---------------------------------------------------------------------------------------------------------

//Library File updater-------------------------------------------------------------------------------------
boolean decodeBitmap(char* bitmapFile){
  if(!SD.exists(bitmapFile)){
    return 0;
  }
  if (SD.exists(tempImage)){
    SD.remove(tempImage);
  }
  songList = SD.open(bitmapFile);
  if((songList.read() != 0x42)||(songList.read() != 0x4D)){
    //If first two bytes are not 0x42 then 0x4D, then not valid
    return 0;
  }
  for(byte i = 0;i < 4;i++){
    songList.read();
  }
  if((songList.read() != 0x00)||(songList.read() != 0x00)||(songList.read() != 0x00)||(songList.read() != 0x00)){
    //If next four bytes are not 0, then not valid.
    return 0;
  }
  long temp;
  unsigned long filePointer = songList.read();
  temp = songList.read();
  filePointer |= temp << 8;
  temp = songList.read();
  filePointer |= temp << 16;
  temp = songList.read();
  filePointer |= temp << 24;
  
  if((songList.read() != 0x28)||(songList.read() != 0x00)||(songList.read() != 0x00)||(songList.read() != 0x00)){
    //If next four bytes are not this, then not valid.
    return 0;
  }
  
  unsigned long imageWidth = songList.read();
  temp = songList.read();
  imageWidth |= temp << 8;
  temp = songList.read();
  imageWidth |= temp << 16;
  temp = songList.read();
  imageWidth |= temp << 24;
    
  unsigned long imageHeight = songList.read();
  temp = songList.read();
  imageHeight |= temp << 8;
  temp = songList.read();
  imageHeight |= temp << 16;
  temp = songList.read();
  imageHeight |= temp << 24;
    
  if ((imageWidth < 128)||(imageHeight < 128)){
    //Image too small
    return 0;
  }
  
  if((songList.read() != 0x01)||(songList.read() != 0x00)||(songList.read() != 0x18)||(songList.read() != 0x00)){
    //If next four bytes are not this, then not valid.
    return 0;
  }
  
  if((songList.read() != 0x00)||(songList.read() != 0x00)||(songList.read() != 0x00)||(songList.read() != 0x00)){
    //If next four bytes are not 0, then not valid.
    return 0;
  }
  unsigned long imageLength = songList.read();
  temp = songList.read();
  imageLength |= temp << 8;
  temp = songList.read();
  imageLength |= temp << 16;
  temp = songList.read();
  imageLength |= temp << 24;
 
  songList.close();
  
  byte padding = 0;
  
  if((imageWidth * 3) % 4){
    padding = 4 - ((imageWidth * 3) % 4);
  }
  boolean ledState = 0;
  byte dataArray[2][192]; //Large array buffering data from SD card - used for songs and bitmaps
  for(byte k = 0;k < 128;k++){
    songList = SD.open(bitmapFile);
    songList.seek(filePointer);
    ledState = !ledState;
    digitalWrite(LedPin,ledState);
    for (byte i = 0; i < 128;i++){
      //Get the image pixel, there is always one per loop
      dataArray[1][(i&1) + 0] = songList.read();//Blue
      dataArray[1][(i&1) + 2] = songList.read();//Green
      dataArray[1][(i&1) + 4] = songList.read();//Red
      if(i&1){
        //There must have been two pixels read for i to be an odd number
        //Convert to 12 bit (ready for the twoPixels Function)
        //NOTE: the nutter that invented bitmaps decided to store all the data in reverse! (so BGR colour, last line first)
        dataArray[0][((i>>1)*3)+0] = (dataArray[1][4] & 0xF0) | (dataArray[1][2] >> 4); //R1G1
        dataArray[0][((i>>1)*3)+1] = (dataArray[1][0] & 0xF0) | (dataArray[1][5] >> 4); //B1R2
        dataArray[0][((i>>1)*3)+2] = (dataArray[1][3] & 0xF0) | (dataArray[1][1] >> 4); //G2B2
      }
      if((pgm_read_byte(&(scalar[imageWidth % 128][i])) + (imageWidth >> 7) - 1)){
        //There are pixels to remove
        for (byte j = 0;j < (pgm_read_byte(&(scalar[imageWidth % 128][i])) + (imageWidth >> 7) - 1);j++){
          //clear a pixel
          songList.read();//Blue
          songList.read();//Green
          songList.read();//Red
        }
      }
    }
    //Clear padding bytes
    for(byte i = 0;i < padding;i++){
      songList.read();
    }
    filePointer += ((imageWidth * 3) + padding);
    if((pgm_read_byte(&(scalar[imageHeight % 128][k])) + (imageHeight >> 7) - 1)){
      //There are pixels to remove
      for (byte j = 0;j < (pgm_read_byte(&(scalar[imageHeight % 128][k])) + (imageHeight >> 7) - 1);j++){
        //clear a line
        for (unsigned long i = 0;i < imageWidth;i++){
          songList.read();//Blue
          songList.read();//Green
          songList.read();//Red
        }
        //Clear padding bytes
        for(byte i = 0;i < padding;i++){
          songList.read();
        }
        filePointer += ((imageWidth * 3) + padding);
      }
    }
    songList.close();
    //Write the first data - cropping the bottom off so that is fits the 128 x 96 pixel space 
    if(k > 31){
      songList = SD.open(tempImage,FILE_WRITE);
      songList.write(dataArray[0],192);
      //Carriage Return and Line Feed
      songList.write(lfcr,2);
      songList.close();
    }
  }

  digitalWrite(LedPin,LOW);
  //Move to next image temporary file - you can add 9999 images in one go which should be plenty :).
  if(tempImage[7] == '9'){
    tempImage[7] = '0';
    if(tempImage[6] == '9'){
      tempImage[6] = '0';
      if(tempImage[5] == '9'){
        tempImage[5] = '0';
        tempImage[4]++;
      } else {
        tempImage[5]++;
      }
    } else {
      tempImage[6]++;
    }
  } else {
    tempImage[7]++;
  }
  return 1;
}

void drawBitmap(char* fileName){
  graphic.Configure(0);
  graphic.Window(1,23,128,118);
  songList = SD.open(fileName);
  byte dataArray[1538]; //Large array buffering data from SD card - used for songs and bitmaps (2 bytes oversize to avoid overflow in final read block

#if (MEMTYPE == 2) || (MEMTYPE == 4)
  memory.page = 160;
  memory.address = 0;
  for(byte i = 0; i < 3;i++){
    memory.sector = i;
    memory.Command(FLASH_SE,1);
    do{
      memory.Command(FLASH_RDSR,1);
    } while(memory.flashData[256] & 1); //Wait for first memory to not be busy before doing anything else
  }
  //Seed the visualisation matrix
  memory.Command(FLASH_PP,1);
  memory.transfer(0x01);
  memory.transfer(0x10); 
  memory.transfer(0x10);
  memory.transfer(0x00);
  memory.transfer(0x00);
  memory.writeSS2(1); //Finish transaction
#endif
  for(byte i = 0;i < 12;i++){
    //Get image data
    for(int j = 0;j < 1536;j += 192){
      songList.read(dataArray + j,194);
    }
    byte k = 0;
    for(int j = 0;j < 1536;j += 3){ //Display the image
#if (MEMTYPE == 2) || (MEMTYPE == 4)
      if(k == 0){//Next page
        do{
          memory.Command(FLASH_RDSR,1);
        } while(memory.flashData[256] & 1); //Wait for first memory to not be busy before doing anything else
        memory.Command(FLASH_PP,1);
        memory.transfer(memory.sector);
        memory.transfer(memory.page++);
        memory.transfer(memory.address);
      }
      memory.transfer(dataArray[j]);
      memory.transfer(dataArray[j+1]);
      memory.transfer(dataArray[j+2]);
      k += 3;
      if(k > 191){
        memory.writeSS2(1);  //Finish Transaction
        k = 0; //skip the rest, so one line per page
      }
#endif
      graphic.twoPixels(dataArray[j],dataArray[j+1],dataArray[j+2]);
    }
  }
  graphic.Configure(1);
  graphic.SetForeColour(0,0,0);
  graphic.Box(0,23,129,118,6);
  songList.close();
}

boolean libraryWizard(){
  if(!SD.exists(songFile)){
    return 1; //file to check is missing, so no need to update library
  }
  boolean found;
  byte lineLength = 41;
  songList = SD.open(songFile);
  int filePointer = 0;
  signed long fileLength;
  int numberOfNew = 0;
  int numberOfNewImages = 0;
  songList.seek(filePointer);
  char addToLib = songList.read();
  while((addToLib == '+')||(addToLib == '@')){ //There is a new song or album art, so add it to the library
    songList.read((byte*)file,8); //get its name
    file[8] = '.';
    if(addToLib == '+'){
      //Song File
      file[9] = 'm';
      file[10] = 'p';
      file[11] = '3';
      file[12] = 0; 
      songList.close();
      if(validateSongFile(file,0)){ //Validate Song
        //If song is valid, then add it to the library. Otherwise it will be ignored
        if(!SD.exists(artistDecoder)){ //Look up to check if there is a decoder file
          recreateIndex(ARTIST, "0001"); //Recreate it
        }
        songIndexer(Artist, artistDecoder, artistFile, lineLength, 10000);
        if(!SD.exists(artistFile)){ //Look up to check if there is an Artist file 
          //Create one if there isn't
          recreateIndex(ALBUM,artistFile);
        }
        songIndexer(Album, artistFile, albumFile, lineLength, 1);
        if(!SD.exists(albumFile)){ //Look up to check if there is an Album file 
          songList = SD.open(albumFile,FILE_WRITE);
          songList.close();
        }
        char ignoreReturn[9];
        if (!findInFile(Title,strlen(Title) + 1,albumFile,ignoreReturn,lineLength,8)){ //Check if Song is already in the file, discard returned filename
          //Reopen for writing, and add the new artist onto the end
          songList = SD.open(albumFile,FILE_WRITE);
          songList.write((byte*)file, 8); //Write the new filename to the decoder list
          songList.write((byte*)Title,strlen(Title) + 1); //Print Title
          for(byte j = strlen(Title) + 1;j < 31;j++){ //Padding
            songList.print(' ');
          }
          //Carriage Return and Line Feed
          songList.write(lfcr,2);
          songList.close();
        }
      }
    } else {
      file[9] = 'b';
      file[10] = 'm';
      file[11] = 'p';
      file[12] = 0; 
      songList.close();
      if (decodeBitmap(file)){
        numberOfNewImages++; //another new Image
      }
    }
    songList = SD.open(songFile);
    filePointer += 11; //Moves to next song (+########<CR><LF> is the format of a new file name, which totals to 11 characters to skip
    songList.seek(filePointer);
    numberOfNew++; //Number of new files
    addToLib = songList.read(); //Get the start of the next line
  }
  fileLength = songList.size();
  songList.close();
  if (numberOfNew){
    //If there are any new files we need to remove the + or @ signs to indicate the songs are in the library or album art is converted.
    if (SD.exists(tempFile)){
      SD.remove(tempFile);
    }
    //Copy contents of old library into the tempfile
    int j;
    for(unsigned long i = 0;i < fileLength;i += j){
      songList = SD.open(songFile);
      songList.seek(i);
      for(j = 0;j < 256;j++){
        memory.flashData[j] = songList.read();
        if ((memory.flashData[j] > 127)||(memory.flashData[j] == 0)){ //EOF
          break;
        }
      }
      songList.close();
      songList = SD.open(tempFile, FILE_WRITE);
      songList.write(memory.flashData,j);
      songList.close();
    }
    SD.remove(songFile); //Delete the old library
    filePointer = 0; //start of file
    while(fileLength > filePointer){
      songList = SD.open(tempFile);
      songList.seek(filePointer);
      byte i = 0;
      for (;i < 250;){ //25 lines
        memory.flashData[i] = songList.read();
        if (memory.flashData[i] == '+'){
          filePointer++;//Move Pointer along one to skip the '+'
        } else if (memory.flashData[i] == '@'){
          filePointer += 11;//Skip the whole line as images not stored
          for(byte j = 0;j < 10;j++){
            songList.read();//Clear the line
          }
        } else if ((memory.flashData[i] > 127)||(memory.flashData[i] == 0)){ 
          //EOF, so break early
          break;
        } else {
          i++;
        }
      }
      filePointer += i; //i more bytes read
      songList.close();

      //Write to end of new file
      songList = SD.open(songFile,FILE_WRITE);
      songList.write(memory.flashData,i);
      songList.close();
    }
  }
  
  while(numberOfNewImages){
    //There is some new album art to assign to albums in the library
    if(tempImage[7] == '0'){
      tempImage[7] = '9';
      if(tempImage[6] == '0'){
        tempImage[6] = '9';
        if(tempImage[5] == '0'){
          tempImage[5] = '9';
          tempImage[4]--;
        } 
        else {
          tempImage[5]--;
        }
      } 
      else {
        tempImage[6]--;
      }
    } 
    else {
      tempImage[7]--;
    }
    printSongSelector();
    
    drawBitmap(tempImage);
    
    graphic.SetForeColour(0,0,0);
    graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
    strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[10])));
    graphic.Print(stringRecover,interfaceX[0],interfaceY[14],4);
    
    setStates();
    while(!buttonState);//Wait for button Press
    digitalWrite(LedPin,HIGH);
    while(buttonState); //Wait for release
    PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
    digitalWrite(LedPin,LOW);
    if(!songSelector(1)){
      //Unknown Error
      return 0;
    }
    SD.remove(tempImage);
    
    numberOfNewImages--;
  }
  return 1;
  //library update complete
}

void songIndexer(char indexString[],char indexFile[],char* subIndex, byte lineLength, int indexStart){       
  if (!findInFile(indexString,strlen(indexString) + 1,indexFile,subIndex,lineLength,8)){ //Check if Artist is already in the file  
    //If not, Work out a new index
    findNextFree(indexFile,subIndex,indexStart,lineLength);    
    //reopen for writing
    songList = SD.open(indexFile,FILE_WRITE);
    //add the new artist onto the end
    songList.write((byte*)subIndex,8); //Write the new filename to the decoder list
    songList.write((byte*)indexString,strlen(indexString) + 1);
    for(byte j = strlen(indexString) + 1;j < 31;j++){//Padding
      songList.print(' ');
    }
    //Carriage Return and Line Feed
    songList.write(lfcr,2);
    songList.close();
  }
}

boolean findInFile(char stringToFind[], byte arrayLength, char fileName[], char* locatedFile, byte lineLength, byte startPosition){
  char temp[lineLength];
  songList = SD.open(fileName); //Open for reading
  long fileLength = songList.size();
  for (long i = 0;i < fileLength;i += lineLength){
    songList.read((byte*)temp,lineLength); //Check through each line to find the string.
    
    if(!strncmp(temp + startPosition,stringToFind,arrayLength)){
      //must be the same
      songList.close();
      strncpy(locatedFile,temp,8); //Start of the line (i.e. file belonging to this string). Don't need a NULL character as gets dropped into a string ending with .TXT\0
      return 1; //Found
    }
  }
  songList.close();
  return 0; //Not found
}

void findNextFree(char inThisFile[], char* newFilename, int increment, byte lineLength){
  //Extract the 8 digit numbering system, using first line as the minimum number
  char nextFree[9];
  songList = SD.open(inThisFile);
  songList.read((byte*)nextFree,8); //Get the first number
  nextFree[8] = 0;
  songList.close();
  
  long wantedNumber = atol(nextFree) + increment; //convert string to number to make life easier.
  while (wantedNumber < 99999999){ //It will return before this is reached. It is here just in case to avoid an endless loop
    //Now work through numbers sequentially to find the next unused one
    sprintf(nextFree,"%08ld",wantedNumber); //convert current number to string
    char ignoreReturn[8];
    if(!findInFile(nextFree,8,inThisFile,ignoreReturn,lineLength,0)){ //Check in file if number already used
      //Number is free, so this what will be returned
      strncpy(newFilename,nextFree,8);
      return;
    }
    //Otherwise, try next number
    wantedNumber += increment;
  }
}

void deleteFromLibrary(char* toDelete, char* indexFile){
  songList = SD.open(indexFile);
  unsigned long fileLength = songList.size(); //Get the length of the file
  songList.close();
  //First copy whole index file into TEMPFILE
  if(SD.exists(tempFile)){
    SD.remove(tempFile); //Remove any old tempfile
  }
  unsigned long filePointer = 0;
  while(filePointer < fileLength){
    songList = SD.open(indexFile);
    songList.seek(filePointer);
    songList.read(memory.flashData,41); //Read one line
    filePointer += 41;
    songList.close();
    songList = SD.open(tempFile,FILE_WRITE);
    songList.write(memory.flashData,41); //Write one line
    songList.close();
  }
  SD.remove(indexFile); //Remove the old index file
  //Now copy the content back into the index file, but missing out the line which is to be deleted.
  filePointer = 0;
  while(filePointer < fileLength){
    songList = SD.open(tempFile);
    songList.seek(filePointer);
    songList.read(memory.flashData,41); //Read one line
    filePointer += 41;
    songList.close();
    if(strncmp(toDelete,(char*)memory.flashData,8) != 0){//Check line to see if it is the one to be removed
      //Not identical, so write into new file
      songList = SD.open(indexFile,FILE_WRITE);
      songList.write(memory.flashData,41); //Write the line
      songList.close();
    } //else it will not be written, and is thus removed
  }
}

void recreateIndex(byte window, char* thisFile){
  //Open the correct file
  switch(window){
    case SETTINGS:
      songList = SD.open(settingsFile,FILE_WRITE);
      break;
    case PLAYLIST:
      songList = SD.open(playlistFile,FILE_WRITE);
      break;
    case ARTIST:
      songList = SD.open(artistDecoder,FILE_WRITE);
      break;
    case ALBUM:
      songList = SD.open(artistFile,FILE_WRITE);
      break;
    default:
      return;
  }
  songList.write((byte*)thisFile,4); //First four bytes have to be changeable to make the function multipurpose
  //Each default file is stored in a portion of fileData[] array in the program memory with start
  //and end of that portion given in filePosition[][0] and filePosition[][1]
  for (unsigned int i = pgm_read_word(&(filePosition[window][0]));i < pgm_read_word(&(filePosition[window][1]));i++){
    songList.write(pgm_read_byte(&(fileData[i])));
  }
  songList.close(); 
}

void assignBitmap(boolean assign){
  graphic.SetForeColour(0,0,0);
  graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
  if(assign){
    strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[11])));
    graphic.Print(stringRecover,interfaceX[0],interfaceY[14],4);//Inform that album assigned
    //If the file exists, replace it
    if (SD.exists(artFile)){
      SD.remove(artFile);
    }
    songList = SD.open(tempImage);
    unsigned long fileLength = songList.size();
    unsigned long filePointer = 0;
    songList.close();
    while (filePointer < fileLength){ //Album art has been assigned, so copy the temp file to its new home
      songList = SD.open(tempImage);
      songList.seek(filePointer);
      songList.read(memory.flashData,194); //One line at a time
      filePointer += 194; //194 more bytes sent
      songList.close();
      
      songList = SD.open(artFile,FILE_WRITE);
      songList.write(memory.flashData,194);
      songList.close();
    }
    strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3])));
    graphic.Print(stringRecover,interfaceX[0],interfaceY[14],4); //Clear line at bottom of the screen that says album assigned.
  } else {
    strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[20])));
    graphic.Print(stringRecover,interfaceX[0],interfaceY[14],4); //Inform that library updated
    delay(1000);
  }
}
//---------------------------------------------------------------------------------------------------------

//Song and/or Playlist Selection---------------------------------------------------------------------------
void compilePlaylist(){
  byte i;
  unsigned long filePointer = 0;
  if(SD.exists(toPlayFile)){
    //Necessary if we are editing a playlist. All data from the old file was copied into TEMPFILE, so will have been preserved
    SD.remove(toPlayFile);
  }
  songList = SD.open(tempFile);
  unsigned long fileLength = songList.size();
  while(filePointer < fileLength){
    //Check through the entire file looking for files to add to the final composition. This means finding a file name (not an album name)
    //Then checking through the remainder of the file to see if it appears with a - in front of it. If it does, stop searching and ignore the file, then go back to
    //the next line after the file name we found and repeat the process. If there is no other mention of the file name in the remainder of the file, add it onto the
    //end of the new playlist file
    songList.seek(filePointer);
    songList.read((byte*)tempName,8); //Get the filename (if first is control char, then other 7 will be ignored
    if (tempName[0] == '-'){
      //It was a control character, so skip this file name
      filePointer += 11;
    } else if ((tempName[0] == '/')||(tempName[0] == '\\')){
      //It was an album name, so skip it
      filePointer += 11;
    } else {
      //It must have been a file name, so search through the file in order, stopping if we come across it with a - sign.
      filePointer += 8;
      if(addToPlaylist(fileLength,filePointer + 32)){
        //Get the title of the song for the interface display (This is the 22 character truncated version)
        songList.seek(filePointer);
        char tempTitle[22];
        songList.read((byte*)tempTitle,22); //Get title
        char tempAlbum[8];
        songList.read((byte*)tempAlbum,8); //Get album name
        //Close tempfile, and open the playlist file
        songList.close();
        songList = SD.open(toPlayFile,FILE_WRITE);
        songList.write((byte*)tempName,8); //write the file name
        songList.write((byte*)tempTitle,22); //Add in the name of the song
        for(byte j = 0;j < 9;j++){ //Pad up to 39 characters long
          songList.print(' ');
        }
        songList.write((byte*)tempAlbum,8); //Add album name
        //Carriage Return and Line Feed
        songList.write(lfcr,2);
        songList.close();
        songList = SD.open(tempFile);
      }
      filePointer += 32;
    }
  }
  songList.close();
}

boolean addToPlaylist(unsigned long fileLength,unsigned long filePointer){
  byte i;
  char temp[10];
  songList.seek(filePointer);
  //Count through the remainder of the file. If we get to the end without meeting the '-' file name we are looking for,
  //then return 1. If we do come across the '-' version of it, then break and return 0.
  while(filePointer < fileLength){
    //Check through the entire file looking for a specific file, keeping track of how many times it has appeared
    temp[0] = songList.read();
    if (temp[0] == '-'){
      songList.read((byte*)temp,10); //Get the name of the song following the '-' sign, including <LF><CR> which we ignore
      if(!strncmp(temp,tempName,8)){ //Compare the name after the '-' with the name we are interested in
        return 0;//Song temp and tempName are identical, so we don't need to add it to the playlist, and can leave this macro early.
      }
      filePointer += 11;
    } else if ((temp[0] == '/')||(temp[0] == '\\')){ //Not interested in this case
      songList.read((byte*)temp,10);
      filePointer += 11;
    } else { //Not interested in this case
      for(i = 1;i < 40;i++){
        songList.read(); //so clear away the line
      }
      filePointer += 40;
    }
  }
  return 1;
}

boolean checkIfSelected(char fileToCheck[], char fileToLookFor[], boolean songOrFile){
  signed int netInFile = 0;
  byte i;
    
  songList = SD.open(fileToCheck);
  signed long fileLength = songList.size();
  while(fileLength >= 0){ //Check through the entire file looking for a specific file, keeping track of how many times it has appeared
    tempName[0] = songList.read();
    if (tempName[0] == '-'){
      if(!songOrFile){ //It was a control character, so the next character is the start of the file name
        i = 0;
        if(getAndCompareFiles(fileToLookFor,i)){
          netInFile--;
        }
        fileLength -= 11;
      } else {  //We are not interested in Songs
        fileLength -= 11;
        for(i = 0;i < 10;i++){  //Clear unwanted characters
          songList.read();
        }
      }
    } else if ((tempName[0] == '/')||(tempName[0] == '\\')){
      if(songOrFile){  //It was a control character, so the next character is the start of the filename, but we need to check something
        i = 0;
        if(tempName[0] == '/'){
          if(getAndCompareFiles(fileToLookFor,i)){
            netInFile++;
          }
        } else {
          if(getAndCompareFiles(fileToLookFor,i)){
            netInFile--;
          }
        }
        fileLength -= 11;
      } else {  //We are not interested in Files
        fileLength -= 11;
        for(i = 0;i < 10;i++){ //Clear unwanted characters
          songList.read();
        }
      }
    } else {  //It must have been part of the file name
      i = 1;
      if(getAndCompareFiles(fileToLookFor,i)){
        netInFile++;
      }
      for(byte j = 0;j < 30;j++){ //clear the rest of the line
        songList.read();
      }
      fileLength -= 40;
    }
  }
  songList.close();
  if (netInFile){
    return 1;
  }
  return 0;
}

boolean getAndCompareFiles(char* toFind, byte i){
  songList.read((byte*)&tempName[i],10-i); //include <LF><CR>, but ignore them later
  //Check if temp is what we are looking for, and if so, return 1, else return 0;
  if(!strncmp(tempName,toFind,8)){
    return 1; //The same
  }
  return 0; //Different
}

char* qwertyKeypad(byte bufferLength){
  char buffer[bufferLength + 1];
  graphic.SetForeColour(0,0,0);
  strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3])));
  for(byte i = 0;i < 14;i++){
    graphic.SetBackColour(12+((i&1)*2),12+((i&1)*2),12+((i&1)*2));
    if(i < 9){
      for(byte j = 0;j < 22;j++){
        char key[2] = {pgm_read_byte(&(keyMap[i][j])),0};
        graphic.Print(key,interfaceX[j],interfaceY[i+2],4);//Print out the keymap
      }
    } else if (i > 10){
      graphic.Print(stringRecover,interfaceX[0],interfaceY[i],4);
    }
  }
  keyIndex[0] = 0;
  keyIndex[1] = 0;
  byte i = 0;
  graphic.SetBackColour(12,12,12);
  graphic.Print("_",interfaceX[i],interfaceY[12],4);
  setStates();//Get current state of the tracker ball and enable pin change interrupts
  for(;i < bufferLength;){
    if (buttonState){
      digitalWrite(LedPin,HIGH);
      while(buttonState); //Wait for release
      PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
      upCount = 0;
      downCount = 0;
      leftCount = 0;
      rightCount = 0;
      
      graphic.SetBackColour(12,12,12);
      if((keyIndex[0] == 8)&&(keyIndex[1] == 14)){
        if(i > 0){
          i--;
        }
        graphic.Print("_ ",interfaceX[i],interfaceY[12],4);
        buffer[i] = ' ';
      } else if((keyIndex[0] == 8)&&(keyIndex[1] == 18)){
        break;//The string is complete
      } else {
        buffer[i] = pgm_read_byte(&(keyMap[keyIndex[0]][keyIndex[1]]));
        char key[2] = {buffer[i],0};
        graphic.Print(key,interfaceX[i],interfaceY[12],4);
        i++;
        if(i < bufferLength){
          graphic.Print("_",interfaceX[i],interfaceY[12],4);
        }
      }
      digitalWrite(LedPin,LOW);
      setStates();//Get current state of the tracker ball and enable pin change interrupts
    } else if (upCount > NUMBEROFPULSES){
      PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
      digitalWrite(LedPin,HIGH);
      upCount = 0;
      downCount = 0;
      leftCount = 0;
      rightCount = 0;
      
      graphic.SetBackColour(12+((keyIndex[0]&1)*2),12+((keyIndex[0]&1)*2),12+((keyIndex[0]&1)*2));
      printKey(keyIndex[1],keyIndex[0],0);
      if(keyIndex[0] == 0){
        keyIndex[0] = 8; //Roll around to bottom
        if((keyIndex[1] == 20)||(keyIndex[1] == 16)||(keyIndex[1] == 12)){
          //Move to correct index for the control keys
          keyIndex[1] -= 2;
        }
        if((keyIndex[1] == 18)||(keyIndex[1] == 14)||(keyIndex[1] == 10)){
          graphic.SetBackColour(interfaceColours[0][0],interfaceColours[0][1],interfaceColours[0][2]);
          printKey(keyIndex[1],keyIndex[0],1);
        }
      } else {
        if((keyIndex[0] == 8)&&((keyIndex[1] == 10)||(keyIndex[1] == 14)||(keyIndex[1] == 18))){
          printKey(keyIndex[1],keyIndex[0],1);  
        }
        keyIndex[0]--; //Move up a row
      }
      graphic.SetBackColour(interfaceColours[0][0],interfaceColours[0][1],interfaceColours[0][2]);
      printKey(keyIndex[1],keyIndex[0],0);        
      digitalWrite(LedPin,LOW);
      setStates();//Get current state of the tracker ball and enable pin change interrupts
    } else if (downCount > NUMBEROFPULSES){
      PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
      digitalWrite(LedPin,HIGH);
      upCount = 0;
      downCount = 0;
      leftCount = 0;
      rightCount = 0;
      
      graphic.SetBackColour(12+((keyIndex[0]&1)*2),12+((keyIndex[0]&1)*2),12+((keyIndex[0]&1)*2));
      printKey(keyIndex[1],keyIndex[0],0);       
      if(keyIndex[0] == 8){
        if((keyIndex[1] == 10)||(keyIndex[1] == 14)||(keyIndex[1] == 18)){
          printKey(keyIndex[1],keyIndex[0],1);
        }
        keyIndex[0] = 0; //Roll around to top
      } else {
        keyIndex[0]++; //Move down a row
        if((keyIndex[0] == 8)&&((keyIndex[1] == 20)||(keyIndex[1] == 16)||(keyIndex[1] == 12))){
          //Move to correct index for the control keys
          keyIndex[1] -= 2;
        }
        if((keyIndex[0] == 8)&&((keyIndex[1] == 18)||(keyIndex[1] == 14)||(keyIndex[1] == 10))){
          graphic.SetBackColour(interfaceColours[0][0],interfaceColours[0][1],interfaceColours[0][2]);
          printKey(keyIndex[1],keyIndex[0],1);
        }
      }
      
      graphic.SetBackColour(interfaceColours[0][0],interfaceColours[0][1],interfaceColours[0][2]);
      printKey(keyIndex[1],keyIndex[0],0);
            
      digitalWrite(LedPin,LOW);
      setStates();//Get current state of the tracker ball and enable pin change interrupts
    } else if (leftCount > NUMBEROFPULSES){
      PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
      digitalWrite(LedPin,HIGH);
      upCount = 0;
      downCount = 0;
      leftCount = 0;
      rightCount = 0;
      
      graphic.SetBackColour(12+((keyIndex[0]&1)*2),12+((keyIndex[0]&1)*2),12+((keyIndex[0]&1)*2));
      printKey(keyIndex[1],keyIndex[0],0);
      if(keyIndex[0] == 8){
        if((keyIndex[1] == 18)||(keyIndex[1] == 14)||(keyIndex[1] == 10)){
          printKey(keyIndex[1],keyIndex[0],1);  
          if((keyIndex[1] == 18)||(keyIndex[1] == 14)){
            keyIndex[1] -= 4; //Move left four columns
            graphic.SetBackColour(interfaceColours[0][0],interfaceColours[0][1],interfaceColours[0][2]);
            printKey(keyIndex[1],keyIndex[0],1);
          } else {
            keyIndex[1] -= 2; //Move left two columns
          }
        } else if (keyIndex[1] == 0){
          keyIndex[1] = 18; //Roll around to right
          graphic.SetBackColour(interfaceColours[0][0],interfaceColours[0][1],interfaceColours[0][2]);
          printKey(keyIndex[1],keyIndex[0],1);
        } else {
          keyIndex[1] -= 2; //Move left two columns
        }
      } else {
        if(keyIndex[1] == 0){
          keyIndex[1] = 20; //Roll around to right
        } else {
          keyIndex[1] -= 2; //Move left two columns
        }
      }
      graphic.SetBackColour(interfaceColours[0][0],interfaceColours[0][1],interfaceColours[0][2]);
      printKey(keyIndex[1],keyIndex[0],0);
      
      digitalWrite(LedPin,LOW);
      setStates();//Get current state of the tracker ball and enable pin change interrupts
    } else if (rightCount > NUMBEROFPULSES){
      PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
      digitalWrite(LedPin,HIGH);
      upCount = 0;
      downCount = 0;
      leftCount = 0;
      rightCount = 0;
      
      graphic.SetBackColour(12+((keyIndex[0]&1)*2),12+((keyIndex[0]&1)*2),12+((keyIndex[0]&1)*2));
      printKey(keyIndex[1],keyIndex[0],0);
      if(keyIndex[0] == 8){
        if((keyIndex[1] == 10)||(keyIndex[1] == 14)||(keyIndex[1] == 18)){
          printKey(keyIndex[1],keyIndex[0],1);
          if((keyIndex[1] == 10)||(keyIndex[1] == 14)){
            keyIndex[1] += 4; //Move left four columns
            graphic.SetBackColour(interfaceColours[0][0],interfaceColours[0][1],interfaceColours[0][2]);
            printKey(keyIndex[1],keyIndex[0],1);
          } else {
            keyIndex[1] = 0; //Roll around to right
          }
        } else {
          keyIndex[1] += 2; //Move left two columns
          if(keyIndex[1] == 10){
            graphic.SetBackColour(interfaceColours[0][0],interfaceColours[0][1],interfaceColours[0][2]);
            printKey(keyIndex[1],keyIndex[0],1);
          }
        }
      } else {
        if(keyIndex[1] == 20){
          keyIndex[1] = 0; //Roll around to right
        } else {
          keyIndex[1] += 2; //Move left two columns
        }
      }
      graphic.SetBackColour(interfaceColours[0][0],interfaceColours[0][1],interfaceColours[0][2]);
      printKey(keyIndex[1],keyIndex[0],0);
      
      digitalWrite(LedPin,LOW);
      setStates();//Get current state of the tracker ball and enable pin change interrupts
    }
  }
  buffer[i] = 0; //Null character
  for(++i;i <= bufferLength;i++){
    buffer[i] = ' ';
  }
  return buffer;
}

void printKey(byte x, byte y, boolean pair){
  if(pair){
    x += 2; //shift on for the second pair
  }
  char key[3] = {pgm_read_byte(&(keyMap[y][x])),pgm_read_byte(&(keyMap[y][x+1])),0};
  graphic.Print(key,interfaceX[x],interfaceY[y+2],4);//Print out the keymap  
}

void printSongSelector(){
  //Print Song Selector Wizard------
  graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
  graphic.SetForeColour(0,0,0);
  graphic.Clear();
  graphic.Line(0,8,130,8,1);
  graphic.Line(0,22,130,22,1);
  graphic.Line(0,119,130,119,1);
  graphic.Line(0,121,130,121,1);
  graphic.SetForeColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
  graphic.Box(0,9,130,21,5);
  graphic.Line(0,120,130,120,1);
  textBackgrounds(0);
}

boolean songSelector(boolean mode){
  static byte line = 2;
  static byte listPage = ARTIST;
  static byte newPlaylist = 0;
  
  boolean removeNextPlaylist = 0; //Used to delete playlist later
  boolean renameNextPlaylist = 0; //Used to rename playlist later
  
  printSongSelector();
  
  switch(listPage){
    case SETTINGS:
      line = printSettings(line,2);
      break;
    case TOPLAY:
      line = printToPlay(line,2);
      break;
    case PLAYLIST:
      line = printPlaylists(line,2);
      break;
    case ARTIST:
      line = printArtists(line,2);
      break;
    case ALBUM:
      line = printAlbums(line,2);
      break;
    case SONG:
      line = printSongs(line,2);
      break;
  }
  graphic.SetBackColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
  graphic.SetForeColour(15,15,15);
  strcpy_P(stringRecover,(char*)pgm_read_word(&(pageTitles[listPage])));
  graphic.Print(stringRecover,0,interfaceY[1],4);
  //--------------------------------
  
  setStates();//Get current state of the tracker ball and enable pin change interrupts
  boolean play = 0;
  while(!play){
    if (buttonState){
      digitalWrite(LedPin,HIGH);
      while(buttonState); //Wait for release
      PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
      switch(listPage){
        case SETTINGS:
          if (settingsOption == 0){
            settingsOption = line;
            switch(settingsOption){
              case 2:
                tempSetting = brightness;
                break;
              case 4:
                tempSetting = dimmedBrightness;
                break;
              case 6:
                tempSetting = contrast;
                break;
              default:
                settingsOption = 0; //Aborts changing. This means the "default" case is not needed for the other change settings switches
                break;
            }
          } else {
            switch(settingsOption){
              case 2:
                brightness = tempSetting;
                EEPROM.write(1,brightness);
                break;
              case 4:
                dimmedBrightness = tempSetting;
                EEPROM.write(2,dimmedBrightness);
                break;
              case 6:
                contrast = tempSetting;
                EEPROM.write(3, contrast + 61);
                break;
            }
            settingsOption = 0; //Finished changing the settings
          }
          break;
        case TOPLAY:
          //Work out playlist line base on the line it was at when clicked, this will be the starting song.
          play = playSongs(0,listPage,line,toPlayFile);
          break;
        case PLAYLIST:
          strncpy(toPlayFile,windowData[line - 2],8);
          //Recreate temp as blank
          if(SD.exists(tempFile)){
            SD.remove(tempFile);
          }
          if ((toPlayFile[4] == '0') && (toPlayFile[5] == '0') && (toPlayFile[6] == '0') && (toPlayFile[7] == '2')){
            //playlist is 00000002, which is used to mean remove playlist
            removeNextPlaylist = 1; //Next playlist clicked on will be removed. If scrolling off the playlist page, remove will be disabled without deleting anything
            renameNextPlaylist = 0; //Cant do both
            graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
            graphic.SetForeColour(0,0,0);
            strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[21])));
            graphic.Print(stringRecover,0,interfaceY[14],4);
            break;
          } else if ((toPlayFile[4] == '0') && (toPlayFile[5] == '0') && (toPlayFile[6] == '0') && (toPlayFile[7] == '3')){
            //playlist is 00000003, which is used to mean rename playlist
            renameNextPlaylist = 1; //Next playlist clicked on will be renamed. If scrolling off the playlist page, rename will be disabled without renaming anything
            removeNextPlaylist = 0; //Cant do both
            graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
            graphic.SetForeColour(0,0,0);
            strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[22])));
            graphic.Print(stringRecover,0,interfaceY[14],4);
            break;
          } else if ((toPlayFile[4] != '0') || (toPlayFile[5] != '0') || (toPlayFile[6] != '0') || (toPlayFile[7] != '1')){
            if(!removeNextPlaylist){ //is the playlist to be removed?
              //playlist is not 00000001(create) or 00000002(remove) or 00000003(rename), open the playlist selected and copy its contents into TEMPFILE.TXT
              if(SD.exists(toPlayFile)){
                //If it exists, first get its length
                songList = SD.open(toPlayFile);
                unsigned long fileLength = songList.size();
                songList.close();
                
                //Then copy its entire contents into TEMPFILE
                char toShift;
                unsigned long filePointer = 0;
                while(filePointer < fileLength){
                  //Open the playlist file
                  songList = SD.open(toPlayFile);
                  songList.seek(filePointer);
                  
                  //Copy part of the content into the buffer line by line, truncating lines from 49 to 40 characters long
                  songList.read(memory.flashData,39);
                  songList.read((byte*)albumFile,8);
                  filePointer += 49; //move to next line
                  songList.close();
            
                  //Write to end of TEMPFILE
                  songList = SD.open(tempFile,FILE_WRITE);
                  songList.write(memory.flashData,30); //30 characters to keep
                  songList.write((byte*)albumFile,8); //And add album name
                  //plus <LF><CR>
                  songList.write(lfcr,2);
                  songList.close();
                  
                  if(!checkIfSelected(tempFile, albumFile, 1)){
                    //Add the name of the album to TEMPFILE with a '/' to say that it is in the file, but is an album file, if it is not already in there
                    //This is bound to make a disorganised mess of the temp file, but the mess gets sorted out when the playlist is saved.
                    songList = SD.open(tempFile,FILE_WRITE);
                    songList.print('/');
                    songList.write((byte*)albumFile,8);
                    //Carriage Return and Line Feed
                    songList.write(lfcr,2);
                    songList.close();
                  }
                }
                SD.remove(toPlayFile); //Delete the old playlist file
              }
              //if not, dont need to do anything, it will be created when we save the playlist
              newPlaylist = 0;
              if (renameNextPlaylist){
                //Go to TOPLAY as that will remove the file from the list of playlists then return back to the playList page
                Line[listPage] = line;
                line = 2;
                listPage = TOPLAY;
                Page[listPage] = 0; //Restore Page to default
                
                //tempFile is used when deleting a line from the playlist index, but currently it is being used to store the
                //data from the current playlast. As such, we will temporarily change what tempFile is called to avoid overwriting
                //the current file. The name array will be corrected after the index has been updated.
                tempFile[7] = '0';             
                
                line = printToPlay(line,2);
                if (line == 255){ //This WILL be true, but just in case something weird happens
                  //Playlist couldn't be found, so go back to PLAYLIST page
                  listPage = PLAYLIST;
                  Line[listPage] = 2;
                  Page[listPage] = 0; //Restore Page to default
                  line = printPlaylists(2,2);
                }
                //pretend it is new so that it gets renamed 
                newPlaylist = 1;
                SD.remove(tempFile); //Remove the odly named tempfile
                tempFile[7] = 'E'; //Put the name back to how it should be
              }
            } else {
              SD.remove(toPlayFile); //Remove the playlist file
              //Go to TOPLAY as that will remove the file from the list of playlists then return back to the playList page
              Line[listPage] = line;
              line = 2;
              listPage = TOPLAY;
              Page[listPage] = 0; //Restore Page to default
              line = printToPlay(line,2);
              if (line == 255){ //This WILL be true, but just in case something weird happens
                //Playlist couldn't be found, so go back to PLAYLIST page
                listPage = PLAYLIST;
                Line[listPage] = 2;
                Page[listPage] = 0; //Restore Page to default
                line = printPlaylists(2,2);
              }
              graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
              graphic.SetForeColour(0,0,0);
              strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3])));
              graphic.Print(stringRecover,0,interfaceY[14],4);
              graphic.SetBackColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
              graphic.SetForeColour(15,15,15);
              strcpy_P(stringRecover,(char*)pgm_read_word(&(pageTitles[listPage])));
              graphic.Print(stringRecover,0,interfaceY[1],4);
              removeNextPlaylist = 0; //Disable "remove"
              break; //Finished removing
            }
          } else {
            if(removeNextPlaylist || renameNextPlaylist){ //"remove" or "rename" enabled, so disable it as creating a new playlist
              removeNextPlaylist = 0; //Disable "remove"
              renameNextPlaylist = 0; //Disable "rename"
              graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
              graphic.SetForeColour(0,0,0);
              strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3])));
              graphic.Print(stringRecover,0,interfaceY[14],4);
            }
            newPlaylist = 1; //Indicate that it is a new playlist so that when it is closed the user can be prompted for a Title
            findNextFree(playlistFile,toPlayFile,1,41); //Assign the playlist a filename, i.e. next available unused 0000####.TXT 
            toPlayFile[8] = '.';
            toPlayFile[9] = 'T';
            toPlayFile[10] = 'X';
            toPlayFile[11] = 'T';
            toPlayFile[12] = 0;
            songList = SD.open(tempFile,FILE_WRITE);
          }
          //Mark that there is a playlist open
          playlistSelected = 1;
          
          //Move to the ARTIST List
          Line[listPage] = line; //Store the last line
          listPage = ARTIST;
          line = printArtists(Line[listPage],2); //Use stored value to move to correct position on return
          graphic.SetBackColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
          graphic.SetForeColour(15,15,15);
          strcpy_P(stringRecover,(char*)pgm_read_word(&(pageTitles[listPage])));
          graphic.Print(stringRecover,0,interfaceY[1],4);
          if(renameNextPlaylist){
            graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
            graphic.SetForeColour(0,0,0);
            strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3])));
            graphic.Print(stringRecover,0,interfaceY[14],4);
            renameNextPlaylist = 0; //Disable rename again.
            goto closePlaylist; //Close it again after renaming
          }
          break;
        case ARTIST:
          if(mode){
            strcpy(artFile,tempImage); //This means we are finished assigning an image
            assignBitmap(0);
            play = 1; //Finish assigning this image
          }
          break;
        case ALBUM:
          if(mode){
            //Get the album name as this is what its album art will be called (with extension .ART not .TXT)
            strncpy(artFile,windowData[line - 2],8);
            assignBitmap(1);
          } else {
            //Get the list of Songs in the album
            strncpy(albumFile,windowData[line - 2],8);
            if (playlistSelected){
              if(checkIfSelected(tempFile, albumFile, 1)){
                //The name of the album is in TEMPFILE more times than it is not, add it with a '\' in front of it to say it is not
                songList = SD.open(tempFile,FILE_WRITE);
                songList.print('\\');
                songList.write((byte*)albumFile,8);
                //Carriage Return and Line Feed
                songList.write(lfcr,2);
                songList.close();
                
                //Add all the songs in the album to TEMPFILE with a '-' in front of them if they are in the file more times than not.
                songList = SD.open(albumFile);
                long fileLength = songList.size();
                songList.close();
                long filePointer = 0;
                while (filePointer < fileLength){
                  songList = SD.open(albumFile);
                  songList.seek(filePointer);
                  songList.read((byte*)file,8); //Read the current file name
                  songList.close();
                  filePointer += 41;
                  if(checkIfSelected(tempFile, file, 0)){ //Should it be added?
                    songList = SD.open(tempFile,FILE_WRITE); //Reopen for write
                    songList.print('-');
                    songList.write((byte*)file,8); //Add the file
                    //Carriage Return and Line Feed
                    songList.write(lfcr,2);
                    songList.close();
                  }
                }
                windowData[line - 2][30] = 0; //Remove Highlighting
              } else {
                //Add the name of the album to TEMPFILE with a '/' to say that it is in the album, but is an album file
                songList = SD.open(tempFile,FILE_WRITE);
                songList.print('/');
                songList.write((byte*)albumFile,8);
                //Carriage Return and Line Feed
                songList.write(lfcr,2);
                songList.close();
                
                //Add all the songs in the album to TEMPFILE if they dont appear in the album more times that not
                songList = SD.open(albumFile);
                long fileLength = songList.size();
                songList.close();
                long filePointer = 0;
                while (filePointer < fileLength){
                  songList = SD.open(albumFile);
                  songList.seek(filePointer);
                  songList.read((byte*)file,8); //get the selected filename
                  //Also include the truncated title for menu display
                  byte tempTitle[22];
                  songList.read(tempTitle,22);
                  songList.close();
                  filePointer += 41;
                  if(!checkIfSelected(tempFile, file, 0)){
                    songList = SD.open(tempFile,FILE_WRITE); //Reopen for write
                    songList.write((byte*)file,8);       //include filename
                    songList.write(tempTitle,22); //Also include the truncated title for menu display
                    songList.write((byte*)albumFile,8);       //And add album name
                    //Carriage Return and Line Feed
                    songList.write(lfcr,2);
                    songList.close();
                  }
                }
                windowData[line - 2][30] = 1; //Add Highlighting
              }
            } else {
              //Play the album file, but make sure it sets playlistLine = 0;
              byte oldPage = Page[listPage];
              Page[listPage] = 0;
              play = playSongs(1,listPage,2,albumFile);
              Page[listPage] = oldPage;
            }
          }
          break;
        case SONG:
          if(playlistSelected){
            strncpy(file,windowData[line - 2],8); //Get the name of the currently selected file
            if(checkIfSelected(tempFile, file, 0)){
              //If the song is in the TEMPFILE more times that it is not (it will only ever be in there one more time that it is not, add the song to the file with a '-' in front of it to
              //Say that it is not in the song
              songList = SD.open(tempFile,FILE_WRITE); //Reopen for write
              songList.print('-');
              songList.write((byte*)file,8);       //include filename
              //Carriage Return and Line Feed
              songList.write(lfcr,2);
              songList.close();
              windowData[line - 2][30] = 0; //Remove Highlighting
            } else {
              //Else, add the name of the song to the file to say that it is in the file
              songList = SD.open(tempFile,FILE_WRITE); //Reopen for write
              songList.write((byte*)file,8);       //include filename
              //Also include the truncated title for menu display
              for(byte j = 0;j < 22;j++){
                songList.print(windowData[line - 2][j + 8]);
              }
              songList.write((byte*)albumFile,8);       //And add album name
              //Carriage Return and Line Feed
              songList.write(lfcr,2);
              songList.close();
              windowData[line - 2][30] = 1; //Add Highlighting
            }
          } else {
            //Create a mini playlist of just this one song in the tempFile to be played
            if(SD.exists(tempFile)){
              SD.remove(tempFile);
            }
            songList = SD.open(tempFile,FILE_WRITE);
            
            songList.write((byte*)windowData[line - 2],31); //Add this one line to the mini playlist
            //And Pad to 39 characters 
            for(byte j = 0;j < 8;j++){
              songList.print(' ');
            }
            songList.write((byte*)albumFile,8); //And add album name
            //Carriage Return and Line Feed
            songList.write(lfcr,2);
            songList.close();
            //Play the mini playlist, making sure that it sets playlistLine = 0
            byte oldPage = Page[listPage];
            Page[listPage] = 0;
            play = playSongs(0,listPage,2,tempFile);
            Page[listPage] = oldPage;
          }
          break;
      }
      digitalWrite(LedPin,LOW);
      setStates();//Get current state of the tracker ball and enable pin change interrupts
    } else if (upCount > NUMBEROFPULSES){
      PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
      digitalWrite(LedPin,HIGH);
      upCount = 0;
      downCount = 0;
      leftCount = 0;
      rightCount = 0;
      switch(listPage){
        case SETTINGS:
          if (settingsOption == 0){
            //Scrolling up aborts changing setting
            graphic.Contrast(contrast);
            analogWrite(BacklightPWMPin,brightness);
            settingsOption = 0;
          }
          line = printSettings(line,0);
          break;
        case TOPLAY:
          line = printToPlay(line,0);
          break;
        case PLAYLIST:
          line = printPlaylists(line,0);
          break;
        case ARTIST:
          line = printArtists(line,0);
          break;
        case ALBUM:
          line = printAlbums(line,0);
          break;
        case SONG:
          line = printSongs(line,0);
          break;
      }     
      digitalWrite(LedPin,LOW);
      setStates();//Get current state of the tracker ball and enable pin change interrupts
    } else if (downCount > NUMBEROFPULSES){
      PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
      digitalWrite(LedPin,HIGH);
      upCount = 0;
      downCount = 0;
      leftCount = 0;
      rightCount = 0;
      switch(listPage){
        case SETTINGS:
          if (settingsOption != 0){
            //Scrolling down aborts changing setting
            graphic.Contrast(contrast);
            analogWrite(BacklightPWMPin,brightness);
            settingsOption = 0;
          }
          line = printSettings(line,1);
          break;
        case TOPLAY:
          line = printToPlay(line,1);
          break;
        case PLAYLIST:
          line = printPlaylists(line,1);
          break;
        case ARTIST:
          line = printArtists(line,1);
          break;
        case ALBUM:
          line = printAlbums(line,1);
          break;
        case SONG:
          line = printSongs(line,1);
          break;
      }
      digitalWrite(LedPin,LOW);
      setStates();//Get current state of the tracker ball and enable pin change interrupts
    } else if (leftCount > NUMBEROFPULSES){
      PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
      digitalWrite(LedPin,HIGH);
      upCount = 0;
      downCount = 0;
      leftCount = 0;
      rightCount = 0;
      switch(listPage){
        case SETTINGS:
          if (settingsOption != 0){
            //i.e. we are changing a setting - currently there are only 3 settings, so 3 options.
            switch(settingsOption){
              case 2:
                if(tempSetting > 109){
                  //Can't scroll below than 109 (min value = 106)
                  tempSetting -= 4;
                  byte bar = map(tempSetting - 106,0,149,0,82);
                  graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
                  graphic.Box(interfaceX[4],interfaceY[3] + 1,interfaceX[4] + bar,interfaceY[3] + 6,5);
                  graphic.SetForeColour(0,0,0);
                  graphic.Box(interfaceX[4] + bar + 1,interfaceY[3] + 1,interfaceX[17] + 5,interfaceY[3] + 6,5);
                  analogWrite(BacklightPWMPin,tempSetting);
                }
                break;
              case 4:
                if(tempSetting > 3){
                  //Can't scroll below 3 (min value = 0)
                  tempSetting -= 4; //Decrease temp value
                  byte bar = map(tempSetting,0,127,0,82);
                  graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
                  graphic.Box(interfaceX[4],interfaceY[5] + 1,interfaceX[4] + bar,interfaceY[5] + 6,5);
                  graphic.SetForeColour(0,0,0);
                  graphic.Box(interfaceX[4] + bar + 1,interfaceY[5] + 1,interfaceX[17] + 5,interfaceY[5] + 6,5);
                  analogWrite(BacklightPWMPin,tempSetting);
                }
                break;
              case 6:
                if(tempSetting > (-61 * graphic._Phillips)){
                  //Can't scroll above 61 (max value = 61)
                  tempSetting--; //Increase temp value
                  byte bar = map(tempSetting,(-61 * graphic._Phillips),61,0,82); //Phillips displays have a larger range than EPSON ones
                  graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
                  graphic.Box(interfaceX[4],interfaceY[7] + 1,interfaceX[4] + bar,interfaceY[7] + 6,5);
                  graphic.SetForeColour(0,0,0);
                  graphic.Box(interfaceX[4] + bar + 1,interfaceY[7] + 1,interfaceX[17] + 5,interfaceY[7] + 6,5);
                  graphic.Contrast(tempSetting);
                }
                break;
            }
          }
          break;
        case TOPLAY:
          //Go to SETTINGS
          Line[listPage] = line;
          line = 2;
          listPage = SETTINGS;
          Page[listPage] = 0; //Restore Page to default
          returnPage = TOPLAY;
          line = printSettings(line,2);          
          graphic.SetBackColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
          graphic.SetForeColour(15,15,15);
          strcpy_P(stringRecover,(char*)pgm_read_word(&(pageTitles[listPage])));
          graphic.Print(stringRecover,0,interfaceY[1],4);
          break;
        case PLAYLIST:
          if(removeNextPlaylist || renameNextPlaylist){ //"remove" or "rename" enabled, so disable it as leaving the page
            removeNextPlaylist = 0; //Disable "remove"
            renameNextPlaylist = 0; //Disable "rename"
            graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
            graphic.SetForeColour(0,0,0);
            strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3])));
            graphic.Print(stringRecover,0,interfaceY[14],4);
          }
          strncpy(toPlayFile,windowData[line - 2],8); //Select the playlist
          if ((toPlayFile[4] == '0') && (toPlayFile[5] == '0') && (toPlayFile[6] == '0') && ((toPlayFile[7] == '1') || (toPlayFile[7] == '2') || (toPlayFile[7] == '3'))){
            //Go to SETTINGS
            Line[listPage] = line;
            line = 2;
            listPage = SETTINGS;
            Page[listPage] = 0; //Restore Page to default
            returnPage = PLAYLIST;
            line = printSettings(line,2);
          } else {
            //This means that the currently selected playlist is not the Create Playlist... line, so we can move into TOPLAY and display the list of songs in the playlist
            //Go to TOPLAY
            Line[listPage] = line;
            line = 2;
            listPage = TOPLAY;
            Page[listPage] = 0; //Restore Page to default
            line = printToPlay(line,2);
            if (line == 255){
              //Playlist couldn't be found, so go back to PLAYLIST page
              listPage = PLAYLIST;
              Line[listPage] = 2;
              Page[listPage] = 0; //Restore Page to default
              line = printPlaylists(2,2);
            }
            graphic.SetBackColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
            graphic.SetForeColour(15,15,15);
            strcpy_P(stringRecover,(char*)pgm_read_word(&(pageTitles[listPage])));
            graphic.Print(stringRecover,0,interfaceY[1],4);
          }
          break;
        case ARTIST:
          if(!mode){
            if (playlistSelected){
              songList.close();
closePlaylist:
              if (newPlaylist){
                //add the name of the new playlist onto the end of the playlist list
                songList = SD.open(playlistFile,FILE_WRITE);
                byte temp[41];
                char *name;
                name = qwertyKeypad(22);
                songList.write((byte*)toPlayFile,8); //Filename
                songList.write((byte*)name,23); //Playlist Name 
                for(byte i = 0;i < 8;i++){ //Add padding
                  songList.print(' ');
                }    
                //plus <LF><CR>
                songList.write(lfcr,2);
                songList.close();
              }
              //Copy the contents of the TEMPFILE into the playlist file, only copying the filename of songs who after adding up all the "in playlist"'s and "not in playlists"'s, have a net "in playlist".
              //This is because there isn't enough SRAM on the arduino to store all the contents of TEMPFILE, and with the current library, it is not possible to delete lines from a file on the SD card
              //or even to write data in the middle of the file, so I have had to be creative!!!
              compilePlaylist();
              playlistSelected = 0;
            }
            Line[listPage] = 2; //Restore line to default
            Page[listPage] = 0; //Restore Page to default
            listPage = PLAYLIST;
            line = printPlaylists(Line[listPage],2); //Use stored value to move to correct position on return
            graphic.SetBackColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
            graphic.SetForeColour(15,15,15);
            strcpy_P(stringRecover,(char*)pgm_read_word(&(pageTitles[listPage])));
            graphic.Print(stringRecover,0,interfaceY[1],4);
          }
          break;
        case ALBUM:
          Line[listPage] = 2; //Restore line to default
          Page[listPage] = 0; //Restore Page to default
          listPage = ARTIST;
          line = printArtists(Line[listPage],2); //Use stored value to move to correct position on return
          graphic.SetBackColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
          graphic.SetForeColour(15,15,15);
          strcpy_P(stringRecover,(char*)pgm_read_word(&(pageTitles[listPage])));
          graphic.Print(stringRecover,0,interfaceY[1],4);
          break;
        case SONG:
          Line[listPage] = 2; //Restore line to default
          Page[listPage] = 0; //Restore Page to default
          listPage = ALBUM;
          line = printAlbums(Line[listPage],2); //Use stored value to move to correct position on return
          graphic.SetBackColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
          graphic.SetForeColour(15,15,15);
          strcpy_P(stringRecover,(char*)pgm_read_word(&(pageTitles[listPage])));
          graphic.Print(stringRecover,0,interfaceY[1],4);
          break;
      }
      digitalWrite(LedPin,LOW);
      setStates();//Get current state of the tracker ball and enable pin change interrupts
    } else if (rightCount > NUMBEROFPULSES){
      PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
      digitalWrite(LedPin,HIGH);
      upCount = 0;
      downCount = 0;
      leftCount = 0;
      rightCount = 0;
      switch(listPage){
        case SETTINGS:
          if (settingsOption != 0){
            //i.e. we are changing a setting - currently there are only 3 settings, so 3 options.
            switch(settingsOption){
              case 2:
                if(tempSetting < 252){
                  //Can't scroll above 252 (max value = 255)
                  tempSetting += 4; //Increase temp value
                  byte bar = map(tempSetting - 106,0,149,0,82);
                  graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
                  graphic.Box(interfaceX[4],interfaceY[3] + 1,interfaceX[4] + bar,interfaceY[3] + 6,5);
                  graphic.SetForeColour(0,0,0);
                  graphic.Box(interfaceX[4] + bar + 1,interfaceY[3] + 1,interfaceX[17] + 5,interfaceY[3] + 6,5);
                  analogWrite(BacklightPWMPin,tempSetting);
                }
                break;
              case 4:
                if(tempSetting < 124){
                  //Can't scroll above 124 (max value = 127)
                  tempSetting += 4; //Increase temp value
                  byte bar = map(tempSetting,0,127,0,82);
                  graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
                  graphic.Box(interfaceX[4],interfaceY[5] + 1,interfaceX[4] + bar,interfaceY[5] + 6,5);
                  graphic.SetForeColour(0,0,0);
                  graphic.Box(interfaceX[4] + bar + 1,interfaceY[5] + 1,interfaceX[17] + 5,interfaceY[5] + 6,5);
                  analogWrite(BacklightPWMPin,tempSetting);
                }
                break;
              case 6:
                if(tempSetting < 61){
                  //Can't scroll above 61 (max value = 61)
                  tempSetting++; //Increase temp value
                  byte bar = map(tempSetting,(-61 * graphic._Phillips),61,0,82); //Phillips displays have a larger range than EPSON ones
                  graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
                  graphic.Box(interfaceX[4],interfaceY[7] + 1,interfaceX[4] + bar,interfaceY[7] + 6,5);
                  graphic.SetForeColour(0,0,0);
                  graphic.Box(interfaceX[4] + bar + 1,interfaceY[7] + 1,interfaceX[17] + 5,interfaceY[7] + 6,5);
                  graphic.Contrast(tempSetting);
                }
                break;
            }
          } else {
            //Not changing a setting, so scrolling right moves page
            Line[listPage] = 2; //Restore line to default
            Page[listPage] = 0; //Restore Page to default
            if(returnPage == TOPLAY){
              //We have to return to the TOPLAY page
              listPage = TOPLAY;
              line = printToPlay(Line[listPage],2); //Use stored value to move to correct position on return
            } else {
              //Else we need to go back to the PLAYLIST page
              listPage = PLAYLIST;
              line = printPlaylists(Line[listPage],2); //Use stored value to move to correct position on return
            }
            graphic.SetBackColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
            graphic.SetForeColour(15,15,15);
            strcpy_P(stringRecover,(char*)pgm_read_word(&(pageTitles[listPage])));
            graphic.Print(stringRecover,0,interfaceY[1],4);
          }
          break;
        case TOPLAY:
          Line[listPage] = 2; //Restore line to default
          Page[listPage] = 0; //Restore Page to default
          listPage = PLAYLIST;
          line = printPlaylists(Line[listPage],2); //Use stored value to move to correct position on return
          graphic.SetBackColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
          graphic.SetForeColour(15,15,15);
          strcpy_P(stringRecover,(char*)pgm_read_word(&(pageTitles[listPage])));
          graphic.Print(stringRecover,0,interfaceY[1],4);
          break;
        case PLAYLIST:
          if(removeNextPlaylist || renameNextPlaylist){ //"remove" or "rename" enabled, so disable it as leaving the page
            removeNextPlaylist = 0; //Disable "remove"
            renameNextPlaylist = 0; //Disable "rename"
            graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
            graphic.SetForeColour(0,0,0);
            strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3])));
            graphic.Print(stringRecover,0,interfaceY[14],4);
          }
          Line[listPage] = line; //Store the last line
          listPage = ARTIST;
          line = printArtists(Line[listPage],2); //Use stored value to move to correct position on return
          graphic.SetBackColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
          graphic.SetForeColour(15,15,15);
          strcpy_P(stringRecover,(char*)pgm_read_word(&(pageTitles[listPage])));
          graphic.Print(stringRecover,0,interfaceY[1],4);
          break;
        case ARTIST:
          Line[listPage] = line; //Store the last line
          strncpy(artistFile,windowData[line - 2],8); //Get the name of the artists album index
          line = 2;
          listPage = ALBUM;
          line = printAlbums(line,2);
          if (line == 255){
            //artist couldn't be found, so go back to ARTIST page
            listPage = ARTIST;
            Line[listPage] = 2;
            Page[listPage] = 0; //Restore Page to default
            line = printArtists(2,2);
          }
          graphic.SetBackColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
          graphic.SetForeColour(15,15,15);
            strcpy_P(stringRecover,(char*)pgm_read_word(&(pageTitles[listPage])));
            graphic.Print(stringRecover,0,interfaceY[1],4);
          break;
        case ALBUM:
          if(!mode){
            Line[listPage] = line; //Store the last line
            strncpy(albumFile,windowData[line - 2],8); //Get the name of the albums song index
            line = 2;
            listPage = SONG;
            line = printSongs(line,2);
            if (line == 255){
              //Album couldn't be found, so go back to ALBUM page
              listPage = ALBUM;
              Line[listPage] = 2;
              Page[listPage] = 0; //Restore Page to default
              line = printAlbums(2,2);
            }
            graphic.SetBackColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
            graphic.SetForeColour(15,15,15);
            strcpy_P(stringRecover,(char*)pgm_read_word(&(pageTitles[listPage])));
            graphic.Print(stringRecover,0,interfaceY[1],4);
          }
          break;
        case SONG:
          break;
      }
      digitalWrite(LedPin,LOW);
      setStates();//Get current state of the tracker ball and enable pin change interrupts
    }
    if(line == 255){
      return 0; //Library Corrupt, shutdown
    }  
  }
  PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
  
  if (mode){
    //We need to put these back to there default values only if we have just assigned an image (i.e. mode = 1)
    line = 2;
    listPage = ARTIST;
  }
  return 1;
}

//Text Backgrounds-----------------------------------------------------------------------------------------
void textBackgrounds(byte start){
  graphic.SetForeColour(0,0,0);
  for(byte line = start + 2;line < 14;line++){
    graphic.SetBackColour(12+((line&1)*2),12+((line&1)*2),12+((line&1)*2));
    strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3])));
    graphic.Print(stringRecover,0,interfaceY[line],4);
  }
}
//---------------------------------------------------------------------------------------------------------

//Move the selected line up or down, and highlight the newly selected line. 
byte moveHighlighter(byte line,boolean dir){
  //Reprint the currently highlighed line as unhighlighed
  graphic.SetBackColour((1 - windowData[line - 2][30])*(12+((line&1)*2)),12+((line&1)*2),12+((line&1)*2));
  graphic.SetForeColour(0,0,0);
  strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3])));
  graphic.Print(stringRecover,0,interfaceY[line],4);
  //reprint text on the last line
  char tempString[23];
  for (byte i = 0;i < 22;i++){
    tempString[i] = windowData[line - 2][i + 8];
  }
  tempString[22] = 0;
  graphic.Print(tempString,0,interfaceY[line],0);
  //Move to the next line depending on direction
  if (dir){
    line++;
  } else {
    line--;
  }
  return line;
}

//Select which list is being displayed (artist, songs, albums, etc), and display the current page of the list
byte gotoPage(byte line, byte tabNumber, byte pages, byte currentPage, char* readFile){
  unsigned long filePointer;
  if(tabNumber == TOPLAY){ //There are 492 (or 588) characters per page in each file, that is 41 (or 49) per line * 12 lines
    filePointer = currentPage * 588;
  } else {
    filePointer = currentPage * 492;
  }
  songList.seek(filePointer); //Move to current pages data in the file
  
  byte linesOnPage = 12;
  if(currentPage == pages - 1){ //On the last page
    unsigned long pageLength = songList.size() - filePointer; 
    //pageLength is actually small enough to fit in an int, but the subtraction is performed with long's,
    //and weird things happen if pageLength is defined as an int
    if(tabNumber == TOPLAY){
      linesOnPage = pageLength / 49;
    } else {
      linesOnPage = pageLength / 41;
    }
  }
  char tempString[23];
  byte i = 0;
  for(;i < linesOnPage;i++){
    //Get a line from the file
    songList.read((byte*)windowData[i],30);
    byte j;
    for(j = 30;j < 41;j++){
      songList.read();//Clear remainder of the line
    }
    if(tabNumber == TOPLAY){
      for(;j < 49;j++){
        songList.read();//Clear remainder of the longer playlist line
      }
    }
    filePointer += j; //Move pointer on to next line
       
    for (j = 0;j < 22;j++){ //Extract the name to be printed from the window data
      tempString[j] = windowData[i][j + 8];
    }
    tempString[22] = 0; //NULL character
    
    //Check if line should be blue when editing a playlist
    if(playlistSelected && ((tabNumber == ALBUM)||(tabNumber == SONG))){
      songList.close();
      char temp[13];
      strncpy(temp,windowData[i],8); //Get name of file from window data
      temp[8] = '.';
      temp[12] = 0;
      //If we are editing a playlist, do a check to determine whether or not a song has been selected
      if (tabNumber == ALBUM){
        temp[9] = 'T';
        temp[10] = 'X';
        temp[11] = 'T';
        windowData[i][30] = checkIfSelected(tempFile,temp,1); //Album
      } else {
        temp[9] = 'm';
        temp[10] = 'p';
        temp[11] = '3';
        windowData[i][30] = checkIfSelected(tempFile,temp,0); //Song
      }
      //If this returns TRUE, then it will be blue, otherwise line will be white.
      songList = SD.open(readFile); //Return to last file
      songList.seek(filePointer); 
    } else {
      windowData[i][30] = 0; //If not editing, then line is white
    }
    
    if(i == line - 2){ //Print currently highlighted line, then move on to next line
      printCursor(line);
      continue;
    }
    graphic.SetBackColour((1 - windowData[i][30])*(12+((i&1)*2)),12+((i&1)*2),12+((i&1)*2)); //colour depends on whether or not the line is highlighed
    graphic.SetForeColour(0,0,0);
    strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3]))); //Blank Line
    graphic.Print(stringRecover,0,interfaceY[i+2],4);
    graphic.Print(tempString,0,interfaceY[i+2],0); 
  }
  //Blank the rest of the lines.
  textBackgrounds(i); //This is needed just in case there are blank lines
  for(;i < 12;i++){
    for(byte j = 0;j < 30;j++){
      windowData[i][j] = -1;
    }
    windowData[i][30] = 0;
  }
  return line; //we have reached the target line and all are filled
}

//At the bottom or top of the page, so move to a new one
byte movePage(byte line, boolean dir, byte tabNumber, char readFile[]){  
  int lines; //The division to find lines will always return an exact integer as lines are a fixed length
  long fileLength = songList.size();
  if(tabNumber == TOPLAY){
    lines = fileLength / 49; //Longer 49 character lines
  } else {
    lines = fileLength / 41; //41 Characters per line
  }
  int currentPage = Page[tabNumber];
  int pages = lines / 12;
  if(lines % 12){
    pages++; //Round up
  }
  if(pages == 0){
    return 255; //No pages so return error
  }
  
  //To save time, it only moves to the next page if there is more than one, otherwise, it just reprints only the newly highlighed line  
  if(dir == 1){ // moving down
    if ((line == 2 + ((lines - 1) % 12)) && (currentPage == pages - 1)){ //Moving down off the end of the last page, so return to the beginning
      if(pages > 1){
        line = 2;
        currentPage = 0;
        gotoPage(line,tabNumber,pages,currentPage,readFile);
      } else {
        line = moveHighlighter(line,dir); //Move as normal
        line = 2; //Adjust line
        printCursor(line);
      }
    } else if (line == 13){
      line = 2;
      currentPage++;
      gotoPage(line,tabNumber,pages,currentPage,readFile);
    } else {
      line = moveHighlighter(line,dir); //Move down as normal
      printCursor(line);
    }
  } else if (dir == 0){ //moving up
    if ((line == 2) && (currentPage == 0)){ //Moving up off the start of the first page, so go to end
      if(pages > 1){
        line = 2 + ((lines - 1) % 12);
        currentPage = pages - 1;
        gotoPage(line,tabNumber,pages,currentPage,readFile);
      } else {
        line = moveHighlighter(line,dir); //Move as normal
        line = 2 + ((lines - 1) % 12); //adjust line
        printCursor(line);
      }
    } else if (line == 2){
      line = 13;
      currentPage--;
      gotoPage(line,tabNumber,pages,currentPage,readFile);
    } else {
      line = moveHighlighter(line,dir); //Move up as normal
      printCursor(line);
    }
  } else { //Going to page for first time
    gotoPage(line,tabNumber,pages,currentPage,readFile);
  }  
  Page[tabNumber] = currentPage;
  return line;
}

void printCursor(byte line){
  char tempString[23];
  for (byte i = 0;i < 22;i++){
    tempString[i] = windowData[line - 2][i + 8];
  }
  tempString[22] = 0;
  graphic.SetForeColour(0,0,0);
  graphic.SetBackColour(interfaceColours[0][0],interfaceColours[0][1],interfaceColours[0][2]);
  strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3])));
  graphic.Print(stringRecover,0,interfaceY[line],4);
  graphic.Print(tempString,0,interfaceY[line],0);
}

//Control the page of available Songs 
byte printSongs(byte line, byte dir){
  if(!SD.exists(albumFile)){
    if((albumFile[4] != '0')||(albumFile[5] != '0')||(albumFile[6] != '0')||(albumFile[7] != '1')){ //Unknown = xxxx0001, and should not be deleted, if not unknown:
      deleteFromLibrary(albumFile,artistFile);//albumFile Doesn't Exist, so remove it from the library.
    }
    return 255;//This will inform SongSelector to return to Album Page
  }
  songList = SD.open(albumFile);
  
  line = movePage(line,dir,ALBUM,albumFile);
  if(line == 255){
    songList.close();
    SD.remove(albumFile);// file is empty, so remove the old one
    if((albumFile[4] != '0')||(albumFile[5] != '0')||(albumFile[6] != '0')||(albumFile[7] != '1')){ //Unknown = xxxx0001, and should not be deleted, if not unknown:
      deleteFromLibrary(albumFile,artistFile);//albumFile Doesn't Exist, so remove it from the library.
    }
    return 255;//This will inform SongSelector to return to Album Page
  }
  songList.close();
  return line;
}

//Control the page of available Albums
byte printAlbums(byte line, byte dir){
  if(!SD.exists(artistFile)){
    if((artistFile[0] != '0')||(artistFile[1] != '0')||(artistFile[2] != '0')||(artistFile[3] != '1')){ //Unknown = 0001xxxx, and should not be deleted, if not unknown:
      deleteFromLibrary(artistFile,artistDecoder);//artistFile Doesn't Exist, so remove it from the library.
      return 255;//This will inform SongSelector to return to Artist Page
    }
    //Recreate the file if it is not there, only if it is the "Unknown" page
    recreateIndex(ALBUM, artistFile);
  }
  songList = SD.open(artistFile);
  
  line = movePage(line,dir,ALBUM,artistFile);
  if(line == 255){
    songList.close();
    SD.remove(artistFile);// file is empty, so remove the old one
    if((artistFile[0] != '0')||(artistFile[1] != '0')||(artistFile[2] != '0')||(artistFile[3] != '1')){ //Unknown = 0001xxxx, and should not be deleted, if not unknown:
      deleteFromLibrary(artistFile,artistDecoder);//artistFile Doesn't Exist, so remove it from the library.
      return 255;//This will inform SongSelector to return to Artist Page
    }
    recreateIndex(ALBUM, artistFile);//Recreate the file if it is not there, only if it is the "Unknown" page
    songList = SD.open(artistFile);
    line = 2;
    line = movePage(line,dir,ALBUM,artistFile); //Retry now that file is recreated
  }
  songList.close();
  return line;
}

//Control the page of available Artists
byte printArtists(byte line, byte dir){
  if(!SD.exists(artistDecoder)){
    //Recreate the file if it is not there.
    recreateIndex(ARTIST,"0001");
  }
  songList = SD.open(artistDecoder);
  
  line = movePage(line,dir,ARTIST,artistDecoder);
  if(line == 255){
    songList.close();
    SD.remove(artistDecoder);// file is empty, so remove the old one
    recreateIndex(ARTIST,"0001"); //and recreate the default
    songList = SD.open(artistDecoder);
    line = 2;
    line = movePage(line,dir,ARTIST,artistDecoder); //Retry now that file is recreated
  }
  songList.close();
  return line;
}

//Control the page of available Playlists
byte printPlaylists(byte line, byte dir){
  if(!SD.exists(playlistFile)){
    //Recreate the file if it is not there.
    recreateIndex(PLAYLIST);
  }
  songList = SD.open(playlistFile);
  
  line = movePage(line,dir,PLAYLIST,playlistFile);
  if(line == 255){
    songList.close();
    SD.remove(playlistFile);// file is empty, so remove the old one
    recreateIndex(PLAYLIST); //and recreate the default
    songList = SD.open(playlistFile);
    line = 2;
    line = movePage(line,dir,PLAYLIST,playlistFile); //Retry now that file is recreated
  }
  songList.close();
  return line;
}

//Control the page of songs to Play 
byte printToPlay(byte line, byte dir){
  if(!SD.exists(toPlayFile)){
    deleteFromLibrary(toPlayFile,playlistFile);//toPlayFile Doesn't Exist, so remove it from playlistFile.
    return 255; //This will inform SongSelector to return to Playlist Page
  }
  songList = SD.open(toPlayFile);
  
  line = movePage(line,dir,TOPLAY,toPlayFile);
  if(line == 255){
    songList.close();
    SD.remove(toPlayFile);// file is empty, so remove the old one
    deleteFromLibrary(toPlayFile,playlistFile);//toPlayFile Doesn't Exist, so remove it from playlistFile.
    return 255; //This will inform SongSelector to return to Playlist Page
  }
  songList.close();
  return line;
}

//Control the page of settings
byte printSettings(byte line, byte dir){
  if(!SD.exists(settingsFile)){
    //Recreate the file if it is not there.
    recreateIndex(SETTINGS);
  }
  songList = SD.open(settingsFile);
  
  line = movePage(line,dir,SETTINGS,settingsFile);
  if(line == 255){
    songList.close();
    SD.remove(settingsFile);// file is empty, so remove the old one
    recreateIndex(SETTINGS); //and recreate the default
    songList = SD.open(settingsFile);
    line = 2;
    line = movePage(line,dir,SETTINGS,settingsFile); //Retry now that file is recreated
  }
  if(dir < 2){//Jump two at a time, as the line in between will be used to show option!
    line = movePage(line,dir,SETTINGS,settingsFile);
  }
  //Print the options for each setting.
  byte bar[3] = {  map(brightness - 106,0,149,interfaceX[4],interfaceX[4] + 82),  map(dimmedBrightness,0,127,interfaceX[4],interfaceX[4] + 82),  map(contrast,(-61 * graphic._Phillips),61,interfaceX[4],interfaceX[4] + 82)};
  for(byte i = 0;i<3;i++){
    graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
    graphic.Box(interfaceX[4],interfaceY[3+(2*i)] + 1,bar[i],interfaceY[3+(2*i)] + 6,5);
    graphic.SetForeColour(0,0,0);
    graphic.Box(bar[i] + 1,interfaceY[3+(2*i)] + 1,interfaceX[17] + 5,interfaceY[3+(2*i)] + 6,5);
  }   
  return line;
}

byte playSongs(boolean album, byte listPage, byte line, char* listOfSongs){
    playlistLine = (Page[listPage] * 12) + line - 2;
    firstSong = 1;
    for(byte i = 0;i < 8;i++){
      toPlayFile[i] = listOfSongs[i];
    }
    if(!SD.exists(toPlayFile)){
      //Do not play, as file is invalid
      return 0;
    }
    songList = SD.open(toPlayFile);
    
    //Playlist file must contain multiples of 41 or (49) characters, 8 for the file, 31 for the menu display, (8 for the album), two for the CR and LF
    lineLength = 49;
    if(album){
      lineLength = 41;
    }
    if(playlist.size() % lineLength){
      //Do not play, as playlist is invalid
      songList.close();
      return 0;
    }
    numberOfSongs = songList.size() / lineLength;
    songList.close();
    return 1;
}

//Linear Feedback Shift Register
unsigned int shuffler(unsigned int lfsr){
  lfsr++;
  do {
    /* taps: 16 14 13 11; characteristic polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
    boolean newbit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
    lfsr =  (lfsr >> 1) | (newbit << 15);
  } while(lfsr > numberOfSongs); //Make sure the number is in range of the playlist
  lfsr--;
  return lfsr;
}

void readPlaylist(){
  playlist = SD.open(toPlayFile); //Open the playlist
  if (!firstSong){
    if ((!Shuffle)&&((playlistLine * lineLength) >= playlist.size())){
      //If we have reached the end, go back to the beginning when shuffle is off
      playlistLine = 0;
    } else if (Shuffle){
      //Pick a random song if shuffle is on. 
      playlistLine = shuffler(playlistLine); //Picks a random number within the range of the playlist
    } else {
      //Go to next song
      playlistLine++;
    }
    numberPlayed++;
  } else {
    numberPlayed = 1; //Number is compared at the end of the mediaPlayer loop, so we always need to be one ahead at this point
    firstSong = 0;
  }
  //Move to the correct place in the file
  playlist.seek(playlistLine * lineLength); 
  
  playlist.read((byte*)file,8);//Get the song name.
  file[8] = '.';
  file[9] = 'm';
  file[10] = 'p';
  file[11] = '3';
  file[12] = 0;
  
  //Get the 8 characters of the album art.
  if(lineLength == 49){
    playlist.seek((playlistLine * 49) + 39); 
    playlist.read((byte*)artFile,8);
  } else {
    strncpy(artFile,toPlayFile,8);
  }
  if(!SD.exists(artFile)){
    strncpy(artFile,defaultArtFile,8); //Album art not available, so use default
  }
  //Finished with the playlist for now. Only one file can be open at a time, so we need to close the playlist file
  playlist.close();
}
//---------------------------------------------------------------------------------------------------------

//Part of the Media Player that allows user to control things while playing--------------------------------
void printMediaPlayer(){
  graphic.SetForeColour(interfaceColours[2][0],interfaceColours[2][1],interfaceColours[2][2]);
  graphic.Box(0,9,130,21,5);
  graphic.SetForeColour(15,15,15);
  graphic.SetBackColour(0,0,0);
  graphic.Line(29,10,29,20,1);
  graphic.Line(73,10,73,20,1);
  graphic.Line(114,10,114,20,1);
  
  //Shuffle
  if (Shuffle){
    graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
  } else {
    graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
  }
  char temp[2] = {mediaCharacters[SHUFFLE],0};
  graphic.Circle(mediaControlX[SHUFFLE]+2,15,6,4);
  graphic.Print(temp,mediaControlX[SHUFFLE],12,4);
  
  //Repeat
  if (Repeat){
    graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
  } else {
    graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
  }
  temp[0] = mediaCharacters[REPEAT];
  graphic.Circle(mediaControlX[REPEAT]+2,15,6,4);
  graphic.Print(temp,mediaControlX[REPEAT],12,4);
  
  //Stop
  graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
  graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
  temp[0] = mediaCharacters[STOP];
  graphic.Circle(mediaControlX[STOP]+2,15,6,4);
  graphic.Print(temp,mediaControlX[STOP],12,4);
  mediaControl = STOP;
  
  //Play
  graphic.SetBackColour(0,0,0);
  graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
  temp[0] = mediaCharacters[PLAY];
  graphic.Circle(mediaControlX[PLAY]+2,15,6,4);
  graphic.Print(temp,mediaControlX[PLAY],12,4);
  
  //Skip
  temp[0] = mediaCharacters[SKIP];
  graphic.Circle(mediaControlX[SKIP]+2,15,6,4);
  graphic.Print(temp,mediaControlX[SKIP],12,4);
    
  //Volume
  graphic.SetForeColour(15,15,15);
  graphic.Box(86,9,112,21,4);
  graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
  if (mute){
    temp[0] = mediaCharacters[MUTE];  
    //Volume scale
    for (byte i = 8;i > 0;i--){
      graphic.Box(mediaControlX[VOLUME+i],19,mediaControlX[VOLUME+i] + 1,19,5);
    }
    graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
    graphic.Circle(mediaControlX[MUTE]+2,15,6,4);
    graphic.Print(temp,mediaControlX[MUTE],12,4);
    graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
  } else {
    temp[0] = mediaCharacters[VOLUME];
    //Volume scale
    byte i;
    for (i = 8;i > (Volume + 1);i--){
      graphic.Box(mediaControlX[VOLUME+i],19,mediaControlX[VOLUME+i] + 1,19,5);
    }
    for (;i > 0;i--){
      graphic.Box(mediaControlX[VOLUME+i],19 - i,mediaControlX[VOLUME+i] + 1,19,5);
    }
    graphic.Circle(mediaControlX[VOLUME]+2,15,6,4);
    graphic.Print(temp,mediaControlX[VOLUME],12,4);
  }
  
  //Shutdown
  graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
  shutdownEnable = 0;
  temp[0] = mediaCharacters[SHUTDOWN];
  graphic.Circle(mediaControlX[SHUTDOWN]+2,15,6,4);
  graphic.Print(temp,mediaControlX[SHUTDOWN],12,4);
  
  graphic.SetForeColour(0,0,0);
}

void volumeGraph(){   
  //Volume Graph
  graphic.SetBackColour(0,0,0);
  graphic.SetForeColour(15,15,15);
  graphic.Box(86,9,112,21,4);
  graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
  if (mute){
    //Volume scale
    for (byte i = 8;i > 0;i--){
      graphic.Box(mediaControlX[VOLUME+i],19,mediaControlX[VOLUME+i] + 1,19,5);
    }
  } else {
    //Volume scale
    byte i;
    for (i = 8;i > (Volume + 1);i--){
      graphic.Box(mediaControlX[VOLUME+i],19,mediaControlX[VOLUME+i] + 1,19,5);
    }
    for (;i > 0;i--){
      graphic.Box(mediaControlX[VOLUME+i],19 - i,mediaControlX[VOLUME+i] + 1,19,5);
    }
  }
}

boolean songTools(){
  static char temp[2] = {-127,0};
  
  graphic.SetBackColour(0,0,0);
  
  if (buttonState){
    digitalWrite(LedPin,HIGH);
    while(buttonState); //Wait for release
    PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
    
    if ((shutdownEnable)&&(mediaControl == SHUTDOWN)){
      return 0; //Indicates shutdown
    }
    shutdownEnable = 0;
    if (mediaControl == VOLUME){
      //Mute
      mute = 1;
      decoder.Buffer[0] = mute; //Data to send: Unmute
      decoder.Buffer[8] = 1; //How many bytes: 1
      decoder.Write(20); //Write data to register
      mediaControl = MUTE;
      volumeGraph();
      graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
      graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
      temp[0] = mediaCharacters[mediaControl];
      graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
      graphic.Print(temp,mediaControlX[mediaControl],12,4);
    } else if (mediaControl == MUTE){
      //Unmute
      mute = 0;
      decoder.Buffer[0] = mute; //Data to send: Unmute
      decoder.Buffer[8] = 1; //How many bytes: 1
      decoder.Write(20); //Write data to register
      mediaControl = VOLUME;
      volumeGraph();
      graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
      graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
      temp[0] = mediaCharacters[mediaControl];
      graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
      graphic.Print(temp,mediaControlX[mediaControl],12,4);
    } else if (mediaControl == SKIP){
      //Skip to the next song
      detachInterrupt(dataReqInt); 
      finished = 1; 
      play = 0;
      decoder.Buffer[0] = play; //Data to send: Pause
      decoder.Buffer[8] = 1; //How many bytes: 1
      decoder.Write(19); //Write data to register
      bytesSent = 0;
    } else if (mediaControl == PAUSE){
      play = 1;
      decoder.Buffer[0] = play; //Data to send: Play/Pause
      decoder.Buffer[8] = 1; //How many bytes: 1
      decoder.Write(19); //Write data to register
      mediaControl = PLAY;
      graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
      graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
      temp[0] = mediaCharacters[mediaControl];
      graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
      graphic.Print(temp,mediaControlX[mediaControl],12,4);
    } else if (mediaControl == PLAY){
      play = 0;
      decoder.Buffer[0] = play; //Data to send: Play/Pause
      decoder.Buffer[8] = 1; //How many bytes: 1
      decoder.Write(19); //Write data to register
      mediaControl = PAUSE;
      graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
      graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
      temp[0] = mediaCharacters[mediaControl];
      graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
      graphic.Print(temp,mediaControlX[mediaControl],12,4);
    } else if (mediaControl == STOP){
      //Stop Playback and return to the song selector
      detachInterrupt(dataReqInt); 
      finished = 1; 
      numberPlayed = numberOfSongs;
      play = 0;
      decoder.Buffer[0] = play; //Data to send: Pause
      decoder.Buffer[8] = 1; //How many bytes: 1
      decoder.Write(19); //Write data to register
      bytesSent = 0;
      Repeat = 0; //Turn off repeat so can exit the media Player - else Stop will go back to beginning of song
    } else if (mediaControl == REPEAT){
      Repeat = 1 - Repeat; //Turn Repeat on or off
      if(Repeat){
        graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
      } else {
        graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
      }
      graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
      temp[0] = mediaCharacters[mediaControl];
      graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
      graphic.Print(temp,mediaControlX[mediaControl],12,4);
    } else if (mediaControl == SHUFFLE){
      Shuffle = 1 - Shuffle; //Turn Shuffle on or off
      if(Shuffle){
        graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
      } else {
        graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
      }
      graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
      temp[0] = mediaCharacters[mediaControl];
      graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
      graphic.Print(temp,mediaControlX[mediaControl],12,4);
    }
    digitalWrite(LedPin,LOW);
    setStates();//Get current state of the tracker ball and enable pin change interrupts
  } else if (upCount > NUMBEROFPULSES){
    shutdownEnable = 0;
    PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
    digitalWrite(LedPin,HIGH);
    
    if (mediaControl == MUTE){
      //Unmute
      mute = 0;
      decoder.Buffer[0] = mute; //Data to send: Unmute
      decoder.Buffer[8] = 1; //How many bytes: 1
      decoder.Write(20); //Write data to register
      mediaControl = VOLUME;
      graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
      graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
      temp[0] = mediaCharacters[mediaControl];
      graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
      graphic.Print(temp,mediaControlX[mediaControl],12,4);
      volumeGraph();
    } else if (mediaControl == VOLUME){
      //Volume Down
      if (Volume < 7){
        Volume++;
        EEPROM.write(0,Volume);
        decoder.Buffer[0] = pgm_read_byte(&(volume[Volume])); //Data to send: Volume
        decoder.Buffer[8] = 1; //How many bytes: 1
        decoder.Write(0x46); //Write data to register
        decoder.Buffer[0] = pgm_read_byte(&(volume[Volume])); //Data to send: Volume
        decoder.Buffer[8] = 1; //How many bytes: 1
        decoder.Write(0x48); //Write data to register
      }
      volumeGraph();
    } else if (mediaControl == SHUTDOWN){
      shutdownEnable = 1;
      graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
      graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
      temp[0] = mediaCharacters[mediaControl];
      graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
      graphic.Print(temp,mediaControlX[mediaControl],12,4);
    }
    digitalWrite(LedPin,LOW);
    setStates();//Get current state of the tracker ball and enable pin change interrupts
  } else if (downCount > NUMBEROFPULSES){
    if(mediaControl == SHUTDOWN){
      shutdownEnable = 0;
      graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
      graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
      temp[0] = mediaCharacters[mediaControl];
      graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
      graphic.Print(temp,mediaControlX[mediaControl],12,4);
    }
    PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
    digitalWrite(LedPin,HIGH);
   
    if (mediaControl == MUTE){
      //Unmute
      mute = 0;
      decoder.Buffer[0] = mute; //Data to send: Unmute
      decoder.Buffer[8] = 1; //How many bytes: 1
      decoder.Write(20); //Write data to register
      mediaControl = VOLUME;
      graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
      graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
      temp[0] = mediaCharacters[mediaControl];
      graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
      graphic.Print(temp,mediaControlX[mediaControl],12,4);
      volumeGraph();
    } else if (mediaControl == VOLUME){
      //Volume Down
      if (Volume > 0){
        Volume--;
        EEPROM.write(0,Volume);
        decoder.Buffer[0] = pgm_read_byte(&(volume[Volume])); //Data to send: Volume
        decoder.Buffer[8] = 1; //How many bytes: 1
        decoder.Write(0x46); //Write data to register
        decoder.Buffer[0] = pgm_read_byte(&(volume[Volume])); //Data to send: Volume
        decoder.Buffer[8] = 1; //How many bytes: 1
        decoder.Write(0x48); //Write data to register
      } else {
        //Enable Mute
        mute = 1;
        decoder.Buffer[0] = mute; //Data to send: Mute
        decoder.Buffer[8] = 1; //How many bytes: 1
        decoder.Write(20); //Write data to register
        mediaControl = MUTE;
        graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
        graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
        temp[0] = mediaCharacters[mediaControl];
        graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
        graphic.Print(temp,mediaControlX[mediaControl],12,4);
      }
      volumeGraph();
    }
    
    digitalWrite(LedPin,LOW);
    setStates();//Get current state of the tracker ball and enable pin change interrupts
  } else if (leftCount > NUMBEROFPULSES){
    PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
    digitalWrite(LedPin,HIGH);
    
    //Unhighlight the current position of the cursor  
    if ((mediaControl == SHUTDOWN)||((mediaControl == SHUFFLE)&&(!Shuffle))||((mediaControl == REPEAT)&&(!Repeat))||(mediaControl == MUTE)||(mediaControl == NOSPEAKER)){
      //Cases where the character is red
      graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
    } else {
      //Cases where the character is blue 
      graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
    }
    temp[0] = mediaCharacters[mediaControl];
    graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
    graphic.Print(temp,mediaControlX[mediaControl],12,4);
    
    if(mediaControl == PAUSE){
      //Currently the cursor is hovering over pause, then we need to move the index one place to jump over PLAY
      mediaControl -= 2;
    } else if ((play) && (mediaControl == SKIP)){
      //The song is playling, and currently the cursor is hovering over skip, then we need to move the index one place to jump over PAUSE
      mediaControl -= 2;
    } else if ((!phones) && (mediaControl == SHUTDOWN)){
      shutdownEnable = 0;
      //The no headphones, and currently the cursor is hovering over shutdown, then we need to move the index nine places to jump over VOLUME
      mediaControl -= 10;
    } else if ((mute) && (mediaControl == SHUTDOWN)){
      shutdownEnable = 0;
      //The song is muted, and currently the cursor is hovering over shutdown, then we need to move the index ten places to jump over NOSPEAKER and VOLUME
      mediaControl -= 11;
    } else if (mediaControl == SHUTDOWN){
      shutdownEnable = 0;
      //The song is unmuted and currently the cursor is hovering over shutdown, then we need to move the index eight places
      mediaControl -= 9;
    } else if (mediaControl == VOLUME){
      //Currently the cursor is hovering over volume, then we need to move the index two places to jump over MUTE and NOSPEAKER
      mediaControl -= 3;
    } else if (mediaControl == NOSPEAKER){
      //Currently the cursor is hovering over nospeaker, then we need to move the index one place to jump over MUTE
      mediaControl -= 2;
    } else if(mediaControl == SHUFFLE){
      //Already at the far left, so move to the far right
      mediaControl = SHUTDOWN;
    } else {
      //Otherwise, just move left one place
      mediaControl--;
    }    
    //Highlight the new index 
    if ((mediaControl == SHUTDOWN)||((mediaControl == SHUFFLE)&&(!Shuffle))||((mediaControl == REPEAT)&&(!Repeat))||(mediaControl == MUTE)||(mediaControl == NOSPEAKER)){
      //Cases where the character is red
      graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
    } else {
      //Cases where the character is blue 
      graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
    }   
    graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
    temp[0] = mediaCharacters[mediaControl];
    graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
    graphic.Print(temp,mediaControlX[mediaControl],12,4);
    
    digitalWrite(LedPin,LOW);
    setStates();//Get current state of the tracker ball and enable pin change interrupts
  } else if (rightCount > NUMBEROFPULSES){
    PCICR &= ~(1 << PCIE2); //Disable pin change interrupts
    digitalWrite(LedPin,HIGH);
    
    //Unhighlight the current position of the cursor  
    if ((mediaControl == SHUTDOWN)||((mediaControl == SHUFFLE)&&(!Shuffle))||((mediaControl == REPEAT)&&(!Repeat))||(mediaControl == MUTE)||(mediaControl == NOSPEAKER)){
      //Cases where the character is red
      graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
    } else {
      //Cases where the character is blue 
      graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
    }
    temp[0] = mediaCharacters[mediaControl];
    graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
    graphic.Print(temp,mediaControlX[mediaControl],12,4);
    
    if(mediaControl == PLAY){
      //Currently the cursor is hovering over play, then we need to move the index one place to jump over PAUSE
      mediaControl += 2;
    } else if ((!play) && (mediaControl == STOP)){
      //The song is paused, and currently the cursor is hovering over stop, then we need to move the index one place to jump over PLAY
      mediaControl += 2;
    } else if ((!phones) && (mediaControl == SKIP)){
      //The no headphones, and currently the cursor is hovering over skip, then we need to move the index one place to jump over MUTE
      mediaControl += 2;
    } else if ((!mute) && (mediaControl == SKIP)){
      //The song is not muted, and currently the cursor is hovering over skip, then we need to move the index two places to jump over MUTE and SKIP
      mediaControl += 3;
    } else if (mediaControl == MUTE){
      //Currently the cursor is hovering over mute, then we need to move the index ten places to jump over VOLUME and NOSPEAKER
      mediaControl += 11;
    } else if (mediaControl == NOSPEAKER){
      //Currently the cursor is hovering over no speaker, then we need to move the index nine places to jump over VOLUME
      mediaControl += 10;
    } else if (mediaControl == VOLUME){
      //Currently the cursor is hovering over volume, then we need to move the index eight places
      mediaControl += 9;
    } else if(mediaControl == SHUTDOWN){
      shutdownEnable = 0;
      //Already at the far right, so move to the far left
      mediaControl = SHUFFLE;
    } else {
      //Otherwise, just move right one place
      mediaControl++;
    }
    
    //Highlight the new index
    if ((mediaControl == SHUTDOWN)||((mediaControl == SHUFFLE)&&(!Shuffle))||((mediaControl == REPEAT)&&(!Repeat))||(mediaControl == MUTE)||(mediaControl == NOSPEAKER)){
      //Cases where the character is red
      graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
    } else {
      //Cases where the character is blue 
      graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
    }   
    graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
    temp[0] = mediaCharacters[mediaControl];
    graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
    graphic.Print(temp,mediaControlX[mediaControl],12,4);
    
    digitalWrite(LedPin,LOW);
    setStates();//Get current state of the tracker ball and enable pin change interrupts
  }
  return 1;
}
//---------------------------------------------------------------------------------------------------------

//The actual Media Player----------------------------------------------------------------------------------
boolean mediaPlayer(){
  mute = 0;
  decoder.Buffer[0] = mute; //Data to send: Unmute
  decoder.Buffer[8] = 1; //How many bytes: 1
  decoder.Write(20); //Write data
  memory.writeDataEn(1);
  do{
    readPlaylist();
    graphic.SetForeColour(0,0,0);
    graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
    drawBitmap(artFile);
    graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
    strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[23]))); //"Validating Song..."
repeatSongT2:
    if(!validateSongFile(file,1)){
      strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[1]))); //"Song Error..."
      graphic.Print(stringRecover,interfaceX[0],interfaceY[14],4); 
      continue; //Move on to the next song, or leave the loop if there isn't another
    }
    strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[2]))); //"Song File Loaded!"
    graphic.Print(stringRecover,interfaceX[0],interfaceY[14],4);

    char songInfo[200]; //Create a String of all the tag info to be scrolled through.
    strcpy_P(songInfo,(char*)pgm_read_word(&(messages[3]))); //"                      " (22 spaces)
    strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[4]))); //" | " (seperator)
    sprintf(songInfo + 22,"%s%s%s%s%s%s%s%s%s%s%s%s",stringRecover,Title,stringRecover,Artist,stringRecover,Album,stringRecover,Genre,stringRecover,Track,stringRecover,Year);
    byte infoLength = strlen(songInfo); 
    strncpy(stringRecover,&songInfo[22],22);
    stringRecover[22] = 0;
    sprintf(songInfo + infoLength,"%s",stringRecover); //Add the first 22 chars onto the end of songInfo to make it easier to loop through later 
	                                            //(this ignores the 22 spaces as they are only printed on screen once).
repeatSong: //Repeating has skipped buffering the song file, as it is already in the memory
    printMediaPlayer();
    //Check headphones connected-----------------------------------------------------------------
    attachInterrupt(phonesInt,phonesDisconnect,CHANGE); //Interrupt for detecting whether headphones have been attached/detached
    phones = digitalRead(3);
    //-----------------------------------------------------------------------------------------

    //Start reading the memory from the beginning
    n = 0;

#if (MEMTYPE == 2) || (MEMTYPE == 4)
    readIndex = 0; //Position to start reading at
    writeIndex = 0; //Position to writing reading at
    memory.Command(FLASH_READ,0); //read
    memory.transfer(0); //upper byte address
    memory.transfer(0); //lower byte address
    memory.writeDataEn(0);//Enable data input of the decoder
    memory.writeCLK(0);
    memory.writeHold(0);//hold the first module
    memory.writeHold2(1);//Unhold the second module
    delayMicroseconds(1);
#elif (MEMTYPE == 100)||(MEMTYPE == 0)
    memory.address = 0;
    memory.page = 0;
    memory.sector = 0;
    memory.Command(FLASH_READ,1); //Begin reading the second module (no data will be extracted yet)
    memory.writeCLK(0);
    memory.writeHold2(0);//Hold the second module
    memory.writeHold(1);//Unhold the first module
    memory.writeDataEn(0);//Enable data input of the decoder
    delayMicroseconds(1);
    memory.Command(FLASH_READ,0); //Begin reading the first module.
    //The first memory is now selected at sector 0. We will then count through each sector, alternating module as we go.
    //The flash is now waiting for a clock, and the decoder data recieve is now enabled. The clock will come when the decoder
    //requests data, thus meaning all the arduino has to do is send dummy bytes. It is done this way so that the program can
    //know when all the data has been sent, by keeping track of how many dummy bytes it has sent, but so that data can be sent
    //fast enough to the decoder.
    lastSector = 0;
#elif MEMTYPE == 1
    memory.address = 0;
    memory.page = 0;
    memory.sector = 0;
    memory.Command(FLASH_READ,1); //Begin reading the second module (no data will be extracted yet)
    memory.writeCLK(0);
    memory.writeHold2(0);//Hold the second module
    memory.writeHold(1);//Unhold the first module
    memory.writeDataEn(0);//Enable data input of the decoder
    delayMicroseconds(1);
#endif
    decoder.Buffer[0] = 1; //Data to send: Start Decoding
    decoder.Buffer[8] = 1; //How many bytes: 1
    decoder.Write(114); //Write data to register
    finished = 0;
    bytesSent = 0;
    
    dataRequest(); //Fill the data buffer.

    currentTime = 0;
    playingTime = 0;
    
    play = 1;
    decoder.Buffer[0] = 1; //Data to send: Play
    decoder.Buffer[8] = 1; //How many bytes: 1
    decoder.Write(19); //Write data
    
    attachInterrupt(dataReqInt,dataRequest,RISING); //Then enable interrupt to find out when buffer empty.
    delay(100); //Give the the decoder time to register valid data.

    getPlayingTime();
    currentTime = 1;
    printScroll(0,0,0,interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2],songInfo,infoLength,1);
    strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[8]))); //"           " (11 spaces to blank the scroll bar) 
    graphic.Print(stringRecover,interfaceX[5],interfaceY[0],4);
    for (char i = 31;i < 94;i += 2){
      graphic.Plot(i,4,1);
    }

    char last = 0;
#if (MEMTYPE == 1)||(MEMTYPE == 0)
    unsigned int lastTime = (bytesSent * 26) / (1000 * frameLength);  //Amount Sent/FrameLength * 26mS;
#else
    unsigned int lastTime = (readIndex * 26) / (1000 * frameLength);  //Amount Sent/FrameLength * 26mS;
#endif

    setStates();

    while(!finished){
      if (currentTime > lastTime){ //Update the display as too how far into the song we are.
        printScroll(0,0,0,interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2],songInfo,infoLength,0); 
        lastTime = currentTime;
#if MEMTYPE > 1000
        if (!gameOfLife()){ //Visualisations
          //User turned the media player off (This is the only case which returns FALSE
          goto shutdown;
        }
#endif
      }
      if (!phones){
        //Pause playback because of speaker disconnection
        decoder.Buffer[0] = 0; //Data to send: Pause
        decoder.Buffer[8] = 1; //How many bytes: 1
        decoder.Write(19); //Write data to sequential registers
        if((mediaControl != MUTE) && (mediaControl != VOLUME)){
          graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
          graphic.SetBackColour(0,0,0);
        } else {
          graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
          graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
        }
        oldMediaControl = mediaControl;
        mediaControl = NOSPEAKER;
        char temp[2] = {mediaCharacters[mediaControl],0};
        graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
        graphic.Print(temp,mediaControlX[mediaControl],12,4);
        graphic.SetForeColour(0,0,0);
        graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
        strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[9]))); //"Connect Headphones    "
        graphic.Print(stringRecover,interfaceX[0],interfaceY[14],4);
        unsigned long nextMillis = millis() + 250;
        int i = 0;
        while(!phones){
          checkBattery(); //Check the status of the battery
          if (!songTools()){
            //User turned the media player off (This is the only case which returns FALSE)
            goto shutdown;
          }
          graphic.SetForeColour(0,0,0);
          graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
          
          if(millis() > nextMillis){
            if(i < 3){
              strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[12]))); //"."
              graphic.Print(stringRecover,interfaceX[18 + i],interfaceY[14],4); //Print moving dots to show it is waiting
              i++;
            } else {
              strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[13]))); //"   " (3 spaces to clear the dots)
              graphic.Print(stringRecover,interfaceX[18],interfaceY[14],4);
              i = 0;
            }
            nextMillis = millis() + 250;
          }
        }
        mediaControl = oldMediaControl;
        last = 2;
        graphic.SetBackColour(0,0,0);
        if((oldMediaControl != MUTE) && (oldMediaControl != VOLUME)){
          if (mute){
            mediaControl = MUTE;
            graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
          } else {
            mediaControl = VOLUME;
            graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
          }
        } else {
          graphic.SetBackColour(interfaceColours[4][0],interfaceColours[4][1],interfaceColours[4][2]);
          if (mute){
            mediaControl = MUTE;
            graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
          } else {
            mediaControl = VOLUME;
            graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
          }
        }
        temp[0] = mediaCharacters[mediaControl];
        graphic.Circle(mediaControlX[mediaControl]+2,15,6,4);
        graphic.Print(temp,mediaControlX[mediaControl],12,4);
        mediaControl = oldMediaControl;
      } else if (play){
        if (last != 1){
          decoder.Buffer[0] = 1; //Data to send: Play
          decoder.Buffer[8] = 1; //How many bytes: 1
          decoder.Write(19); //Write data to register
          getPlayingTime();
          printScroll(0,0,0,interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2],songInfo,infoLength,0); 
          last = 1;
        }
      }
      checkBattery(); //Check the status of the battery
      if (!songTools()){
        //User turned the media player off (This is the only case which returns FALSE
        goto shutdown;
      }
    }
    detachInterrupt(phonesInt);
    delay(250); //Give the decoder time to empty its buffer, else you get the last few ms of the previous song when you skip.
    //Stop the decoder decoding. Ideally this would be in the dataRequest ISR, but it won't function there.
    decoder.Buffer[0] = 0; //Data to send: Pause
    decoder.Buffer[8] = 1; //How many bytes: 1
    decoder.Write(19); //Write data to register

    decoder.Buffer[0] = 0; //Data to send: Stop Decoding
    decoder.Buffer[8] = 1; //How many bytes: 1
    decoder.Write(114); //Write data to register


    //All data has been sent, so we can end the Read command.
    memory.writeDataEn(1);//Disable data input of the decoder
    memory.writeCLK(0);
    memory.writeHold(1);//Unhold 1st module, ready for programming
    memory.writeHold2(1);//Unhold 2nd module, ready for programming
    delayMicroseconds(2);
    memory.writeSS(1);//End read
    memory.writeSS2(1);//End read

    if(Repeat){
#if MEMTYPE > 1
      goto repeatSongT2; //Different for type 2 as whole song not stored in onboard memory
#else
      goto repeatSong;
#endif
    }
  } while(numberPlayed < numberOfSongs);
  return 1;

shutdown:
  detachInterrupt(dataReqInt);
  detachInterrupt(phonesInt);
  play = 0;
  decoder.Buffer[0] = play; //Data to send: Pause
  decoder.Buffer[8] = 1; //How many bytes: 1
  decoder.Write(19); //Write data to register
        
  decoder.Buffer[0] = 1; //Data to send: Mute
  decoder.Buffer[8] = 1; //How many bytes: 1
  decoder.Write(20); //Write data to register

  decoder.Buffer[0] = 0; //Data to send: Stop Decoding
  decoder.Buffer[8] = 1; //How many bytes: 1
  decoder.Write(114); //Write data to register

  //All data has been sent, so we can end the Read command.
  memory.writeDataEn(1);//Disable data input of the decoder
  memory.writeCLK(0);
  memory.writeHold(1);//Unhold 1st module, ready for programming
  memory.writeHold2(1);//Unhold 2nd module, ready for programming
  delayMicroseconds(2);
  memory.writeSS(1);//End read
  memory.writeSS2(1);//End read

  return 0;
}
//---------------------------------------------------------------------------------------------------------

//Check the status of the battery pack---------------------------------------------------------------------
void batterySymbol(int charge){
  graphic.SetForeColour(15,15,15);
  graphic.SetBackColour(0,0,0);
  graphic.Box(86,9,112,21,4);
  
  if (charge == 0){
    //Using this to mean it is charging
    graphic.SetForeColour(interfaceColours[3][0],interfaceColours[3][1],interfaceColours[3][2]);
    //Draw out the battery symbol
    for(byte i = 0;i < 23;i++){
      for(byte j = 0;j < 8;j++){
        if(pgm_read_byte(&(battery[i])) & (1 << j)){ //If the jth bit in the ith byte of the battery symbol is a 1
          graphic.Plot(interfaceX[13] + 10 + i,interfaceY[1] - 1 + j,3);
        }
      }
    }
  } else {
    if (charge > 675){
      graphic.SetForeColour(interfaceColours[0][0],interfaceColours[0][1],interfaceColours[0][2]);
    } else {
      //If running low, then red
      graphic.SetForeColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
    }
    //Draw out the battery symbol in light red
    for(byte i = 0;i < 23;i++){
      for(byte j = 0;j < 8;j++){
        if(pgm_read_byte(&(battery[i])) & (1 << j)){ //If the jth bit in the ith byte of the battery symbol is a 1
          graphic.Plot(interfaceX[13] + 10 + i,interfaceY[1] - 1 + j,3);
        }
      }
    }
    if(charge < 550){
      charge = 0; //This bit is just in case
    } else {
      charge -= 550; //The mimimum possible value is 550, so by subtracting it minimum becomes 0;
    }
    charge /= 19; //There are 14 bars, each of which I have calculated will represent a range of 19 values ((max - min)/14 ~= 19)
    charge += 14 + interfaceX[13]; //Convert to x co-ordinate (the 14 here has nothing to do with the 14 above!)
    
    //Draw how much of the battery is charged
    graphic.Box(interfaceX[13] + 14,interfaceY[1] + 1,charge++,interfaceY[1] + 4,5);
    graphic.SetForeColour(0,0,0);
    graphic.Box(charge,interfaceY[1] + 1,interfaceX[13] + 27,interfaceY[1] + 4,5);
  }
}

void checkBattery(){
  static boolean lastOneHz = false;
  static byte count = 0;
  static boolean volSym = true;
  static unsigned long millisDelay = 1000; //initial delay of 1 second 
  boolean battCharge = digitalRead(ChargePin); //Will be high when a charger is plugged in
  int battLevel = analogRead(LevelPin); //Reads the voltage level of the battery
  boolean battLow; 
  if(battLevel < 675){
    //615 equates to 3.1v, which is about the point where a LiPo battery should be charged (It can go to 2.6V before power is shut off to prevent over discharge).
    battLow = true;
  } else {
    //Battery level is acceptable
    battLow = false;
  }
  if(!play){
    //If paused, use millis to create the 1 Hz variable
    if(millis() > millisDelay){
      oneHz = !oneHz;
      millisDelay = millis() + 1000; //current time plus a one second delay
    }
  }
  if(lastOneHz != oneHz){
    //True once approximately every second while song is playing
    lastOneHz = oneHz; 
    
    if(screenDimCount >= TIMEOUT){
      analogWrite(BacklightPWMPin,dimmedBrightness); //Auto dim the screen after 60 seconds of no user activity to save power
    } else {
      screenDimCount++; 
    }
    
    if (battCharge){
      //Charging takes priority
      if(count & 2){
        //True for four seconds in every eight
        if(!volSym){
          //Only print if volume not currently printed
          volumeGraph();
          volSym = true;
        }
      } else if(count & 4){
        //Ture for two seconds in eight
        if(volSym){
          //Only print if battery not currently printed
          batterySymbol(battLevel); //Half the time show the current level
          volSym = false;
        }
      } else {
        if(volSym){
          //Only print if battery not currently printed
          batterySymbol(); //and the other half indicate charging
          volSym = false;
        }
      }
      count++;
      if(screenDimCount < TIMEOUT){
        analogWrite(BacklightPWMPin,brightness); //Set screen to normal brightness
      }
    } else if(battLow){
      if(count & 1){
        //Ture for Odd seconds
        batterySymbol(battLevel);
        volSym = false;
      } else {
        volumeGraph();
        volSym = true;
      }
      count++;
      if(screenDimCount < TIMEOUT){
        analogWrite(BacklightPWMPin,dimmedBrightness); //Set screen to dimmed brightness
      }
    } else {
      if(count & 6){
        //True for six seconds in every eight
        if(!volSym){
          //Only print if volume not currently printed
          volumeGraph();
          volSym = true;
        }
      } else {
        //Ture for two seconds in eight
        if(volSym){
          //Only print if battery not currently printed
          batterySymbol(battLevel); //Show the current level
          volSym = false;
        }
      }
      count++;
      if(screenDimCount < TIMEOUT){
        analogWrite(BacklightPWMPin,brightness); //Set screen to normal brightness
      }
    }
  } 
}
//---------------------------------------------------------------------------------------------------------

//Buffer MP3 data from SD card to flash memory chips-------------------------------------------------------
void bufferSong(){ //Fixed for alternating RAM
  //Write the file to the flash memory
  graphic.SetForeColour(0,0,0);
  graphic.SetBackColour(interfaceColours[1][0],interfaceColours[1][1],interfaceColours[1][2]);
  strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[0]))); //"Loading Song File"
  graphic.Print(stringRecover,interfaceX[0],interfaceY[14],4);
#if (MEMTYPE == 2) || (MEMTYPE == 4)
  byte dataArray[512];
  unsigned int bytestosend = 32768; //256kbits block to initially flood the external SRAM
  byte scrollBar = 2;
  strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3])));
  graphic.Print(stringRecover,0,interfaceY[0],4);
  for (byte i = 1;i < 130;i += 2){
    graphic.Plot(i,4,1);
  }
  graphic.Box(0,0,4,7,5);
  n = 0;
  byte i = 0;
  memory.writeDataEn(1); //disable decoder
  memory.writeCLK(0); //Lower Clock
  memory.writeHold(1); //unhold
  memory.writeHold2(0); //hold
  memory.Command(FLASH_PP,0); //TYPE 2 write command
  memory.transfer(0); //Address is 0
  memory.transfer(0); //Address is 0
  memory.writeCLK(0); //Lower Clock
  memory.writeHold(0); //Hold
  while (bytestosend > 0){
    Song.read(dataArray,512); //Retrieve a 1kB block
    memory.writeCLK(0); //Lower Clock
    memory.writeHold(1); //unhold
    for(int j = 0;j < 512;j++){
      memory.transfer(dataArray[j]); //Send the data
    }
    memory.writeCLK(0); //Lower Clock
    memory.writeHold(0); //Hold
    bytestosend -= 512; //One block sent
    graphic.Box(scrollBar,0,scrollBar + 2,7,5); //Draw the scroll bar
    scrollBar += 2;
    if (i == 5){
      i = 0;
      strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[14])));
      graphic.Print(stringRecover,interfaceX[17],interfaceY[14],4);
    } else {
      strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[12])));
      graphic.Print(stringRecover,interfaceX[17 + i],interfaceY[14],0); //Print moving dots to show it is waiting
      i++;
    }
  }
  memory.writeCLK(0); //Lower Clock
  memory.writeHold(1); //unhold
  memory.writeSS(1);  //Finish Transaction
#elif MEMTYPE == 1
  long bytestosend = songLength;
  byte scrollWidth  = ((128 * 65536L) / songLength);//Number of pixels / Number of sectors = 128/(Length/65536)
  if ((128 * 65536L) % songLength){
    scrollWidth++; //Rounds up
  }
  strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3])));
  graphic.Print(stringRecover,0,interfaceY[0],4);
  for (byte i = 1;i < 130;i += 2){
    graphic.Plot(i,4,1);
  }
  graphic.Box(0,0,scrollWidth,7,5);
  memory.sector = 0;
  memory.page = 0;
  memory.address = 0;
  n = 0;
  
  byte dataArray[4096]; //Large array buffering data from SD card - used for songs and bitmaps
  scrollWidth++; //Accounts for the faster programming speed.
  unsigned int arrayPointer = 0; 
  memory.Command(FLASH_BE,1); //Bulk erase takes only 50ms on type 1, so might as well do that instead of sector erase.   
  while (bytestosend > 0){
    //Loop for the pages in this sector
    do{
      if(!(memory.page % 16)){
        //every 16th page, get 16 pages of data starting from the first - This means striping 4kB blocks over the memory
        Song.read(dataArray,4096);
        arrayPointer = 0;
      }
      do{
        memory.Command(FLASH_RDSR,1);
      } while(memory.flashData[256] & 1); //Wait for first memory to not be busy before doing anything else
      memory.Command(FLASH_PP,1);    //Call Page Program, CS left low ready for address and data
      memory.transfer(memory.sector);
      memory.transfer(memory.page);
      memory.transfer(memory.address);
      do{
        memory.transfer(dataArray[arrayPointer++]);//Transfer the next byte in the file.
      } while(arrayPointer % 256); //loop 256 times
      memory.writeSS2(1);  //Finish Transaction
    } while(++memory.page); //memory.page will equal 0 when it rolls over - i.e. the end of the sector
    bytestosend -= 65536; //1 sector sent.
    memory.sector++; //Move to next sector
            
    unsigned int scrollBar = ((128 * (songLength - bytestosend))/songLength); //Calculate the % complete and how that corresponds to a 128 pixel wide bar
    graphic.Box(scrollBar - 1,0,scrollBar + scrollWidth,7,5); //Draw the scroll bar
  }
  //Make sure memory chip not busy before returning to media player
  do{
    memory.Command(FLASH_RDSR,1);
  }while(memory.flashData[256] & 1); //Wait for second memory to not be busy before doing anything else  
#elif MEMTYPE == 100
  long bytestosend = songLength;
  byte scrollWidth  = ((128 * 65536L) / songLength);//Number of pixels / Number of sectors = 128/(Length/65536)
  if ((128 * 65536L) % songLength){
    scrollWidth++; //Rounds up
  }
  strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3])));
  graphic.Print(stringRecover,0,interfaceY[0],4);
  for (byte i = 1;i < 130;i += 2){
    graphic.Plot(i,4,1);
  }
  graphic.Box(0,0,scrollWidth,7,5);
  memory.sector = 0;
  memory.page = 0;
  memory.address = 0;
  n = 0;
  
  byte dataArray[2][2048]; //Large array buffering data from SD card - used for songs and bitmaps
  scrollWidth++; //Accounts for the faster programming speed.
  unsigned int arrayPointer[2] = {0,0}; 
  memory.Command(FLASH_BE,0); //Bulk erase takes only 50ms on type 1, so might as well do that instead of sector erase.
  memory.Command(FLASH_BE,1); //Bulk erase second module        
  while (bytestosend > 0){
    //Loop for the pages in this sector
    do{
      //loop for each memory chip
      for(byte j = 0;j < 2;j++){
        if(!(memory.page % 8)){
          //every 8th page, get 8 pages of data starting from the first - This means striping 2kB blocks over the memory
          Song.read(dataArray[j],2048);
          arrayPointer[j] = 0;
        }
        do{
          memory.Command(FLASH_RDSR,j);
        } while(memory.flashData[256] & 1); //Wait for first memory to not be busy before doing anything else
        memory.Command(FLASH_PP,j);    //Call Page Program, CS left low ready for address and data
        memory.transfer(memory.sector);
        memory.transfer(memory.page);
        memory.transfer(memory.address);
        do{
          memory.transfer(dataArray[j][arrayPointer[j]++]);//Transfer the next byte in the file.
        } while(arrayPointer[j] % 256); //loop 256 times
        if(j){
          memory.writeSS2(1);  //Finish Transaction
        } else {
          memory.writeSS(1);  //Finish Transaction
        }
      }
    } while(++memory.page); //memory.page will equal 0 when it rolls over - i.e. the end of the sector
    bytestosend -= 131072; //2 sectors sent.
    memory.sector++; //Move to next sector
            
    unsigned int scrollBar = ((128 * (songLength - bytestosend))/songLength); //Calculate the % complete and how that corresponds to a 128 pixel wide bar
    graphic.Box(scrollBar - 1,0,scrollBar + scrollWidth,7,5); //Draw the scroll bar
  }
  //Make sure both memory chips are not busy before returning to media player
  do{
    memory.Command(FLASH_RDSR,0);
  }while(memory.flashData[256] & 1); //Wait for first memory to not be busy before doing anything else
  do{
    memory.Command(FLASH_RDSR,1);
  }while(memory.flashData[256] & 1); //Wait for second memory to not be busy before doing anything else
#elif MEMTYPE == 0
  long bytestosend = songLength;
  byte scrollWidth  = ((128 * 65536L) / songLength);//Number of pixels / Number of sectors = 128/(Length/65536)
  if ((128 * 65536L) % songLength){
    scrollWidth++; //Rounds up
  }
  strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[3]))); //blank line
  graphic.Print(stringRecover,0,interfaceY[0],4);
  for (byte i = 1;i < 130;i += 2){
    graphic.Plot(i,4,1);
  }
  graphic.Box(0,0,scrollWidth,7,5);
  byte i = 0;
  memory.sector = 0;
  memory.page = 0;
  memory.address = 0;
  n = 0;
  
  memory.Command(FLASH_SE,0); //Erase the very first sector
  do{
    memory.Command(FLASH_RDSR,0);
  }while(memory.flashData[256] & 1); //Wait for command to complete

  //Read file, and store in memory
  while (bytestosend > 0){
    if (n){
      memory.sector++; //Look ahead one sector
      memory.Command(FLASH_SE,0); //Begin the erase of the next sector
      memory.sector--;
    } else { 
      memory.Command(FLASH_SE,1); //Begin the erase of the next sector
    }
    //Get the first page worth of data while waiting for the erase to complete.
    Song.read(memory.flashData,256);
    //Do a quick check to make sure the erase of the current sector was completed - It should be by now.
    do{
      memory.Command(FLASH_RDSR,n);
    }while(memory.flashData[256] & 1);
    //Now loop for the pages in this sector
    do{
      memory.Command(FLASH_PP,n);    //Call Page Program, CS left low ready for address and data
      memory.transfer(memory.sector);
      memory.transfer(memory.page++);
      memory.transfer(memory.address);
      do{
        memory.transfer(memory.flashData[memory.address++]);//Transfer the next byte in the file.
      }while(memory.address);
      if (n){
        memory.writeSS2(1);  //Finish Transaction
      } else {
        memory.writeSS(1);  //Finish Transaction
      }

      if (memory.page){ //When page = 0, we don't want new data
        Song.read(memory.flashData,256);//Get the page data from the SD card while waiting for programming to finish
      }

      do{
        memory.Command(FLASH_RDSR,n);
      }while(memory.flashData[256] & 1);

    }while (memory.page);
    bytestosend -= 65536; //1 sector sent. If beyond the end of the file, then we will have programmed some extra 0xFF's,
    //but this will have no ill effect.
    if (n == 0){
      n = 1; //Switch to second RAM (same sector)
    } else {
      memory.sector++; //Move to next sector
      n = 0; //Switch back to first RAM
    }
    unsigned int scrollBar = ((128 * (songLength - bytestosend))/songLength); //Calculate the % complete and how that corresponds to a 128 pixel wide bar
    graphic.Box(scrollBar,0,scrollBar +  scrollWidth,7,5); //Draw the scroll bar
    if (i == 5){
      i = 0;
      strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[14])));
      graphic.Print(stringRecover,interfaceX[17],interfaceY[14],4);
    } else {
      strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[12])));
      graphic.Print(stringRecover,interfaceX[17 + i],interfaceY[14],0); //Print moving dots to show it is waiting
      i++;
    }
  }
#endif
}
//---------------------------------------------------------------------------------------------------------

//Trackerball Code ----------------------------------------------------------------------------------------
void setStates(){
  upState = PINK & 64;
  upCount = 0;
  downState = PINK & 32;
  downCount = 0;
  leftState = PINK & 16;
  leftCount = 0;
  rightState = PINK & 8;
  rightCount = 0;
  buttonState = PINK & 4;
  buttonState = 4 - buttonState;

  PCICR |= (1 << PCIE2); //Enable Pin Change Interrupt
}

ISR( PCINT2_vect ){
  if(screenDimCount >= TIMEOUT){
    screenDimCount = 0;
    analogWrite(BacklightPWMPin,brightness); //If dimmed, light it up
  }
  //Trackerball Interrrupt code
  if ((PINK & 64) != upState){
    //Up has changed, so increase counter
    upCount++;
    upState = 64 - upState;
  }
  if ((PINK & 32) != downState){
    //Down has changed, so increase counter
    downCount++;
    downState = 32 - downState;
  }
  if ((PINK & 16) != leftState){
    //Left has changed, so increase counter
    leftCount++;
    leftState = 16 - leftState;
  }
  if ((PINK & 8) != rightState){
    //Right has changed, so increase counter
    rightCount++;
    rightState = 8 - rightState;
  }
  if ((PINK & 4) == buttonState){
    //Button has changed
    buttonState = 4 - buttonState;
  }
}
//---------------------------------------------------------------------------------------------------------

//Tag Decoding---------------------------------------------------------------------------------------------

char* trimString(char source[]){
  for(byte len = 31;len > 0;len--){
    if((source[len - 1] != ' ')&&(source[len - 1] != 0)&&(source[len - 1] != 255)){ //found a non whitespace character?
      source[len] = 0; //New null character is the one after the found whitespace
      return source;
    }
  }
  source[0] = 0;
  return source;
}

boolean validateSongFile(char* songName, boolean buffer){
  if (!SD.exists(songName)){
    return 0; //File Error
  }

  Song = SD.open(songName);
  songLength = Song.size();    

  if ((songLength < 0x400)||(songLength > 0xFFFFFF)){
    //File is either too big, or too small (MP3 Files must be at least 1024 bytes long) so we can close it
    Song.close(); 
    return 0; //File Error
  }

  //ID3 tag is the last 128bytes of the song 
  songLength = songLength - 128;
  Song.seek(songLength);

  //Read the TAG information (We will always use flashData for non song data to make it easier to follow)
  Song.read(memory.flashData,1);
  if (memory.flashData[0] == 'T'){
foundT:
    Song.read(memory.flashData,1);  //Look for A
  } 
  else {
    Song.read(memory.flashData,1);  //Look once more for T
    if (memory.flashData[0] == 'T'){
      goto foundT;
    }
  }
  if (memory.flashData[0] == 'A'){
    Song.read(memory.flashData,1); //Look for G
    if (memory.flashData[0] == 'G'){
      Song.read(memory.flashData,125); //Get 125 bytes of TAG data
      goto loadTag;
    }
  }
  //No TAG was found, so everything is Unknown
  strcpy_P(Artist,(char*)pgm_read_word(&(messages[16]))); //"Unknown"
  strcpy_P(Title,(char*)pgm_read_word(&(messages[16]))); //"Unknown"
  strcpy_P(Album,(char*)pgm_read_word(&(messages[16]))); //"Unknown"
  
  strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[16]))); //"Unknown"
  strcpy_P(Genre,(char*)pgm_read_word(&(messages[5]))); //"Gen: " 
  sprintf(Genre + strlen(Genre),"%s",stringRecover); //"Gen: Unknown"
  
  strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[23]))); //"0000"
  strcpy_P(Year,(char*)pgm_read_word(&(messages[7]))); //"Year: " 
  sprintf(Year + strlen(Year),"%s",stringRecover);
  
  strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[18]))); //"-" 
  strcpy_P(Track,(char*)pgm_read_word(&(messages[6]))); //"Trk: " 
  sprintf(Track + strlen(Track),"%s%s",stringRecover,stringRecover); //"Trk: --"

  if (buffer){
    Song.seek(0);
    bufferSong();
  }

#if MEMTYPE < 2
  Song.close();
#endif
  return 1;


  //Tag found and read, so decode information
loadTag:
  //strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[17]))); //Single Space
  strncpy(stringRecover,(char*)&memory.flashData[0],30); //Characters from Tag
  stringRecover[30] = 0; //NULL char
  strcpy(Title,trimString(stringRecover));
  
  strncpy(stringRecover,(char*)&memory.flashData[30],30); //Characters from Tag
  stringRecover[30] = 0; //NULL char
  strcpy(Artist,trimString(stringRecover));
  
  strncpy(stringRecover,(char*)&memory.flashData[60],30); //Characters from Tag
  stringRecover[30] = 0; //NULL char
  strcpy(Album,trimString(stringRecover));
  
  strcpy_P(Year,(char*)pgm_read_word(&(messages[7]))); //"Year: " 
  strncpy(stringRecover,(char*)&memory.flashData[90],4); //Characters from Tag
  stringRecover[4] = 0; //NULL char
  sprintf(Year + strlen(Year),"%s",stringRecover); //"Year: ####"
  
  //Check if track number is stored
  strcpy_P(Track,(char*)pgm_read_word(&(messages[6]))); //"Trk: " 
  if (memory.flashData[122] == 0){
    //Track number is stored in following byte
    sprintf(Track + strlen(Track),"%02d",memory.flashData[123]); //"Trk: ##"
  } else {
    //Track number is not stored
    strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[18]))); //"-"
    sprintf(Track + strlen(Track),"%s%s",stringRecover,stringRecover); //"Trk: --"
  }

  //WinAMP uses an extended Genre list, where genre index can be more than 80. If one of those is used, this program can't decode it.
  strcpy_P(Genre,(char*)pgm_read_word(&(messages[5]))); //"Gen: " 
  if (memory.flashData[124] < 80){
    strcpy_P(stringRecover,(char*)pgm_read_word(&(Genres[memory.flashData[124]]))); //Get the genre from program memory
    sprintf(Genre + strlen(Genre),"%s",stringRecover); //"Gen: &&&&&&"
  } else {
    strcpy_P(stringRecover,(char*)pgm_read_word(&(messages[16]))); //Genre unknown or unsupported
    sprintf(Genre + strlen(Genre),"%s",stringRecover); //"Gen: Unknown"
  }

  if (buffer){
    Song.seek(0);
    bufferSong();
  }
#if (MEMTYPE != 2) && (MEMTYPE != 4)
  Song.close();
#endif
  return 1;
}
//---------------------------------------------------------------------------------------------------------

//ISR to send song data to the decoder upon request--------------------------------------------------------
void dataRequest(){
#if (MEMTYPE == 2) || (MEMTYPE == 4)
  memory.writeCLK(0);
  memory.writeHold(1); //unhold
  memory.writeHold2(0); //hold
  memory.Command(FLASH_READ,0); //read
  memory.transfer((readIndex >> 8) & 0x7F); //upper byte address
  memory.transfer(readIndex & 0xFF); //lower byte address
  memory.writeDataEn(0);//Renable data input of the decoder
  delayMicroseconds(5);
  //Transfer more data to the Decoder while it is requesting it or stop when all data sent.
  while ((readIndex <= songLength)&&(PINE & 16)) {
    memory.transfer(0xFF); //Creates a clock for transfer of Flash data to decoder
    readIndex++; //increment address for future reference
  }
  memory.writeSS(1);//End session
  memory.writeDataEn(1);//Disable data input of the decoder
  
  currentTime = (readIndex * 26)/(1000 * frameLength);
  if (readIndex > songLength){ //Song complete
    //If this is true, then last time data was requested, the rest of the song had been sent. Thus the buffer is now
    //empty, and we can stop.
    play = 0; //Pause playback
    readIndex = 0;     
    finished = 1; //Change pause to Stop
    detachInterrupt(dataReqInt); 
    Song.close(); //Finished with the song file
  } else { //More data to send, so refill buffer
    byte dataArray[512];
    unsigned int blockSize = (readIndex - writeIndex);
    memory.Command(FLASH_PP,0); //write
    memory.transfer((writeIndex >> 8) & 0x7F); //upper byte address
    memory.transfer(writeIndex & 0xFF); //lower byte address
    writeIndex = readIndex;
    while(blockSize > 0){
      if(blockSize >= 512){
        //Start with buffering blocks of 1kB
        memory.writeCLK(0); //Lower Clock
        memory.writeHold(0); //Hold
        Song.read(dataArray,512);
        memory.writeCLK(0); //Lower Clock
        memory.writeHold(1); //unhold
        for(int i = 0;i < 512;i++){
          memory.transfer(dataArray[i]); //Send new data to fill up emptied space
        }
        blockSize -= 512;
      } else {
        //Finish with a block of the remaining data if less than 1kB
        memory.writeCLK(0); //Lower Clock
        memory.writeHold(0); //Hold
        Song.read(dataArray,blockSize);
        memory.writeCLK(0); //Lower Clock
        memory.writeHold(1); //unhold
        for(int i = 0;i < blockSize;i++){
          memory.transfer(dataArray[i]); //Send new data to fill up emptied space
        }
        blockSize = 0;
      }
    }
    memory.writeSS(1);//End session
    memory.writeCLK(0);
    memory.writeHold2(1);//End hold (2)
    memory.writeHold(0);//Begin Hold (1)
  }
#elif (MEMTYPE == 100)||(MEMTYPE == 0)
  //Transfer more data to the Decoder while it is requesting it or stop when all data sent.
  while ((bytesSent <= songLength)&&(PINE & 16)) {
    memory.transfer(0xFF); //Creates a clock for transfer of Flash data to decoder
    bytesSent++;
    
    currentSector = bytesSent & stripeSize; //This will change after a certain amount of data sent.
    if (currentSector > lastSector){ //Swap module
      memory.writeCLK(0);
      if (n){
        memory.writeHold2(0);//Begin Hold (2)
        memory.writeHold(1);//End hold (1)
        n = 0;
      } else {
        memory.writeHold(0);//Begin Hold (1)
        memory.writeHold2(1);//End hold (2)
        n = 1;
      }
      lastSector = currentSector;
    }
  }

  currentTime = (bytesSent * 26)/(1000 * frameLength);

  if (bytesSent >= songLength){ //Song complete
    //If this is true, then last time data was requested, the rest of the song had been sent. Thus the buffer is now
    //empty, and we can stop.
    play = 0; //Pause playback
    bytesSent = 0;     
    finished = 1; //Change pause to Stop
    detachInterrupt(dataReqInt); 
  }
#elif MEMTYPE == 1
  memory.writeCLK(0);
  memory.writeHold(0);//Begin Hold (1)
  memory.writeDataEn(0);//Renable data input of the decoder
  delayMicroseconds(5);
  memory.writeHold2(1);//End hold (2)
  //Transfer more data to the Decoder while it is requesting it or stop when all data sent.
  while ((bytesSent <= songLength)&&(PINE & 16)) {
    memory.transfer(0xFF); //Creates a clock for transfer of Flash data to decoder
    bytesSent++;
  }
  memory.writeCLK(0);
  memory.writeHold2(0);//Begin Hold (2)
  memory.writeHold(1);//End hold (1)
  memory.writeDataEn(1);//disable data input of the decoder

  currentTime = (bytesSent * 26)/(1000 * frameLength);

  if (bytesSent >= songLength){ //Song complete
    //If this is true, then last time data was requested, the rest of the song had been sent. Thus the buffer is now
    //empty, and we can stop.
    play = 0; //Pause playback
    bytesSent = 0;     
    finished = 1; //Change pause to Stop
    detachInterrupt(dataReqInt); 
  }
#endif
}
//---------------------------------------------------------------------------------------------------------

//ISR handles headphones being unplugged-------------------------------------------------------------------
void phonesDisconnect(){
  if(!(PINE & 32)){ //Same as !digitalRead(3), only ALOT faster.
    //Phones Disconnected
    detachInterrupt(dataReqInt); //Ignore requests for more data just in case
    phones = 0;
  } else {
    //Phones Reconnected
    if(!finished){ //Only resume if there is more to play
      attachInterrupt(dataReqInt,dataRequest,RISING); //Start listening for more data again.      
    }
    phones = 1;
  }
}
//---------------------------------------------------------------------------------------------------------

//Print song information/playing time, during playback-----------------------------------------------------
void printScroll(char Rf, char Gf, char Bf, char Rb, char Gb, char Bb, char* songInfo, byte infoLength, boolean start){
  static byte tagPosition = 0;
  if(start){
    tagPosition = 0;
  }
  
  graphic.SetForeColour(Rf,Gf,Bf);
  graphic.SetBackColour(Rb,Gb,Bb);

  unsigned int time2Go = playingTime - currentTime; //How long is left
  unsigned int currentSec = currentTime % 60; //Seconds Played
  unsigned int currentMin = currentTime / 60; //Minutes Played
  unsigned int sec2Go = time2Go % 60; //Seconds left
  signed int min2Go = 0 - (time2Go / 60); //Minutes left
  
  unsigned int scrollBar = ((64 * currentTime) / playingTime) + 30;
  byte scrollWidth  = (64 / playingTime);//Number of pixels / Number of divisions = 64/playingTime
  if (64 % songLength){
    scrollWidth++; //Rounds up
  }
  
  sprintf(stringRecover,"%2d:%02d",currentMin,currentSec); //convert current time to string
  graphic.Print(stringRecover,0,0,4);
  sprintf(stringRecover,"%3d:%02d",min2Go,sec2Go); //convert time remaining to string
  graphic.Print(stringRecover,94,0,4);
  
  graphic.Box(scrollBar,0,scrollBar +  scrollWidth,7,5); //Draw the scroll bar

  strncpy(stringRecover,&songInfo[tagPosition],22); //22 character substring starting at [tagPosition]
  stringRecover[22] = 0; //NULL character
  graphic.Print(stringRecover,0,122,4);
  //toPrint = songInfo.substring((tagPosition - 22),tagPosition); //Get 22 characters to print from the current position.

  tagPosition += 2;
  if (tagPosition >= infoLength){ //Back to the beginning of the data (excludes the 22 spaces at the beginning)
    tagPosition = 22;
  }

  //graphic.Print(toPrint,0,122,4);
  
  oneHz = !oneHz; //This creates a 1Hz (ish) signal that can be used by other parts of the program to avoid using the millis() function <- which the ISR's are messing around with.
}
//---------------------------------------------------------------------------------------------------------

//Get Playing Time-----------------------------------------------------------------------------------------
void getPlayingTime(){

  decoder.Read(67,3);
  ID = (decoder.Buffer[0] & 0x08) >> 3;
  IDex = (decoder.Buffer[0] & 0x10) >> 4;
  sampleRateCode = (decoder.Buffer[1] & 0x0C) >> 2;
  bitRateCode = (decoder.Buffer[1] & 0xF0) >> 4;

  frameLength = pgm_read_word(&(bitRate[bitRateCode][ID])); //This has to be added seperately, otherwise it thinks 144 * 32000 = 4?!?!?!?!
  frameLength *= 144;
  frameLength /= (pgm_read_word(&(sampleRate[sampleRateCode][ID])) * (IDex + 1));  //FrameLengthInBytes = (144 * BitRate) / SampleRate.
  //The (IDex + 1) corrects space saving in the lookup table
  //SongLength/FrameLength * 26mS 
  playingTime = (songLength * 26) / (1000 * frameLength);             

}
//---------------------------------------------------------------------------------------------------------

//Visualistations (Conway's Game of Life)
boolean gameOfLife(){
  memory.sector = storeIndex;
  memory.page = 0;
  memory.address = 0;
  memory.Command(FLASH_SE,1);
  do{
    memory.Command(FLASH_RDSR,1);
    if (!songTools()){ //Here to avoid slowing down the user interface too much
      //User turned the media player off (This is the only case which returns FALSE
      return 0;
    }
    if(finished == 1){
      return 1; //Finished the song
    }
  }while(memory.flashData[256] & 1);
  
  storeIndex = 1 - storeIndex;
  for(byte i = 1;i < 95;i++){ //count through each row
    byte dataArray[4][128];
    if (!songTools()){ //Here to avoid slowing down the user interface too much
      //User turned the media player off (This is the only case which returns FALSE
      return 0;
    }
    if(finished == 1){
      return 1; //Finished the song
    }
    for(byte j = 0;j < 3;j++){
      memory.sector = storeIndex;
      memory.Command(FLASH_READ,1);
      memory.transfer(memory.sector);
      memory.transfer(memory.page);
      memory.transfer(memory.address);
      for(byte k = 0;k < 128;k++){
        dataArray[j][k] = memory.transfer(0x00);//Dummy byte to get data back
        if((k == 0) && (dataArray[j][k] == 254)){ //remove the no change warning
          dataArray[j][k] = 255;
        }
      }
      memory.writeSS2(1);//End transaction
      memory.address += 128;
      if(!memory.address){
        memory.page++;
      }
    }
    for(byte j = 0;j < 128;j++){
      dataArray[3][j] = dataArray[1][j]; //Store the current states in the temporary section
    }
    for(byte j = 1;j < 127;j++){ //Count through each cell in the row
      byte nearCells = 0;
      if(dataArray[3][j] == 15){
        dataArray[3][j] = 255; //Cell dies of old age
      } else if ((dataArray[3][j] > 0) && (dataArray[3][j] != 255)){
        dataArray[3][j]++; //Cell gets older
      } else {
        for(byte k = 0;k < 3;k++){
          for(char m = -1;m < 2;m++){
            if((k == 1) && (m == 0)){
              m++; //Skip the centre of the 3x3 as it is the cell we are considering
            }
            char n = j + m;
            /*
            if (n == 0){
              n = 126;
            } else if (n == 127){
              n = 1;
            }
            */
            if(dataArray[k][n] == 0){
              nearCells++;
            }
          }
        }
        if(dataArray[3][j] == 0){ //current cell is alive
          if(nearCells == 2){
            dataArray[3][j] = 0;
          } else {
            dataArray[3][j]++; //Cell gets older
          }
        } else {
          if((nearCells == 1) || (nearCells == 3)){
            dataArray[3][j] = 0; //cell is born
          }
        }
      }
    }
    
    
    if(!strncmp((char*)dataArray[3],(char*)dataArray[1],128)){
      dataArray[3][0] = 254;//Line hasn't changed, so skip
    }
    
    memory.sector = 1 - storeIndex;
    memory.page--; //Move back a page for next time (needed for memory saving technique)
    memory.Command(FLASH_PP,1);
    memory.transfer(memory.sector);
    memory.transfer(memory.page);
    memory.transfer(memory.address);
    for(byte j = 0;j < 128;j++){
      memory.transfer(dataArray[3][j]); //program in the new lives
    }
    memory.writeSS2(1);//End transaction
    do{
      memory.Command(FLASH_RDSR,1);
    }while(memory.flashData[256] & 1);
  }
  //Next generation now calculated, so time to update the screen
  if(!displayBitmap()){
    return 0;
  }  
  return 1;
}

boolean displayBitmap(){
  storeIndex = 1 - storeIndex;
  for(byte i = 1;i < 95;i++){ //count through each row
    if (!songTools()){ //Here to avoid slowing down the user interface too much
      return 0; //User turned the media player off (This is the only case which returns FALSE
    }
    if(finished == 1){
      return 1; //Finished the song
    }
    byte dataArray[4][128];
    //First get the ages of the cells in this row
    memory.sector = storeIndex;
    memory.page = (i >> 1);
    memory.address = (i & 1) << 7;
    memory.Command(FLASH_READ,1);
    memory.transfer(memory.sector);
    memory.transfer(memory.page);
    memory.transfer(memory.address);
    for(byte k = 0;k < 128;k++){
      dataArray[0][k] = memory.transfer(0x00);//Dummy byte to get data back
      dataArray[0][k]++;
    }
    memory.writeSS2(1);//End transaction
    if(dataArray[0][0] == 255){
      continue; //line hasn't changed, so don't bother updating it.
    }
    
    //Then get the image data for that line
    memory.sector = 2;
    memory.page = i + 160;
    memory.address = 0;
    memory.Command(FLASH_READ,1);
    memory.transfer(memory.sector);
    memory.transfer(memory.page);
    memory.transfer(memory.address);

    for(byte k = 2;k < 127;){
      byte getByte = memory.transfer(0x00);//Dummy byte to get data back
      dataArray[1][k] = (getByte & 0xF0) >> 4;
      dataArray[2][k] = getByte & 0x0F;
      getByte = memory.transfer(0x00);//Dummy byte to get data back
      dataArray[3][k++] = (getByte & 0xF0) >> 4;
      dataArray[1][k] = getByte & 0x0F;
      getByte = memory.transfer(0x00);//Dummy byte to get data back
      dataArray[2][k] = (getByte & 0xF0) >> 4;
      dataArray[3][k++] = getByte & 0x0F;
    }
    for(byte k = 1;k < 4;k++){
      dataArray[k][1] = dataArray[k][2];
    }

    memory.writeSS2(1);//End transaction
    
    //Now we need to age the image data and print it to the screen
    graphic.Window(2,119 - i,127,119 - i);
    for (byte j = 1;j < 127;j += 2){
      if((dataArray[0][j] > 0) && (dataArray[0][j] < 17)){
        for(byte k = 3;k > 0;k--){
          dataArray[k][j] = pgm_read_byte(&(gameOfLifeColours[dataArray[0][j]-1][3-k]));
        }
      }
      if((dataArray[0][j+1] > 0) && (dataArray[0][j+1] < 17)){
        for(byte k = 3;k > 0;k--){
          dataArray[k][j+1] = pgm_read_byte(&(gameOfLifeColours[dataArray[0][j+1]-1][3-k]));
        }
      }
      graphic.twoPixels(dataArray[1][j],dataArray[2][j],dataArray[3][j],dataArray[1][j+1],dataArray[2][j+1],dataArray[3][j+1]);
    }
  }
  storeIndex = 1 - storeIndex;
  return 1;
}
//---------------------------------------------------------------------------------------------------------
