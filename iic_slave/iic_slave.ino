#include <Wire.h>

#define SENSOR_COUNT 2

#define EVENT_POT 1
#define EVENT_ALARM 2
#define ALARM_COUNT 5

int alarmTick = 0;
byte SlaveReceived = 0;
byte myArray[2];
bool alarmTrigger = false;
float inputVoltage;

void receiveEvent (int howMany)                    //This Function is called when Slave receives value from master
{
  Serial.println("receiveEvent: " + howMany);
  SlaveReceived = Wire.read();                    //Used to read value received from master and store in variable SlaveReceived
}

void requestEvent()                                //This Function is called when Master wants value from slave
{ 
  digitalWrite(13,HIGH);
  int16_t potVal = analogRead(A0);
  int potMap = map(potVal, 0,1000,0,100);
  
  int EVENT = alarmTrigger ? EVENT_ALARM : EVENT_POT;
  String alarm = alarmTrigger ? "YES":"NO"; 

  if (EVENT == EVENT_ALARM && alarmTick < ALARM_COUNT){
    alarmTick++;
  }else{
    alarmTrigger = false;
  }
  
  String out = "POT:"+String(potMap)+";ALM:"+alarm+".";
  Serial.println(out.c_str());
  Serial.println(strlen(out.c_str()));
  Wire.write(out.c_str(), strlen(out.c_str()));                          // sends one byte converted POT value to master
  digitalWrite(13,LOW);
}

void alarm(){
  alarmTick = 0;
  alarmTrigger = true; 
  Serial.println("ALARM!!");
}

void setup()
{
  Serial.begin(9600);     
  pinMode(13, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(7, INPUT);
  pinMode(A1, INPUT);
  Wire.begin(7);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  attachInterrupt(digitalPinToInterrupt(2), alarm, RISING );
}

void loop()
{ 
  delay(500);
}
