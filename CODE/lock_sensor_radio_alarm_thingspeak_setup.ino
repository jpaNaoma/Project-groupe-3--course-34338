#include <RH_ASK.h> // RadioHead library, downloaded from arduino library manager
#include "SPI.h"
#include "Servo.h"
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>


//variables
int i=0;
int alarm=0;
int door=0;

// Wi-Fi credentials
const char* ssid = "Xperia_50";
const char* password = "detvedjegikke";

// ThingSpeak settings
WiFiClient client; 
unsigned long channelID = 2814485;
const char* APIKey = "6COFCODQXCAV6F9O";
// const int postDelay = 20 * 1000; // 20-second delay

Servo lock;  
// Create Amplitude Shift Keying Object
RH_ASK rf_driver(2000,2);

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch

void setup() {
  rf_driver.init();      // Initialize receiver
  lock.attach(0);        //D3 , GPIO0, "0"
  Serial.begin(115200);  // Initialize serial communications with the PC
  SPI.begin();           // Init SPI bus
  delay(4);              // Optional delay. Some board do need more time after init to be ready, see Readme

  WiFi.begin(ssid, password);

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}


unsigned long previousMillis = 0;
const unsigned long postDelay = 20000; // 20 seconds

void loop() {

 unsigned long currentMillis = millis();


  uint8_t buf[25 + 1];
  uint8_t buflen = sizeof(buf);
  // Check if received packet is correct size
    if (rf_driver.recv(buf, &buflen)){
      buf[buflen] = '\0'; // will not overflow since we added the +1 
      // Message received with valid checksum
      if (strcmp((char*)buf , "LockedNFar") == 0){
        
        door=0;

         Serial.println("Locking the door");
        lock.write(160);
      }
      else if(strcmp((char*)buf , "LockedNClose") == 0){
         Serial.println("Locking the door223");
              door=0;
              
              alarm = 0;
        for (int i=0; i<10; i++)   //The alarm goes off for 10 seconds
              alarm = 1;
         Serial.println("Alarm goes off");
      
        lock.write(160);
      }
      else if(strcmp((char* )buf , "UnlockNFar") == 0){
         Serial.println("Unlocking the door");
        
        door=1;
        
        lock.write(0);
      }
      else if(strcmp((char* )buf , "UnlockNClose") == 0){
         Serial.println("Unlocking the door");
        
        door=1;

        lock.write(0);
      }
      else{

        Serial.print("Message Received: ");
        Serial.print((char*)buf);
      }

    }

 // Message handling logic here

    if (currentMillis - previousMillis >= postDelay) {
        previousMillis = currentMillis;

        // Send data to ThingSpeak
        ThingSpeak.setField(1, door);
        ThingSpeak.setField(2, alarm);
        int response = ThingSpeak.writeFields(channelID, APIKey);

        if (response == 200) {
            Serial.println("Data sent to ThingSpeak successfully!");
        } else {
            Serial.print("Error sending data to ThingSpeak: ");
            Serial.println(response);
        }
    }
}