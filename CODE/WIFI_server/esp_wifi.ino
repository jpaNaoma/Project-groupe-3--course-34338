#include <ESP8266WiFi.h>
#include "ThingSpeak.h" // Ensure you have the ThingSpeak library installed

// Wi-Fi credentials
const char* ssid = "Jarls Galaxy";
const char* pass = "jarlvep26";

// ThingSpeak settings
WiFiClient client;
unsigned long channelID = 2808163;
const char* APIKey = "XS1Z5OLYIGIWYLPP";
const int postDelay = 20 * 1000; // 20-second delay between updates

// GPIO pin for the LED
const int ledPin = LED_BUILTIN;
bool ledState = 0;

void setup() {
  Serial.begin(115200);

  // Initialize LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Start with LED off

  // Connect to Wi-Fi
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Get WiFi RSSI
  int rssi = WiFi.RSSI();
  Serial.print("RSSI: ");
  Serial.println(rssi);

  // Toggle LED state
  ledState = !ledState; // Toggle state
  digitalWrite(ledPin, ledState);
  Serial.print("LED State: ");
  Serial.println(ledState ? "ON" : "OFF");

  // Set ThingSpeak fields
  ThingSpeak.setField(1, rssi);      // Field 1: WiFi RSSI
  ThingSpeak.setField(2, ledState);  // Field 2: LED state (1 for ON, 0 for OFF)

  // Post data to ThingSpeak
  int responseCode = ThingSpeak.writeFields(channelID, APIKey);
  if (responseCode == 200) {
    Serial.println("Data successfully sent to ThingSpeak!");
  } else {
    Serial.print("Error sending data. Response code: ");
    Serial.println(responseCode);
  }

  // Part 17c: Explore reading from ThingSpeak
  Serial.println("\nData successfully read from ThingSpeak!");
  float field1Data = ThingSpeak.readFloatField(channelID, 1); // Read Field 1 data
  float field2Data = ThingSpeak.readFloatField(channelID, 2); // Read Field 2 data
  Serial.print("Field 1 (RSSI): ");
  Serial.println(field1Data);
  Serial.print("Field 2 (LED State): ");
  Serial.println(field2Data);

  delay(postDelay); // Wait for the next cycle
}
