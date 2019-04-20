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

float VCC_IN=0.0;

#define V_DIVIDER 1023

// 1 Forward, 0 Backwards
const int motorRotation = FORWARD;

float readVcc() {
  long result; // Read 1.1V reference against AVcc

  //http://maxembedded.com/2011/06/the-adc-of-the-avr/
  ADMUX = bit(REFS0) | bit(MUX3) | bit(MUX2) | bit(MUX1);
  delay(2); // Wait for Vref to settle

  // https://www.arduino.cc/en/Reference/PortManipulation
  // http://www.robotplatform.com/knowledge/ADC/adc_tutorial_2.html
  ADCSRA |= bit(ADSC);
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL; 
  result |= ADCH << 8;
  Serial.print(result); Serial.println(" 1.1 VREF"); 
  // 1.1vref * 1023steps * 1000mv = 1125300L
    result = (1.1 * V_DIVIDER * 1000) / result; // Back-calculate AVcc in mV
  return result / 1000.0;
}


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
  VCC_IN=readVcc();
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
   Serial.println("");
   VCC_IN=readVcc();
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
  float voltage = reading * VCC_IN;
  voltage /= V_DIVIDER; 
  // print out the voltage
  Serial.print(reading); Serial.println(" READING");
  Serial.print(VCC_IN); Serial.println(" VCC");
  Serial.print(voltage); Serial.println(" volts");

  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  Serial.print(temperatureC); Serial.println(" degrees C");
}
