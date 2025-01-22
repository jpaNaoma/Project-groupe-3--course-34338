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


const int photocellPin = A0; ///< Analog pin connected to the photocell sensor.
const int tiltPin = 15; ///< 15 = D8, Digital pin connected to the tilt switch.

int prevStateTilt = LOW; ///< Previous state of the tilt switch.
int counterPhotoTilt = 0; ///< Counter for sensor readings.
int sumPhotocell = 0; ///< Accumulated photocell sensor values.



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


  pinMode(photocellPin, INPUT);
  pinMode(tiltPin, INPUT);
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
int loopCounter = 0; // Global counter to track loop iterations


void loop() {
  int valuePhotocell = analogRead(photocellPin); ///< Read the current photocell value.
  int currentStateTilt = digitalRead(tiltPin);  ///< Read the current tilt switch state.
  unsigned long currentMillis = millis();


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



  uint8_t buf[25 + 1];
  uint8_t buflen = sizeof(buf);
  // Check if received packet is correct size
    if (rf_driver.recv(buf, &buflen)){
      buf[buflen] = '\0'; // will not overflow since we added the +1 
      // Message received with valid checksum
      if (strcmp((char*)buf , "LockedNFar") == 0){
        
        door=0;
alarm = 0;
         Serial.println("Locking the door");
        lock.write(160);
      }
      else if(strcmp((char*)buf , "LockedNClose") == 0) {
    Serial.println("Locking the door223");
    door = 0;

    alarm = 0; // Ensure the alarm starts off
    loopCounter = 0; // Reset the loop counter

    lock.write(160); // Lock the door immediately

    
}



      else if(strcmp((char* )buf , "UnlockNFar") == 0){
         Serial.println("Unlocking the door");
        
        door=1;
        alarm = 0;
        
        lock.write(0);
      }
      else if(strcmp((char* )buf , "UnlockNClose") == 0){
         Serial.println("Unlocking the door");
        
        door=1;
        alarm = 0;

        lock.write(0);
      }
      else{

        Serial.print("Message Received: ");
        Serial.print((char*)buf);
      }

    }


     if (loopCounter < 10) {
    loopCounter++;
      } else if (loopCounter == 10 && alarm == 0 && door == 0) {
    alarm = 1; // Trigger the alarm
    Serial.println("Alarm goes off");
    loopCounter = 0;
    }

 // Message handling logic here

    if (currentMillis - previousMillis >= postDelay) {
        previousMillis = currentMillis;

        // Send data to ThingSpeak
        ThingSpeak.setField(1, door);
        ThingSpeak.setField(2, alarm);
        ThingSpeak.setField(3, currentStateTilt);
        int response = ThingSpeak.writeFields(channelID, APIKey);

        if (response == 200) {
            Serial.println("Data sent to ThingSpeak successfully!");
        } else {
            Serial.print("Error sending data to ThingSpeak: ");
            Serial.println(response);
        }
    }
}