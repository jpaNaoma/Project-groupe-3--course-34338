// Include RadioHead Amplitude Shift Keying Library
#include <RH_ASK.h>
// Include dependant SPI Library
#include <SPI.h>

#include <WString.h>
// Create Amplitude Shift Keying Object
RH_ASK rf_driver;

unsigned long lastTransmitTime = 0;
const unsigned long heartbeatInterval = 500; // Send a "heartbeat" every second

void setup() {
  // Initialize ASK Object
  rf_driver.init();
  Serial.begin(115200);
}

void loop() {
  String input;
  if (Serial.available() > 0) {
    // Read the input string
    input = Serial.readString();
    Serial.print("Sending: ");
    Serial.println(input);

    // Convert String to const char*
    const char *msg = input.c_str();

    // Transmit the message
    rf_driver.send((uint8_t *)msg, strlen(msg));
    rf_driver.waitPacketSent();
   // delay(100);
  } else {
    // Send a periodic "heartbeat" to maintain synchronization
        unsigned long currentTime = millis();
        if (currentTime - lastTransmitTime > heartbeatInterval) {
            const char *heartbeat = "HB"; // Heartbeat message
            rf_driver.send((uint8_t *)heartbeat, strlen(heartbeat));
            rf_driver.waitPacketSent();
            lastTransmitTime = currentTime;
  }
}
}
