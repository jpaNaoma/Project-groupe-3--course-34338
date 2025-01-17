/**
 * @file photocell_tiltSwitch.ino
 * @brief Monitors a photocell sensor and a tilt switch to detect potential tampering or break-ins.
 *
 * This program reads data from a photocell sensor and a tilt switch. Based on the readings, it detects
 * suspicious activities like tampering or a forceful break-in and sends alerts.
 */

const int photocellPin = A0; ///< Analog pin connected to the photocell sensor.
const int tiltPin = 8; ///< Digital pin connected to the tilt switch.

int prevStateTilt = LOW; ///< Previous state of the tilt switch.
int counterPhotoTilt = 0; ///< Counter for sensor readings.
int sumPhotocell = 0; ///< Accumulated photocell sensor values.

/**
 * @brief Sets up the sensor pins.
 */
void setup() {
    pinMode(photocellPin, INPUT);
    pinMode(tiltPin, INPUT);
    Serial.begin(9600);
}

/**
 * @brief Main loop to read sensor data, process it, and generate alerts.
 *
 * - Reads photocell and tilt switch values.
 * - Accumulates photocell readings to calculate an average every 10 cycles.
 * - Checks for tilt state changes and triggers alerts for suspicious activities.
 */
void loop() {
    int valuePhotocell = analogRead(photocellPin); ///< Read the current photocell value.
    int currentStateTilt = digitalRead(tiltPin);  ///< Read the current tilt switch state.

    sumPhotocell += valuePhotocell; ///< Add photocell reading to sum.
    counterPhotoTilt++; ///< Increment reading counter.

    if (counterPhotoTilt == 10) { ///< Process data every 10 readings.
        if ((abs(sumPhotocell) / 10) > 150) { ///< Check for high average changes brightness.
            Serial.println("Be on alert. Someone is looking around the system.");
        }

        if (currentStateTilt != prevStateTilt) { ///< Detect tilt state changes.
            if (currentStateTilt == LOW) {
                Serial.println("Forceful break in"); ///< Alert for a break-in.
            }
            prevStateTilt = currentStateTilt; ///< Update tilt state.
        }

        sumPhotocell = 0; ///< Reset photocell sum.
        counterPhotoTilt = 0; ///< Reset counter.
    }

    delay(200); 
}
