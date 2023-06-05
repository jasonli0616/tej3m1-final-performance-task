#include <Servo.h>

#define SERVO_PIN 10
#define PHOTORESISTOR_PIN_1 A0
#define PHOTORESISTOR_PIN_2 A1
#define PHOTORESISTOR_PIN_3 A2


// Objects
Servo servo;

// State
int photoresistorValue1;
int photoresistorValue2;
int photoresistorValue3;
int servoPosition = 90;


void setup() {
  // Serial monitor
  Serial.begin(9600);
  
  // Set pins
  servo.attach(10);
  pinMode(PHOTORESISTOR_PIN_1, OUTPUT);
  pinMode(PHOTORESISTOR_PIN_2, OUTPUT);
  pinMode(PHOTORESISTOR_PIN_3, OUTPUT);

  // Default state
  servo.write(servoPosition);
}


void loop() {

  // Get photoresistor values
  photoresistorValue1 = analogRead(PHOTORESISTOR_PIN_1);
  photoresistorValue2 = analogRead(PHOTORESISTOR_PIN_2);
  photoresistorValue3 = analogRead(PHOTORESISTOR_PIN_3);
  
}

/* Move the photoresistor to the bright position, indicated by the photoresistors. */
void moveServoBasedOnPhotoresistor() {

  // Temporary solution. Change method later.
  if (photoresistorValue1 > photoresistorValue2 && photoresistorValue1 > photoresistorValue3)
    servo.write(0);
  else if (photoresistorValue2 > photoresistorValue1 && photoresistorValue2 > photoresistorValue3)
    servo.write(90);
  else if (photoresistorValue3 > photoresistorValue2 && photoresistorValue3 > photoresistorValue1)
    servo.write(180);
}
