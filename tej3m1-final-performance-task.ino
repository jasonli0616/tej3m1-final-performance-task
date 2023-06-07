#include <Servo.h>
#include <RunningAverage.h>

#define SERVO_PIN 10
#define PHOTORESISTOR_PIN_1 A0
#define PHOTORESISTOR_PIN_2 A1
#define PHOTORESISTOR_PIN_3 A2
#define WIND_TURBINE_PIN A3

const int WIND_TURBINE_LED_PIN1[] = {6, 5, 4, 3, 2};


// Objects
Servo servo;

// State
int photoresistorValue1;
int photoresistorValue2;
int photoresistorValue3;
int servoPosition = 90;

RunningAverage windTurbineAverages(5);

void setup() {
  // Serial monitor
  Serial.begin(9600);
  
  // Set pins
  servo.attach(10);
  pinMode(PHOTORESISTOR_PIN_1, OUTPUT);
  pinMode(PHOTORESISTOR_PIN_2, OUTPUT);
  pinMode(PHOTORESISTOR_PIN_3, OUTPUT);
  pinMode(WIND_TURBINE_PIN, INPUT);
  for (int i = 0; i < 5; i++) {
    pinMode(WIND_TURBINE_LED_PIN1[i], OUTPUT);
  }

  // Default state
  servo.write(servoPosition);
}


void loop() {

  // Get photoresistor values
  photoresistorValue1 = analogRead(PHOTORESISTOR_PIN_1);
  photoresistorValue2 = analogRead(PHOTORESISTOR_PIN_2);
  photoresistorValue3 = analogRead(PHOTORESISTOR_PIN_3);

  // Get wind turbine value
  double windTurbineIn = analogRead(WIND_TURBINE_PIN);
  windTurbineAverages.addValue(windTurbineIn);
  double average = windTurbineAverages.getAverage();
  average = constrain(map(average, 0, 20, 0, 5), 0, 5);
  Serial.println(average);
  windTurbineOutputToLEDs(average);

  // Write photoresistor calculated value to servo
  servo.write(180 - getServoMovementBasedOnPhotoresistor());
  
}

/* Move the photoresistor to the bright position, indicated by the photoresistors. */
int getServoMovementBasedOnPhotoresistor() {

  // Convert to 0-1 scale
  double scaledVal1 = constrain(photoresistorValue1 / 100.0, 0.0, 1.0);
  double scaledVal2 = constrain(photoresistorValue2 / 100.0, 0.0, 1.0);
  double scaledVal3 = constrain(photoresistorValue3 / 100.0, 0.0, 1.0);

  // Drop lowest value
  
  if (scaledVal1 < scaledVal2 && scaledVal1 < scaledVal3) {
    // If 2 and 3 are highest, range is 90 to 160
    return get45MovementRange(scaledVal2, scaledVal3, 90);

  } else if (scaledVal2 < scaledVal1 && scaledVal2 < scaledVal3) {
    // If 1 and 3 are highest, return 90
    return 90;

  } else if (scaledVal3 < scaledVal1 && scaledVal3 < scaledVal2) {
    // If 1 and 2 are highest, range is 20 to 90
    return get45MovementRange(scaledVal2, scaledVal3, 20);
  }
  
}

double get45MovementRange(double val1, double val2, double start) {
  double difference = val2 - val1;
  double range = 70;

  if (difference > 0)
    return (start + range) - (range * difference);
  else if (difference < 0) {
    return start + abs(range * difference);
  } else return start + (range / 2.0);
}

void windTurbineOutputToLEDs(int amount) {
  for (int i = 0; i < 5; i++) {
    digitalWrite(WIND_TURBINE_LED_PIN1[i], LOW);
  }
  for (int i = 0; i < amount; i++) {
    digitalWrite(WIND_TURBINE_LED_PIN1[i], HIGH);
  }
}
