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


    if (driver.recv(buf, &buflen)){
      buf[buflen] = '\0'; // will not overflow since we added the +1 
      // Message received with valid checksum

      if (strcmp((char *)buf , "HB") == 0){
      }else if(strcmp((char *)buf , "Lock") == 0){
        Serial.println("Locking the door");
      }else if(strcmp((char *)buf , "Unlock") == 0){
        Serial.println("Unlocking the door");
      }else{
        Serial.print("Message Received: ");
        Serial.println((char*)buf);
      }

    }

}