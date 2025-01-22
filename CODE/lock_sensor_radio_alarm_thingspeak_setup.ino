/**
 * @file lock_sensor_radio_alarm_thingspeak_setup.ino
 * @author Jarl Pallesen, Thomas Niebuhr, Anders Hansen
 * @version 0.1
 * @date 2025-01-22
 * 
 * @brief This program integrates multiple hardware components to provide a secure locking system with alert mechanisms. 
 * It uses Wi-Fi to send data to ThingSpeak for monitoring and logging, and employs RF communication for inter-module data transfer.
 * 
 * @details The system controls a lock, monitors tilt switch and light sensor states, and triggers an alarm when required.
 * ThingSpeak is used for real-time monitoring, while the servo and RF receiver manage the lock mechanism.
 * 
 * @copyright Copyright (c) 2025
 */

#include <RH_ASK.h> ///< RadioHead library for RF communication.
#include "SPI.h" ///< SPI library for serial communication.
#include "Servo.h" ///< Servo library for controlling the lock mechanism.
#include <ESP8266WiFi.h> ///< ESP8266 library for Wi-Fi connectivity.
#include <ThingSpeak.h> ///< ThingSpeak library for cloud-based data logging.

// Variables for tilt switch and light sensor
const int photocellPin = A0; ///< @brief Analog pin connected to the photocell sensor.
const int tiltPin = 4; ///< @brief Digital pin connected to the tilt switch.

int prevStateTilt = LOW; ///< @brief Previous state of the tilt switch. 0 = LOW, 1 = HIGH.
int counterPhotoTilt = 0; ///< @brief Counter to aggregate light sensor readings.
int sumPhotocell = 0; ///< @brief Sum of light sensor readings over a set period.
int tilt = 0; ///< @brief Tilt state indicator. 0 = normal, 1 = break-in detected.

// Variables for the door and alarm system
int door = 0; ///< @brief Door state indicator. 0 = locked, 1 = unlocked.
int alarm = 0; ///< @brief Alarm state indicator. 0 = off, 1 = triggered.
int i = 0; ///< @brief Counter used in logic processing (unused currently).

// Wi-Fi credentials
const char* ssid = "Xperia_50"; ///< @brief Wi-Fi network SSID.
const char* password = "detvedjegikke"; ///< @brief Wi-Fi password.

// ThingSpeak settings
WiFiClient client; ///< @brief Wi-Fi client object for network communication.
unsigned long channelID = 2814485; ///< @brief ThingSpeak channel ID for data logging.
const char* APIKey = "6COFCODQXCAV6F9O"; ///< @brief ThingSpeak API key for authentication.

Servo lock; ///< @brief Servo object to control the locking mechanism.

// RF communication setup
RH_ASK rf_driver(2000, 2); ///< @brief RF driver object initialized with a 2000 baud rate and D2 as the data pin.

// Variables for ThingSpeak data posting
unsigned long previousMillis = 0; ///< @brief Timestamp for the last ThingSpeak data post.
const unsigned long postDelay = 20000; ///< @brief Delay between ThingSpeak data posts (20 seconds).
int loopCounter = 0; ///< @brief Counter to track iterations of the main loop.

/**
 * @brief Sets up the hardware and network components.
 * 
 * Initializes RF receiver, servo motor, Wi-Fi connection, and ThingSpeak communication. 
 * Configures pins for the tilt switch and light sensor.
 */
void setup() {
    rf_driver.init(); ///< @brief Initializes the RF receiver.
    lock.attach(14); ///< @brief Attaches the servo to pin D5 (GPIO14).
    Serial.begin(115200); ///< @brief Starts serial communication with the PC.
    SPI.begin(); ///< @brief Initializes the SPI bus.

    // Connect to Wi-Fi
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500); ///< @brief Waits for Wi-Fi connection.
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");

    ThingSpeak.begin(client); ///< @brief Initializes ThingSpeak communication.

    pinMode(photocellPin, INPUT); ///< @brief Configures the photocell pin as an input.
    pinMode(tiltPin, INPUT); ///< @brief Configures the tilt switch pin as an input.
}

