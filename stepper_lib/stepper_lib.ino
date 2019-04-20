#include <Stepper.h>
#include <Servo.h>

/**
   STEPPER MOTOR CONFIGURATION

   - The correct order that work for 28BJY-48 with the library is 1-3-2-4
   - I have found that stepsPerRevolution variable fits better with
     64 Steps (5.625º/step) * 64 Gear_ratio / 2 = 2048
*/
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11
#define STEPS_PER_REVOLUTION 2048
#define POWER_SAVE_TIME 2000
bool MOTOR_ON = false;
unsigned long MOTOR_TIME = 0;
Stepper myStepper(STEPS_PER_REVOLUTION, IN1, IN3, IN2, IN4);

/**
   Servo Setup
*/
#define SERVO_PIN 5
int servoPosition = 90;
bool servoInitialized = false;
Servo servoMotor;

/**
   JOYSTICK SETUP
*/
#define JS_BUTTON_PIN 4
#define JS_X_PIN A0
#define JS_Y_PIN A1
#define JS_MIN_CONSTRAIN -500
#define JS_MAX_CONSTRAIN 500
int JS_X_CENTER; // This is the read on center of the Joystick
int JS_Y_CENTER; // This is the read on center of the Joystick

void moveStepper(int steps)
{
  MOTOR_ON = true;
  myStepper.step(steps);
  MOTOR_TIME = millis();
}

/**
   This function will power off all coils to save power after 2 seconds
   and keep motor cool.
*/
void powerSaveMotor() {
  if (MOTOR_ON) {
    if ( (MOTOR_TIME + POWER_SAVE_TIME) < millis() )
    {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      MOTOR_ON = false;
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(JS_BUTTON_PIN, INPUT_PULLUP);
  pinMode(JS_X_PIN, INPUT);
  pinMode(JS_Y_PIN, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);

  JS_X_CENTER = analogRead(JS_X_PIN);
  JS_Y_CENTER = analogRead(JS_Y_PIN);

  myStepper.setSpeed(10);
  servoMotor.attach(SERVO_PIN);
}

void loop() {
  int X_AXIS = analogRead(JS_X_PIN) - JS_X_CENTER;
  int Y_AXIS = analogRead(JS_Y_PIN) - JS_Y_CENTER;

  //if (X_AXIS != 0 || Y_AXIS != 0 ) {
  //  Serial.println("X: " + String(X_AXIS) + " Y: " + String(Y_AXIS) + " BTN: " + digitalRead(JS_BUTTON_PIN));
  //}

  X_AXIS = map(constrain(X_AXIS, JS_MIN_CONSTRAIN, JS_MAX_CONSTRAIN), JS_MIN_CONSTRAIN , JS_MAX_CONSTRAIN, -10, 10);
  //Serial.println("XMAP : " + String(X_AXIS));
  if (X_AXIS != 0 ) {
    moveStepper(X_AXIS);
  }

  if (servoPosition != Y_AXIS && abs(Y_AXIS - servoPosition) > 1) {
    servoMotor.write(map(Y_AXIS, JS_Y_CENTER * -1 , JS_Y_CENTER, 0, 180));
    servoPosition = Y_AXIS;
  }

  powerSaveMotor();
  delay(10);
}
