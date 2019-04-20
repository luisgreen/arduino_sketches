#include <Wire.h>
#include <LiquidCrystal.h>

/**
   RECURSOS
   - https://morf.lv/mems-part-1-guide-to-using-accelerometer-adxl345
   - https://www.luisllamas.es/arduino-acelerometro-adxl345/
   - https://learn.adafruit.com/adxl345-digital-accelerometer/programming
*/

/**
  ADXL345 ACCELEROMETER IIC
  Usualmente, la dirección IIC de este dispositivo es 0x53
*/
#define DEVICE_ADDRESS  0x53

/**
   Lista de Registros
*/
#define DEVID         0x00
#define THRESH_TAP    0x1D
#define OFSX          0x1E
#define OFSY          0x1F
#define OFSZ          0x20
#define TAP_DUR       0x21
#define TAP_LATENT    0x22
#define TAP_WINDOW    0x23
#define THRESH_ACT    0x24
#define THRESH_INACT  0x25
#define TIME_INACT    0x26
#define ACT_INACT_CTL 0x27
#define THRESH_FF     0x28
#define TIME_FF       0x29
#define TAP_AXES      0x2A
#define ACT_TAP_STAT  0x2B
#define BW_RATE       0x2C
#define POWER_CTL     0x2D
#define INT_ENABLE    0x2E
#define INT_MAP       0x2F
#define INT_SOURCE    0x30
#define DATA_FORMAT   0x31
#define DATAX0        0x32 //X-Axis Data 0
#define DATAX1        0x33 //X-Axis Data 1
#define DATAY0        0x34 //Y-Axis Data 0
#define DATAY1        0x35 //Y-Axis Data 1
#define DATAZ0        0x36 //Z-Axis Data 0
#define DATAZ1        0x37 //Z-Axis Data 1
#define FIFO_CTL      0x38
#define FIFO_STATUS   0x39

/**
   Registro POWER_CTL
  Si quieres 8 Hz no setees el bit 0
  POWER_CTL_WAKEUP_8HZ
*/
#define POWER_CTL_WAKEUP_4HZ  0x00
#define POWER_CTL_WAKEUP_2HZ  0x01
#define POWER_CTL_WAKEUP_1HZ  0x02
#define POWER_CTL_SLEEP       0x04
#define POWER_CTL_MEASURE     0x08
#define POWER_CTL_AUTO_SLEEP  0x16
#define POWER_CTL_LINK        0x32


/**
   Registro DATA_FORMAT
   DATA_FORMAT_RNG_2G
*/
#define DATA_FORMAT_RNG_4G        0x00
#define DATA_FORMAT_RNG_8G        0x01
#define DATA_FORMAT_RNG_16G       0x02
#define DATA_FORMAT_JUSTIFY_LEFT  0x04
#define DATA_FORMAT_FULL_RES      0x08 // 1 = 4mg/LSB, 0 = 10 bit
#define DATA_FORMAT_INT_INVERT    0x32
#define DATA_FORMAT_SPI           0x64
#define DATA_FORMAT_SELF_TEST     0x128

/**
   Buffer de lectura de valores
*/
byte _buff[6];

/**
   Inicializacion del LCD en mi versión
*/
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void cl(int line)
{
  lcd.setCursor(0, line);
  lcd.print("                ");
  lcd.setCursor(0, line);
}

/**
   Funcion auxiliar de escritura una dirección IIC
*/
void writeTo(int device, byte address, byte val) {
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.write(val);
  Wire.endTransmission();
}

void readAccel() {
  // Leer los datos, 6 bytes desde el registro DATAX0
  uint8_t numBytesToRead = 6;
  readFrom(DEVICE_ADDRESS, char(DATAX0), numBytesToRead, _buff);

  /**
     Leer los valores del registro y convertir a int (Cada eje tiene 10 bits, en 2 Bytes LSB)
     Por tanto seteo los MSB, los ruedo a la izquierda 8 bits, y le pego los LSB y obtengo
     el entero de cada eje.
  */
  int x = (((int)_buff[1]) << 8) | _buff[0];
  int y = (((int)_buff[3]) << 8) | _buff[2];
  int z = (((int)_buff[5]) << 8) | _buff[4];
  Serial.print("x: ");
  Serial.print( x );
  Serial.print(" y: ");
  Serial.print( y );
  Serial.print(" z: ");
  Serial.println( z );
  cl(0);
  lcd.print("X: " + String(x) + " Y: " + String(y));
  cl(1);
  lcd.print("Z: " + String(z));
}

/**
   Esta función lee la cantidad de bytes especificados desde un registro
*/
void readFrom(int device, byte address, int num, byte _buff[]) {
  int i = 0;

  // Le enviamos desde que dirección queremos leer
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.endTransmission();

  /**
     Empezamos a recibir los paquetes y almacenamos en el buffer,
     recuerda que cada eje ocupa 2 bytes (que contienen 10 bits)
     _buff[0] = LSB_DATAX0
     _buff[1] = MSB_DATAX1
     _buff[2] = LSB_DATAY0
     _buff[3] = MSB_DATAY1
     _buff[4] = LSB_DATAZ0
     _buff[5] = MSB_DATAZ1
  */
  Wire.beginTransmission(device);
  Wire.requestFrom(device, num);
  while (Wire.available()) {
    _buff[i] = Wire.read();
    i++;
  }
  Wire.endTransmission();
}

void initializeDevices() {
  Serial.begin(9600);
  Wire.begin();
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("              ");
  lcd.blink();
}

void setup() {
  initializeDevices();

  writeTo(DEVICE_ADDRESS, DATA_FORMAT,  DATA_FORMAT_RNG_4G);
  writeTo(DEVICE_ADDRESS, POWER_CTL,    POWER_CTL_WAKEUP_4HZ | POWER_CTL_MEASURE );
}


void loop() {
  readAccel();
}
