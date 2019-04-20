#include <Stepper.h>
/**
   STEPPER MOTOR CONFIGURATION

   - The correct order that work for 28BJY-48 with the library is 1-3-2-4
   - I have found that stepsPerRevolution variable fits better with
     64 Steps (5.625º/step) * 64 Gear_ratio / 2 = 2048
*/
// MOTOR A
#define IN1_A 2
#define IN2_A 3
#define IN3_A 4
#define IN4_A 5

// MOTOR B
#define IN1_B 6
#define IN2_B 7
#define IN3_B 8
#define IN4_B 9

#define STEPS_PER_REVOLUTION 2048
#define POWER_SAVE_TIME 2000
bool MOTOR_ON = false;
unsigned long MOTOR_TIME = 0;
Stepper STEPPER_X(STEPS_PER_REVOLUTION, IN1_A, IN3_A, IN2_A, IN4_A);
Stepper STEPPER_Y(STEPS_PER_REVOLUTION, IN1_B, IN3_B, IN2_B, IN4_B);

/**
   JOYSTICK SETUP
*/
#define JS_X_PIN A0
#define JS_Y_PIN A1
#define JS_MIN_CONSTRAIN -500
#define JS_MAX_CONSTRAIN 500
int JS_X_CENTER; // This is the read on center of the Joystick
int JS_Y_CENTER; // This is the read on center of the Joystick

void moveStepperX(int steps)
{
  MOTOR_ON = true;
  STEPPER_X.step(steps);
  MOTOR_TIME = millis();
}

void moveStepperY(int steps)
{
  MOTOR_ON = true;
  STEPPER_Y.step(steps);
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
      digitalWrite(IN1_A, LOW);
      digitalWrite(IN2_A, LOW);
      digitalWrite(IN3_A, LOW);
      digitalWrite(IN4_A, LOW);
      digitalWrite(IN1_B, LOW);
      digitalWrite(IN2_B, LOW);
      digitalWrite(IN3_B, LOW);
      digitalWrite(IN4_B, LOW);
      MOTOR_ON = false;
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(JS_X_PIN, INPUT);
  pinMode(JS_Y_PIN, INPUT);
  pinMode(IN1_A, OUTPUT);
  pinMode(IN2_A, OUTPUT);
  pinMode(IN3_A, OUTPUT);
  pinMode(IN4_A, OUTPUT);

  pinMode(IN1_B, OUTPUT);
  pinMode(IN2_B, OUTPUT);
  pinMode(IN3_B, OUTPUT);
  pinMode(IN4_B, OUTPUT);

  JS_X_CENTER = analogRead(JS_X_PIN);
  JS_Y_CENTER = analogRead(JS_Y_PIN);

}

void loop() {
  int X_AXIS = analogRead(JS_X_PIN) - JS_X_CENTER;
  int Y_AXIS = analogRead(JS_Y_PIN) - JS_Y_CENTER;

  X_AXIS = map(constrain(X_AXIS, JS_MIN_CONSTRAIN, JS_MAX_CONSTRAIN), JS_MIN_CONSTRAIN , JS_MAX_CONSTRAIN, -18, 18);
  if (X_AXIS != 0 ) {
    STEPPER_X.setSpeed(abs(X_AXIS));
    //Serial.println("XMAP : " + String(X_AXIS));
    moveStepperX(map(X_AXIS, -10, 10, -1, 1));
  }

  Y_AXIS = map(constrain(Y_AXIS, JS_MIN_CONSTRAIN, JS_MAX_CONSTRAIN), JS_MIN_CONSTRAIN , JS_MAX_CONSTRAIN, -18, 18);
  if (Y_AXIS != 0 ) {
    STEPPER_Y.setSpeed(abs(Y_AXIS));
    //Serial.println("YMAP : " + String(Y_AXIS));
    moveStepperY(map(Y_AXIS, -10, 10, -1, 1));
  }

  powerSaveMotor();
}
