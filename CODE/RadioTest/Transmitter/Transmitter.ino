/**
 * @section Pin connections
 * SDA | RX | SS is connected to SS_pin 
 * SCK is pin 52 (Mega2560)
 * MOSI is pin 51 (Mega2560)
 * MISO is pin 50 (Mega2560)
 * RST | Reset is connected to RST_pin
 * LED is connected to LED_pin and is active high
 *  The LED should have a resistor to limit current (Using 220R, variation changes light level)
 * @see RFID Library & use examples are from https://github.com/miguelbalboa/rfid/tree/master, Inspiration was used.    
 * @see Radio Frequency Transmission are from https://lastminuteengineers.com/433mhz-rf-wireless-arduino-tutorial/
 * Servo motor uses 5V (Red), GND (brown) and digital pin Servo_pin
 * @see Servo Library is added from due to conflicting timers https://github.com/nabontra/ServoTimer2/tree/master
 */
#include <RH_ASK.h> // RadioHead library, downloaded from arduino library manager

#include "SPI.h"

// Create Amplitude Shift Keying Object
// RH_ASK driver;
  RH_ASK driver(2000, 4,5,0);

/** @brief Sends a string using Radio Frequency using RadioHead library
*   @param Takes a char pointer / string as input
*/
void RadioSend(const char *msg){
  rf_driver.send((uint8_t *)msg, strlen(msg));
  rf_driver.waitPacketSent();
}

void setup() {
  driver.init(); // Initialize receiver

    
  Serial.begin(115200);  // Initialize serial communications with the PC

  // while (!Serial); // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();                     // Init SPI bus
  delay(4);                        // Optional delay. Some board do need more time after init to be ready, see Readme
}


void loop() {
  uint8_t buf[20 + 1];
  uint8_t buflen = sizeof(buf);
  // Check if received packet is correct size
// Serial.println("-------------------------------------------------------------------");

    // Transmit the message
    RadioSend(msg);
  } else {
    // Send a periodic "heartbeat" to maintain synchronization
        unsigned long currentTime = millis();
        if (currentTime - lastTransmitTime > heartbeatInterval) {
            RadioSend("HB");
            lastTransmitTime = currentTime;
  }
}
}


