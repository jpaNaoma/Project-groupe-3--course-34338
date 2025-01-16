#include <Servo.h>

#define trigPin 2
#define echoPin 3

Servo myservo;

long duration;
float distance;
const int minDistance = 10; // Minimum distance to trigger LED (in cm)

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT); 
  myservo.attach(4);
  Serial.begin(115200);
}

void loop() {
  for (int angle = 0; angle <= 180; angle += 5) {
    myservo.write(angle);
    delay(100);
    measureDistance();
  }

  for (int angle = 180; angle >= 0; angle -= 5) {
    myservo.write(angle);
    delay(100);
    measureDistance();
  }
}

void measureDistance() {
  // Send a 10us pulse to trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the duration of the echo
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in cm
  distance = duration * 0.034 / 2;

  // Print distance to Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Light up LED if object is nearby
  if (distance > 0 && distance <= minDistance) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
