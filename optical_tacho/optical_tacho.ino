/*//http://www.pyroelectro.com/tutorials/tachometer_rpm_arduino/software.html*/
volatile float time = 0;
volatile float time_last = 0;
volatile int rpm_array[5] = {
  0,0,0,0,0};

void setup()
{
  //Digital Pin 2 Set As An Interrupt
  attachInterrupt(0, fan_interrupt, FALLING);
  Serial.begin(115200);

}

//Main Loop To Calculate RPM and Update LCD Display
void loop()
{
  int rpm = 0;

  while(1){    

    //Slow Down The LCD Display Updates
    delay(400);
    Serial.println(rpm);
    //Update The RPM
    if(time > 0)
    {
      //5 Sample Moving Average To Smooth Out The Data
      rpm_array[0] = rpm_array[1];
      rpm_array[1] = rpm_array[2];
      rpm_array[2] = rpm_array[3];
      rpm_array[3] = rpm_array[4];
      rpm_array[4] = 60*(1000000/(time*1));// 1 coz of one sproket   
      //Last 5 Average RPM Counts Eqauls....
      rpm = (rpm_array[0] + rpm_array[1] + rpm_array[2] + rpm_array[3] + rpm_array[4]) / 5;
    }

  }
}

//Capture The IR Break-Beam Interrupt
void fan_interrupt()
{
  time = (micros() - time_last); 
  time_last = micros();
}

/*
//code by Crenn from http://thebestcasescenario.com
 //project by Charles Gantt from http://themakersworkbench.com
 
/*To disable interrupts:
 cli();                // disable global interrupts
 
 and to enable them: 
 sei();                // enable interrupts
 */

//Varibles used for calculations
int NbTopsFan;
int Calc;

//The pin location of the sensor
int hallsensor = 2;


typedef struct{                  //Defines the structure for multiple fans and their dividers
  char fantype;
  unsigned int fandiv;
}
fanspec;

//Definitions of the fans
fanspec fanspace[3]={
  {
    0,1  }
  ,{
    1,2  }
  ,{
    2,8  }
};

char fan = 1;   //This is the varible used to select the fan and it's divider, set 1 for unipole hall effect sensor
//and 2 for bipole hall effect sensor

void rpm ()      //This is the function that the interupt calls
{
  NbTopsFan++;
}

//This is the setup function where the serial port is initialised,
//and the interrupt is attached
void setup()
{
  pinMode(hallsensor, INPUT);
  Serial.begin(9600);
  attachInterrupt(0, rpm, RISING);
}
void loop ()
{
  NbTopsFan = 0; //Set NbTops to 0 ready for calculations
  sei(); //Enables interrupts
  delay (1000); //Wait 1 second
  cli(); //Disable interrupts
  Calc = ((NbTopsFan * 60)/fanspace[fan].fandiv); //Times NbTopsFan (which is apprioxiamately the fequency the fan is spinning at) by 60 seconds before dividing by the fan's divider
  Serial.print (Calc, DEC); //Prints the number calculated above
  Serial.print (" rpm\r\n"); //Prints " rpm" and a new line
}
*/

