void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  for(int i = 1; i<=10;i++){
    Serial.println(i);
    delay(100);
  }
  //[2] [RX_RSSI:-18] - ACK sent.
  // put your main code here, to run repeatedly: 

}

