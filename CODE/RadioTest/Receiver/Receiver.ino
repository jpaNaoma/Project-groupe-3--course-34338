#include <RH_ASK.h> // RadioHead library, downloaded from arduino library manager

#include "SPI.h"
#include "Servo.h"

Servo lock;  
// Create Amplitude Shift Keying Object
RH_ASK rf_driver(2000,D4,D3,D2);


void setup() {
  rf_driver.init(); // Initialize receiver
  lock.attach(5);
  Serial.begin(115200);  // Initialize serial communications with the PC
  SPI.begin();                     // Init SPI bus
  delay(4);                        // Optional delay. Some board do need more time after init to be ready, see Readme
}
void loop() {
  uint8_t buf[25 + 1];
  uint8_t buflen = sizeof(buf);
  // Check if received packet is correct size
    if (rf_driver.recv(buf, &buflen)){
      buf[buflen] = '\0'; // will not overflow since we added the +1 
      // Message received with valid checksum
      if (strcmp((char*)buf , "LockedNFar") == 0){
        Serial.println("Locking the door");
        lock.write(160);
      }
      else if(strcmp((char*)buf , "LockedNClose") == 0){
        Serial.println("Locking the door");
        lock.write(160);
      }
      else if(strcmp((char* )buf , "UnlockNFar") == 0){
        Serial.println("Unlocking the door");
        lock.write(0);
      }
      else if(strcmp((char* )buf , "UnlockNClose") == 0){
        Serial.println("Unlocking the door");
        lock.write(0);
      }
      else{
        Serial.print("Message Received: ");
        Serial.println((char*)buf);
      }
    }
}