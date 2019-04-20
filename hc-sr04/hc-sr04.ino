#include <Vcc.h>
#include <Wire.h>
#include <LiquidCrystal.h>

/**
   DISTANCE READING
*/
#define INTERVAL_READ_DISTANCE 100
#define TRIGGER_PIN 9
#define ECHO_PIN 10
unsigned long TIME_READ_DISTANCE = 0;
unsigned long CURRENT_MILLIS = 0;

/**
   LCD initialization
*/
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void cl(int line)
{
  lcd.setCursor(0, line);
  lcd.print("                ");
  lcd.setCursor(0, line);
}

void readDistance() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  // Reads the ECHO_PIN, returns the sound wave travel time in microseconds
  // Multiply per sound speed and only take one sense
  float distance = pulseIn(ECHO_PIN, HIGH) * 0.034 / 2.0;
  cl(1);
  lcd.print(distance);
  Serial.println(distance);
}

void setup()
{
  Serial.begin(19200);
  lcd.begin(16, 2);

  pinMode(TRIGGER_PIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN, INPUT);

  lcd.setCursor(0, 0);
  lcd.print("              ");
  lcd.blink();
}


void loop()
{
  CURRENT_MILLIS = millis();

  if (CURRENT_MILLIS > TIME_READ_DISTANCE + INTERVAL_READ_DISTANCE) {
    readDistance();
    TIME_READ_DISTANCE = CURRENT_MILLIS;
  }
}
