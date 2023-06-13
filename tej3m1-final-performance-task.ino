#include <Servo.h>
#include <RunningAverage.h>
#include <DHT.h>

#define SERVO_PIN 13
#define PHOTORESISTOR_PIN_1 A0
#define PHOTORESISTOR_PIN_2 A1
#define PHOTORESISTOR_PIN_3 A2

#define WIND_TURBINE_PIN A3

#define DHTPIN 10
#define DHTTYPE DHT11

#define MOISTURE_SENSOR_PIN 12

const int WIND_TURBINE_LED_PIN[] = {6, 5, 4, 3, 2};


// Objects
Servo servo;
DHT dht11(DHTPIN, DHTTYPE);

// Averages
RunningAverage servoAverages(10);
RunningAverage windTurbineAverages(5);

// Store state
// temperature and humidity is stored because the sensor occasionally gives NaN
double temperature;
double humidity;

void setup() {
  // Serial monitor
  Serial.begin(9600);
  
  // Set pins
  servo.attach(SERVO_PIN);
  pinMode(PHOTORESISTOR_PIN_1, OUTPUT);
  pinMode(PHOTORESISTOR_PIN_2, OUTPUT);
  pinMode(PHOTORESISTOR_PIN_3, OUTPUT);

  pinMode(WIND_TURBINE_PIN, INPUT);
  for (int i = 0; i < 5; i++) {
    pinMode(WIND_TURBINE_LED_PIN[i], OUTPUT);
  }

  dht11.begin();

  pinMode(MOISTURE_SENSOR_PIN, INPUT);

  // Default state
  servo.write(0);
}


void loop() {

  // Get photoresistor values
  int photoresistorValue1 = analogRead(PHOTORESISTOR_PIN_1);
  int photoresistorValue2 = analogRead(PHOTORESISTOR_PIN_2);
  int photoresistorValue3 = analogRead(PHOTORESISTOR_PIN_3);

  // Get wind turbine value
  int windTurbineIn = analogRead(WIND_TURBINE_PIN);
  windTurbineAverages.addValue(windTurbineIn);

  // Get temperature and humidity
  readDHT11Values();

  // Write photoresistor calculated value to servo
  servo.write(getServoMovementBasedOnPhotoresistor(photoresistorValue1, photoresistorValue2, photoresistorValue3));

  // Write wind turbine values to LEDs
  double windTurbineAverage = constrain(map(windTurbineAverages.getAverage(), 0, 100, 0, 5), 0, 5);
  windTurbineOutputToLEDs(windTurbineAverage);

  // Write DHT11 values
  // outputDHT11Values();
  Serial.println(digitalRead(MOISTURE_SENSOR_PIN));
  
}

/**
 * Move the photoresistor to the bright position, indicated by the photoresistors.
 *
 * @param val1 left photoresistor
 * @param val2 centre photoresistor
 * @param val3 right photoresistor
 * @return target servo movement
 */
int getServoMovementBasedOnPhotoresistor(int val1, int val2, int val3) {

  // Convert to 0-1 scale
  double scaledVal1 = constrain(val1 / 100.0, 0.0, 1.0);
  double scaledVal2 = constrain(val2 / 100.0, 0.0, 1.0);
  double scaledVal3 = constrain(val3 / 100.0, 0.0, 1.0);

  // Store movement
  int movement = servo.read();

  // Drop lowest value
  
  if (scaledVal1 < scaledVal2 && scaledVal1 < scaledVal3) {
    // If 2 and 3 are highest, range is 90 to 160
    movement = get45MovementRange(scaledVal2, scaledVal3, 90);

  } else if (scaledVal2 < scaledVal1 && scaledVal2 < scaledVal3) {
    // If 1 and 3 are highest, return 90
    movement = 90;

  } else if (scaledVal3 < scaledVal1 && scaledVal3 < scaledVal2) {
    // If 1 and 2 are highest, range is 20 to 90
    movement = get45MovementRange(scaledVal2, scaledVal3, 20);
  }

  movement = 180 - movement;
  servoAverages.addValue(movement);
  
  float newPosition = servoAverages.getAverage();
  if (abs(newPosition - servo.read()) > 25)
    return servoAverages.getAverage();
  else return servo.read();
  
}

/**
 * Get the 0-45 deg range of the servo movement, starting from the start number.
 *
 * @param val1 value 1
 * @param val2 value 2
 * @param start start number
 * @return range from start to start+45
 */
double get45MovementRange(double val1, double val2, double start) {
  double difference = val2 - val1;
  double range = 70;

  if (difference > 0)
    return (start + range) - (range * difference);
  else if (difference < 0) {
    return start + abs(range * difference);
  } else return start + (range / 2.0);
}

/**
 * Output the wind turbine to the LEDs.
 * @param amount wind turbine amount
 */
void windTurbineOutputToLEDs(int amount) {
  for (int i = 0; i < 5; i++) {
    digitalWrite(WIND_TURBINE_LED_PIN[i], LOW);
  }
  for (int i = 0; i < amount; i++) {
    digitalWrite(WIND_TURBINE_LED_PIN[i], HIGH);
  }
}

/**
 * Read the DHT11 values, check if is NaN, and set state.
 */
void readDHT11Values() {
  double newTemperature = dht11.readTemperature();
  double newHumidity = dht11.readHumidity();

  if (!isnan(newTemperature)) temperature = newTemperature;
  if (!isnan(newHumidity)) humidity = newHumidity;
}

/**
  * Output the DHT11 values through the serial monitor.
  */
void outputDHT11Values() {

  // Print temperature
  Serial.print("Temperature: ");
  Serial.print(temperature);
  
  // Print humidity
  Serial.print(" Humidity: ");
  Serial.print(humidity);
  
  Serial.print("\n");
}
