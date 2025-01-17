/**
 * @file ultrasonic_sensor.ino
 * @author Jarl Pallesen
 * @brief A script for controlling an HC-SR04 ultrasonic sensor and SG90 servo motor.
 *        The servo sweeps like a radar, and the script measures distance to objects.
 *        If an object is within a specified range, the onboard LED lights up.
 * @version 0.1
 * @date 2025-01-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <Servo.h>

/**
 * @brief Pin definitions for the ultrasonic sensor.
 */
#define trigPin 2 ///< Trigger pin for the ultrasonic sensor.
#define echoPin 3 ///< Echo pin for the ultrasonic sensor.

Servo myservo; ///< Servo motor object.

long duration; ///< Duration of echo signal in microseconds.
float distance; ///< Calculated distance in cm.
const int minDistance = 10; ///< Minimum distance (in cm) to trigger LED.

/**
 * @brief Setup function initializes pins and serial communication.
 */
void setup() {
  pinMode(trigPin, OUTPUT); ///< Set trigger pin as output.
  pinMode(echoPin, INPUT); ///< Set echo pin as input.
  pinMode(LED_BUILTIN, OUTPUT); ///< Set onboard LED pin as output.
  myservo.attach(4); ///< Attach servo to pin 4 (orange wire to digital output, brown to ground, red to power).
  Serial.begin(115200); ///< Initialize serial communication at 115200 baud.
}

/**
 * @brief Main loop sweeps the servo and measures distance.
 */
void loop() {
  // Sweep servo from 0 to 180 degrees.
  for (int angle = 0; angle <= 180; angle += 5) {
    myservo.write(angle); ///< Move servo to the current angle.
    delay(100); ///< Wait for servo to reach position.
    measureDistance(); ///< Measure and process distance.
  }

  // Sweep servo back from 180 to 0 degrees.
  for (int angle = 180; angle >= 0; angle -= 5) {
    myservo.write(angle); ///< Move servo to the current angle.
    delay(100); ///< Wait for servo to reach position.
    measureDistance(); ///< Measure and process distance.
  }
}

/**
 * @brief Measures distance using the ultrasonic sensor and controls the onboard LED.
 */
void measureDistance() {
  // Send a 10us pulse to trigger pin.
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the duration of the echo.
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in cm.
  distance = duration * 0.034 / 2;

  // Print distance to Serial Monitor.
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Light up LED if object is nearby.
  if (distance > 0 && distance <= minDistance) {
    digitalWrite(LED_BUILTIN, HIGH); ///< Turn on LED if object is close.
  } else {
    digitalWrite(LED_BUILTIN, LOW); ///< Turn off LED if no object is nearby.
  }
}
