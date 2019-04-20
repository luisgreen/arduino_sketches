#include <Vcc.h>
#include <Wire.h>
#include <LiquidCrystal.h>

#define INTERVAL_READ_BATTERY 3000
#define INTERVAL_READ_ARDUINOS 250
#define INTERVAL_SHOW_READ 1000

unsigned long TIME_READ = 0;
unsigned long TIME_BATTERY = 0;
unsigned long TIME_SHOW_READ = 0;
unsigned long CURRENT_MILLIS = 0;

/**
 * Voltage Read from VCC
 */
const float VccCorrection = 1.0/1.0;  // Measured Vcc by multimeter divided by reported Vcc
Vcc vcc(VccCorrection);

/**
 * LCD initialization
 */
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/**
* We will write always in 15,0 position
* for simplify things
*/
void lcdBatteryIcon(const int icon = -1)
{
  int xpos = 15;
  int ypos = 0;
  lcd.createChar(0 , vcc.Get_Battery_Icon(icon));
  lcd.setCursor(xpos,ypos);
  lcd.write(byte(0));
}


void readBatteryLevel()
{
  //read the voltage and convert it to volt
  float curvolt = vcc.Read_Volts();

  lcd.setCursor(0,0);
  lcd.print("Bat: " + String(curvolt) + "v");
  lcdBatteryIcon();
}



String readFrom(int addr)
{
  char str[32]="";
  int pos = 0;
  int charRead;
  Wire.requestFrom(addr,32);
  
  while(Wire.available())
  {
    charRead = Wire.read();    // receive a byte as character
    if (char(charRead) == '.'){
      break;
    }
    str[pos] = charRead;
    pos++;
  }
  Serial.println(String(str));
  if (String(str).indexOf("YES") >= 0){
    backLight();
  }
  
  return str;
}

void cl(int line)
{
  lcd.setCursor(0,line);
  lcd.print("                ");
  lcd.setCursor(0,line);
}

void setup()
{
  Serial.begin(9600);  
  pinMode(PIN_BACKLIGHT, OUTPUT);
  pinMode(PIN_13, OUTPUT);
  pinMode(A3, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), keypadInterrupt, RISING );

  Wire.begin();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  digitalWrite(PIN_BACKLIGHT, HIGH);
  lcd.print("Booting up....");
  lcdBatteryIcon(8);
  delay(1000);
  digitalWrite(PIN_BACKLIGHT, LOW);
  delay(100);
  cl(0);
  lcd.setCursor(0, 0);
  lcd.print("              ");
  backLight();
  //lcd.autoscroll();
  lcd.blink();
}

void readArduinos() {
  digitalWrite(PIN_13, HIGH);
  int howManyArduinos = (int) sizeof(arduinoAddress) / sizeof(arduinoAddress[0]);
  for (int i = 0; i < howManyArduinos; i++) {
    readUps[i] = readFrom(arduinoAddress[i]);
  }
  digitalWrite(PIN_13, LOW);
}

void showArduinoRead(){
  cl(1);
  lcd.print(readUps[reading]);
}

void backLight(){
  digitalWrite(PIN_BACKLIGHT, HIGH);
  BACKLIGHT_ON = true;
  TIME_BACKLIGHT = millis();
}

void loop()
{
  CURRENT_MILLIS = millis();
  
  if(CURRENT_MILLIS > TIME_READ + INTERVAL_READ_ARDUINOS){
    Serial.println("LEO ARDUINOS");
    readArduinos();
    TIME_READ = CURRENT_MILLIS;
  }

  if(BACKLIGHT_ON && CURRENT_MILLIS > TIME_BACKLIGHT + INTERVAL_BACKLIGHT){
    Serial.println("BACKLIGHT");
    digitalWrite(PIN_BACKLIGHT, LOW);
    TIME_BACKLIGHT = CURRENT_MILLIS;
    BACKLIGHT_ON = false;
  }

  if(CURRENT_MILLIS > TIME_BATTERY + INTERVAL_READ_BATTERY){
    Serial.println("LEO BATERIA");
    readBatteryLevel();
    TIME_BATTERY = CURRENT_MILLIS;
  }

  if(CURRENT_MILLIS > TIME_SHOW_READ + INTERVAL_SHOW_READ){
    Serial.println("MUESTRO");
    showArduinoRead();
    TIME_SHOW_READ = CURRENT_MILLIS;
  }
  
   delay(100);

}
