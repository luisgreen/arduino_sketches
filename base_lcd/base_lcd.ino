#include <Vcc.h>
#include <LiquidCrystal.h>

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

void initializeLcd() {
  lcd.begin(LCD_COLS, LCD_ROWS);
  testLCD();
#if defined LCD_BLINK && LCD_BLINK == true
  lcd.blink();
#endif
}

void setup() {
  Serial.begin(9600);
  initializeLcd();
}

void loop() {
  CURRENT_MILLIS = millis();
  readBatteryLevel(&CURRENT_MILLIS);

  delay(100);
}
