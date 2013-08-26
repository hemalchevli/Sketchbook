extern volatile unsigned long timer0_overflow_count;
float fanalog0;
int analog0;
unsigned long tiempo;
byte serialByte;

void setup() {

   Serial.begin(9600);
   Serial.println("Empezamos...");
}

void loop()
{

  while (Serial.available()>0){  
    serialByte=Serial.read();
    if (serialByte=='C'){	  
	while(1){
	  analog0=analogRead(0);
	  // Convierto a milivoltios
	  fanalog0=(analog0)*(5000.0/1023);
	  // Recojo ticks del timer0 => 1 tick cada 4 us (usado en millis )
	  tiempo=(timer0_overflow_count << 8) + TCNT0;
	  // Convierto a us
	  tiempo=tiempo*4;
	  //Lo envio para simular archivo tipo *.csv
	  Serial.print(tiempo);
	  Serial.print(';',BYTE);
	  Serial.println(fanalog0,DEC);
	  if (Serial.available()>0){
	    serialByte=Serial.read();
	    if (serialByte=='F')  break;
	  }
	}
    }
  }
}

 
