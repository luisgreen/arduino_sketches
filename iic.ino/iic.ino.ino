#include <Wire.h>
#include <LiquidCrystal.h>

#define EVENT_POT 1
#define EVENT_ALARM 2

#define PIN_BACKLIGHT 7

#define ARDUINO_POT_ALM 7
#define ARDUINO_POT 8

// const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
// Replaced 3 by 6 for using pin 3 as interrupt
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 6, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

bool inMenu = false;
int thresholds[4] = {977, 1023, 844, 812};
char keys[4] = {'U', 'R', 'B', 'L'};

/**
  0 = Upper Arduino
  1 = Lower Arduino
*/
int reading = 0;
int arduinoAddress[2] = {ARDUINO_POT_ALM, ARDUINO_POT};

/**
  Arduino Readups
*/
String readUps[2] = { "", "" };


byte batteryIcon[9][8] = {
  { // AC
    B01110,
    B11111,
    B10101,
    B10001,
    B11011,
    B11011,
    B11111,
    B11111,
  },
  { // BATT FULL
    B01110,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
  },
  { // 80%
    B01110,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
  },
  { // 60%
    B01110,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
  },
  { // 45%
    B01110,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111,
  },
  { // 30%
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111,
  },
  { // 15%
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111,
  },
  { // 0%
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
  },
  { // UNK
    B01110,
    B11011,
    B10101,
    B11101,
    B11011,
    B11111,
    B11011,
    B11111,
  }
};

float readVcc() {
  long result; // Read 1.1V reference against AVcc

  //http://maxembedded.com/2011/06/the-adc-of-the-avr/
  ADMUX = bit(REFS0) | bit(MUX3) | bit(MUX2) | bit(MUX1);
  delay(2); // Wait for Vref to settle

  // https://www.arduino.cc/en/Reference/PortManipulation
  // http://www.robotplatform.com/knowledge/ADC/adc_tutorial_2.html
  ADCSRA |= bit(ADSC);
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL; result |= ADCH << 8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result / 1000.0;
}


void readBatteryLevel()
{
  //read the voltage and convert it to volt
  double curvolt = double( readVcc() );
  // check if voltge is bigger than 4.2 volt so this is a power source
  if (curvolt > 4.2) {
    lcdBatteryIcon(0);
  }
  if (curvolt <= 4.2 && curvolt > 4.0) {
    lcdBatteryIcon(1);
  }
  if (curvolt <= 4.0 && curvolt > 3.8) {
    lcdBatteryIcon(2);
  }
  if (curvolt <= 3.8 && curvolt > 3.6) {
    lcdBatteryIcon(3);
  }
  if (curvolt <= 3.6 && curvolt > 3.4) {
    lcdBatteryIcon(4);
  }
  if (curvolt <= 3.4 && curvolt > 3.2) {
    lcdBatteryIcon(5);
  }
  if (curvolt <= 3.2 && curvolt > 3.0) {
    lcdBatteryIcon(6);
  }
  if (curvolt < 3.0) {
    lcdBatteryIcon(7);
  }
}

/**
  We will write always in 15,0 position
  for simplify things
*/
void lcdBatteryIcon(int icon)
{
  int xpos = 15;
  int ypos = 0;
  lcd.createChar(0 , batteryIcon[icon]);
  lcd.setCursor(xpos, ypos);
  lcd.write(byte(0));
}


void keypadInterrupt()
{
  int keyValue = analogRead(A3);
  Serial.println(keyValue);
  for (int i = 0; i < 4; i++) {
    Serial.println(thresholds[i]);
    if (abs(keyValue - thresholds[i]) < 5) {
      Serial.println(String(keys[i]) + " KEY");
      switch (keys[i])
      {
        case 'U':
        case 'B':
          reading = reading == 1 ? 0 : 1;
          break;
      }
      break;
    }
  }
}


String readFrom(int addr)
{
  //Serial.println("READING:" + String(addr));
  char str[1024] = "";

  Wire.requestFrom(addr, 20);

  while (Wire.available())
  {
    char c[2];
    c[0] = Wire.read();    // receive a byte as character
    if (char(c[0]) == '\n') {
      break;
    }
    strcat(str,  c );
  }

  return str;
}

void cl(int line)
{
  lcd.setCursor(0, line);
  lcd.print("                ");
  lcd.setCursor(0, line);
}

/*
void showInfoFromArduino(int arduino)
{
}
*/

void setup()
{
  Serial.begin(9600);
  pinMode(PIN_BACKLIGHT, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(A3, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), keypadInterrupt, RISING );

  Wire.begin();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Booting up....");
  lcdBatteryIcon(8);
  delay(1000);
  cl(0);
  lcd.setCursor(0, 0);
  lcd.print("I2C Values:");
  digitalWrite(PIN_BACKLIGHT, HIGH);
  //lcd.autoscroll();
  lcd.blink();
}

#define INTERVAL_READ_BATTERY 2000
#define INTERVAL_READ_ARDUINOS 250
#define INTERVAL_SHOW_READ 1000
unsigned long TIME_READ = 0;
unsigned long TIME_BATTERY = 0;
unsigned long TIME_SHOW_READ = 0;
unsigned long CURRENT_MILLIS = 0;

void readArduinos() {
  int howManyArduinos = (int) sizeof(arduinoAddress) / sizeof(arduinoAddress[0]);
  for (int i = 0; i < howManyArduinos; i++) {
    readUps[i] = readFrom(arduinoAddress[i]);
  }
}

void showArduinoRead() {
  cl(1);
  lcd.print(readUps[reading]);
}

void loop()
{
  CURRENT_MILLIS = millis();

  if (CURRENT_MILLIS > TIME_READ + INTERVAL_READ_ARDUINOS) {
    //Serial.println("LEO ARDUINOS");
    readArduinos();
    TIME_READ = CURRENT_MILLIS;
  }

  if (CURRENT_MILLIS > TIME_BATTERY + INTERVAL_READ_BATTERY) {
    //Serial.println("LEO BATERIA");
    readBatteryLevel();
    TIME_BATTERY = CURRENT_MILLIS;
  }

  if (CURRENT_MILLIS > TIME_SHOW_READ + INTERVAL_SHOW_READ) {
    //Serial.println("MUESTRO");
    showArduinoRead();
    TIME_SHOW_READ = CURRENT_MILLIS;
  }

  delay(100);

}
