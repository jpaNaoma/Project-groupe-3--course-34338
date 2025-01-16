// Include RadioHead Amplitude Shift Keying Library
#include <RH_ASK.h>
// Include dependant SPI Library 
#include <SPI.h> 
 
// Create Amplitude Shift Keying Object
RH_ASK rf_driver;
 
void setup()
{
    // Initialize ASK Object
    rf_driver.init();
    // Setup Serial Monitor
    Serial.begin(115200);
}
 
void loop()
{
    // Set buffer to size of expected message
    uint8_t buf[12 + 1];
    uint8_t buflen = sizeof(buf);
    // Check if received packet is correct size
    if (rf_driver.recv(buf, &buflen))
    {
      buf[buflen] = '\0'; // will not overflow since we added the +1 
      // Message received with valid checksum
      Serial.print("Message Received: ");
      Serial.println((char*)buf);         
    }
}