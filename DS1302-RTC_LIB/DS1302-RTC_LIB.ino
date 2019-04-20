#include <Vcc.h>
#include <LiquidCrystal.h>
/** Se debe colocar entre comillas para referirse a los include locales */
#include "DS1302RTC.h"

unsigned long CURRENT_MILLIS = 0;

/**
   Lectura de la carga de batería, o en su defecto, Vcc
*/
#define       INTERVAL_READ_BATTERY 3000
unsigned long TIME_READ = 0;
unsigned long TIME_BATTERY = 0;
unsigned long TIME_SHOW_READ = 0;
const float   VccCorrection = 1.0 / 1.0; // Medición Vcc en multímetro / Reportado por Vcc
Vcc vcc(VccCorrection);

/**
   Parámetros del LCD
   A menos que se necesiten los puertos de interrupciones, los coloco seguidos en el arduino
   1  VSS (+5v)   2  VDD (GND)  3  V0 (CONT)  4   RS    5  RW (+5v)   6  E
   7 - 10 (N/C)   11 D4         12 D5         13  D6    15 D7
   15 (LED Vcc)   16 (LED GND)
*/
#define LCD_COLS  20
#define LCD_ROWS  4
#define LCD_BLINK true
#define RS        7
#define EN        6
#define D4        5
#define D5        4
#define D6        3
#define D7        2
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

/**
   Parametros del Realtime Clock DS1302
   https://github.com/NeiroNx/_rtcLib
*/
#define         INTERVAL_READ_RTC 500
#define         SCK_PIN 8
#define         IO_PIN  9
#define         CE_PIN  10
unsigned long   RTC_TIME = 0;
#define         CLOCK_ROW 3
DS1302_RTC      _rtc(SCK_PIN, IO_PIN, CE_PIN);


/**
  Se escribirá el ícono de batería en la esquina sup. der.
*/
void lcdBatteryIcon(const int icon = -1) {
  lcd.createChar(0 , vcc.Get_Battery_Icon(icon));
  lcd.setCursor(LCD_COLS - 1, 0);
  lcd.write(byte(0));
}

void readBatteryLevel(unsigned long* currentMillis) {
  if (*currentMillis > TIME_BATTERY + INTERVAL_READ_BATTERY) {
    float curvolt = vcc.Read_Volts();
    Serial.println("Bat: " + String(curvolt) + "v");
    lcdBatteryIcon();
    TIME_BATTERY = *currentMillis;
  }
}

void cl(unsigned int line) {
  char cleanLine[LCD_COLS];
  for (int i = 0; i < LCD_COLS; i++) {
    cleanLine[i] = ' ';
  }
  lcd.setCursor(0, line);
  lcd.print(cleanLine);
  lcd.setCursor(0, line);
}

/**
   Escribe un patron en el LCD para mostrar todos los pixeles
*/
void testLCD() {
  for (int row = 0; row < LCD_ROWS; row ++) {
    lcd.setCursor(0, row);
    for (int col = 0; col < LCD_COLS; col ++) {
      lcd.print(char(255));
      delay(10);
    }
  }
  for (int row = 0; row < LCD_ROWS; row ++) {
    lcd.setCursor(0, row);
    for (int col = 0; col < LCD_COLS; col ++) {
      lcd.print(' ');
      delay(10);
    }
  }
}

void readTime(unsigned long* currentMillis) {
  if (*currentMillis > RTC_TIME + INTERVAL_READ_RTC) {
    // This allows for the update of variables for time or accessing the individual elements.
    _rtc.updateTime();

    // Start printing elements as individuals
    Serial.print("Current Date / Time: ");
    Serial.print(_rtc.dayofmonth); //You can switch between day and month if you're using American system
    Serial.print("/");
    Serial.print(_rtc.month);
    Serial.print("/");
    Serial.print(_rtc.year);
    Serial.print(" ");
    Serial.print(_rtc.hours);
    Serial.print(":");
    Serial.print(_rtc.minutes);
    Serial.print(":");
    Serial.println(_rtc.seconds);

    char buffer[20] = "";
    sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d", _rtc.dayofmonth, _rtc.month, _rtc.year, _rtc.hours, _rtc.minutes, _rtc.seconds);
    lcd.setCursor(0, CLOCK_ROW);
    lcd.print(buffer);
    //lcd.print(String(_rtc.dayofmonth) + "/" + String(_rtc.month) + "/" + String(_rtc.year) + " " + String(_rtc.hours) + ":" + String(_rtc.minutes) + ":" + String(_rtc.seconds));

    RTC_TIME = *currentMillis;
  }
}

