const int m1=9;
const int m2=10;
void setup(){
  Serial.begin(9600);
  pinMode(m1,OUTPUT);
  pinMode(m2,OUTPUT);
    
}
void loop(){
  digitalWrite(m1,LOW);
  digitalWrite(m2,HIGH);
    delay(200);
  digitalWrite(m1,HIGH);
  digitalWrite(m2,LOW);
  
   delay(200);
  
}