/**
 * @brief Main operational loop for the system.
 * 
 * Handles RF communication, ThingSpeak data posting, and sensor monitoring. 
 * Manages lock and alarm states based on RF commands and sensor inputs.
 */
void loop() {
    unsigned long currentMillis = millis(); ///< @brief Captures the current system time in milliseconds.

    // RF message handling
    uint8_t buf[25 + 1]; ///< @brief Buffer to store incoming RF data.
    uint8_t buflen = sizeof(buf); ///< @brief Size of the RF data buffer.

    if (rf_driver.recv(buf, &buflen)) { ///< @brief Checks if RF data is received.
        buf[buflen] = '\0'; ///< @brief Null-terminates the received string.
        
        if (strcmp((char*)buf, "LockedNFar") == 0 || strcmp((char*)buf, "LockedNClose") == 0) {
            door = 0; ///< @brief Sets the door state to locked.
            alarm = 0; ///< @brief Turns off the alarm.
            lock.write(160); ///< @brief Locks the door by rotating the servo to 160°.
        } else if (strcmp((char*)buf, "UnlockNFar") == 0 || strcmp((char*)buf, "UnlockNClose") == 0) {
            door = 1; ///< @brief Sets the door state to unlocked.
            alarm = 0; ///< @brief Turns off the alarm.
            lock.write(0); ///< @brief Unlocks the door by rotating the servo to 0°.
        } else {
            Serial.print("Message Received: ");
            Serial.print((char*)buf);
        }
    }

    // Alarm logic
    if (loopCounter < 10) {
        loopCounter++;
    } else if (loopCounter == 10 && alarm == 0 && door == 0) {
        alarm = 1; ///< @brief Triggers the alarm if door is locked and loop reaches 10 iterations.
        Serial.println("Alarm goes off");
        loopCounter = 0; ///< @brief Resets the loop counter.
    }

    // ThingSpeak data posting
    if (currentMillis - previousMillis >= postDelay) {
        previousMillis = currentMillis;
        ThingSpeak.setField(1, door); ///< @brief Sets field 1 to the door state.
        ThingSpeak.setField(2, alarm); ///< @brief Sets field 2 to the alarm state.
        ThingSpeak.setField(3, tilt); ///< @brief Sets field 3 to the tilt state.

        int response = ThingSpeak.writeFields(channelID, APIKey); ///< @brief Sends data to ThingSpeak.
        if (response == 200) {
            Serial.println("Data sent to ThingSpeak successfully!");
        } else {
            Serial.print("Error sending data to ThingSpeak: ");
            Serial.println(response);
        }
    }

    // Sensor monitoring
    int valuePhotocell = analogRead(photocellPin); ///< @brief Reads the photocell sensor value.
    int currentStateTilt = digitalRead(tiltPin); ///< @brief Reads the tilt switch state.

    sumPhotocell += valuePhotocell; ///< @brief Accumulates photocell readings.
    counterPhotoTilt++; ///< @brief Increments the photocell reading counter.

    if (counterPhotoTilt == 10) {
        if ((abs(sumPhotocell) / 10) > 150) { ///< @brief Checks for significant brightness changes.
            Serial.println("Be on alert. Someone is looking around the system.");
            tilt = 0; ///< @brief Resets tilt indicator.
        }

        if (currentStateTilt != prevStateTilt) { ///< @brief Detects changes in the tilt state.
            if (currentStateTilt == LOW) {
                Serial.println("Forceful break in detected!");
                tilt = 1; ///< @brief Triggers tilt alert.
            }
            prevStateTilt = currentStateTilt; ///< @brief Updates the tilt state.
        }

        sumPhotocell = 0; ///< @brief Resets the photocell sum.
        counterPhotoTilt = 0; ///< @brief Resets the counter.
    }

    delay(200); ///< @brief Adds a short delay between loop iterations.
}