void initializeLcd() {
  lcd.begin(LCD_COLS, LCD_ROWS);
  testLCD();
#if defined LCD_BLINK && LCD_BLINK == true
  lcd.blink();
#endif
}

/**
   Inicializa el _rtc en la hora/fecha en la que se compiló
   el sketch
*/
void initializeRTC() {
  /**
    _rtc.begin();
    if (! _rtc.isrunning()) {
    Serial.println("_rtc no esta Corriendo!");
    for (;;);
    }
  */
  Serial.print("Inicializando en: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);
  // _rtc.setDS1302Time(00, 59, 23, 6, 10, 1, 2014);
}

void readFromSerial() {
  while (Serial.available() > 0) {
    char incomingCharacter = Serial.read();
    switch (incomingCharacter) {
      case 'H':
        _rtc.setDS1302Time(_rtc.seconds, _rtc.minutes, (_rtc.hours == 23) ? 0 : ++_rtc.hours, _rtc.dayofweek, _rtc.dayofmonth, _rtc.month, _rtc.year);
        break;

      case 'h':
        _rtc.setDS1302Time(_rtc.seconds, _rtc.minutes, (_rtc.hours == 0) ? 23 : --_rtc.hours, _rtc.dayofweek, _rtc.dayofmonth, _rtc.month, _rtc.year);
        break;

      case 'M':
        _rtc.setDS1302Time(_rtc.seconds, (_rtc.minutes == 59) ? 0 : ++_rtc.minutes, _rtc.hours, _rtc.dayofweek, _rtc.dayofmonth, _rtc.month, _rtc.year);
        break;
        
      case 'm':
        _rtc.setDS1302Time(_rtc.seconds, (_rtc.minutes == 0) ? 59 : --_rtc.minutes, _rtc.hours, _rtc.dayofweek, _rtc.dayofmonth, _rtc.month, _rtc.year);
        break;

      case 'D':
        _rtc.setDS1302Time(_rtc.seconds, _rtc.minutes, _rtc.hours, _rtc.dayofweek, (_rtc.dayofmonth == 31) ? 0 : ++_rtc.dayofmonth, _rtc.month, _rtc.year);
        break;

      case 'd':
        _rtc.setDS1302Time(_rtc.seconds, _rtc.minutes, _rtc.hours, _rtc.dayofweek, (_rtc.dayofmonth == 0) ? 31 : --_rtc.dayofmonth, _rtc.month, _rtc.year);
        break;

      case 'L':
        _rtc.setDS1302Time(_rtc.seconds, _rtc.minutes, _rtc.hours, _rtc.dayofweek, _rtc.dayofmonth , (_rtc.month == 12) ? 0 : ++_rtc.month, _rtc.year);
        break;

      case 'l':
        _rtc.setDS1302Time(_rtc.seconds, _rtc.minutes, _rtc.hours, _rtc.dayofweek, _rtc.dayofmonth , (_rtc.month == 0) ? 12 : --_rtc.month, _rtc.year);
        break;
        
      case 'Y':
        _rtc.setDS1302Time(_rtc.seconds, _rtc.minutes, _rtc.hours, _rtc.dayofweek, _rtc.dayofmonth, _rtc.month, ++_rtc.year);
        break;

      case 'y':
        _rtc.setDS1302Time(_rtc.seconds, _rtc.minutes, _rtc.hours, _rtc.dayofweek, _rtc.dayofmonth, _rtc.month, --_rtc.year);
        break;
    }
    //_rtc.setDS1302Time(_rtc.seconds, _rtc.minutes, _rtc.hours, _rtc.dayofweek, _rtc.dayofmonth, _rtc.month, _rtc.year);
  }
  delay(50);
}

void setup() {
  Serial.begin(9600);
  initializeLcd();
  initializeRTC();
}

void loop() {
  CURRENT_MILLIS = millis();
  readFromSerial();
  readBatteryLevel(&CURRENT_MILLIS);
  readTime(&CURRENT_MILLIS);

  delay(100);
}
