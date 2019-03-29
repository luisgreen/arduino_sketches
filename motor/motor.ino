#define directionInput 7

#define enableMotorA 12
#define enableMotorB 2

#define FORWARD 1
#define BACKWARDS 0

#define motorAForward 3
#define motorAReverse 9

#define motorBForward 5
#define motorBReverse 10

#define potentiometerInput A0
#define tmpInput A1

// 1 Forward, 0 Backwards
const int motorRotation = FORWARD;

void setup()
{ 
  Serial.begin(9600);

  pinMode(directionInput, INPUT_PULLUP);
  pinMode(tmpInput, INPUT);

  pinMode(enableMotorA,OUTPUT);
  pinMode(enableMotorB,OUTPUT);

  digitalWrite(enableMotorA,HIGH);
  digitalWrite(enableMotorB,HIGH);

  pinMode(motorAForward,OUTPUT);
  pinMode(motorAReverse,OUTPUT);
  pinMode(motorBForward,OUTPUT);
  pinMode(motorBReverse,OUTPUT);

  analogWrite(motorAReverse, LOW);
  analogWrite(motorAReverse, LOW);
  analogWrite(motorBForward, LOW);
  analogWrite(motorBForward, LOW);

}

void moveForward(int valueA, int valueB)
{
  analogWrite(motorAReverse, HIGH);
  analogWrite(motorBReverse, HIGH);
  analogWrite(motorAForward, valueA);
  analogWrite(motorBForward, valueB);
}

void moveBackwards(int valueA, int valueB)
{
  analogWrite(motorAForward, HIGH);
  analogWrite(motorBForward, HIGH);
  analogWrite(motorAReverse, valueA);
  analogWrite(motorBReverse, valueB);
}

void setDirection(int direction, int valueA, int valueB)
{
  if (direction == FORWARD){
    moveForward(valueA, valueB);
  }else{
    moveBackwards(valueA, valueB);
  }
}

void loop()
{
  int pot = map(analogRead(potentiometerInput), 0, 1024, 1, 255);
  int dir = digitalRead(directionInput);

  int left = 255 - (pot >= 130 ? (pot - 130) * 2 : 0);
  int right = 255 - (pot < 127 ? (127 - pot) * 2 : 0);


  Serial.println(dir);
  Serial.println(pot);
  Serial.println("Left: "+ String(left) +" Right: "+ String(right) );
  setDirection(dir, left, right);
  delay(250);

  int reading = analogRead(tmpInput); 
  // converting that reading to voltage, for 5v arduino
  float voltage = reading * 5.0;
  voltage /= 1024.0; 
  // print out the voltage
  Serial.print(voltage); Serial.println(" volts");

  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  Serial.print(temperatureC); Serial.println(" degrees C");

}
