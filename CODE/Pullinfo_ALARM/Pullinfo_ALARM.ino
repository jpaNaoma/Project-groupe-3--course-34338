/**
 * @file Pullinfo_ALARM.ino
 * @author Federico Morace
 * @brief A script for communicating with ThingSpeak to retrieve information 
 *        and decide whether to activate or deactivate the alarm system.
 *       
 * @version 0.1
 * @date 2025-01-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */


#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <ThingSpeak.h>

const uint8_t RGBred=D5;
const uint8_t RGBgreen=D6;
const uint8_t RGBblue=D7;
const uint8_t buzzer=D3;

LiquidCrystal_I2C lcd(0x27, 16, 2);  // setting the LCD 


//Wi-Fi informations
const char* ssid = "Xperia_50";
const char* password = "detvedjegikke";

// ThingSpeak channel info
const unsigned long channelID = 2814485; //INSERT CHANNEL ID
const char* readAPIKey = "DF9UCEIB1R550QNT"; // INSERT API KEY

WiFiClient client; // Create a WiFiClient object to enable communication over TCP/IP.



/* function police: alarm system, combination of red and blue light and speaker alarm*/
void police(){
  lcd.clear();

  for (int i=0; i<5; i++)   //The alarm goes off for 5 seconds
    {
      lcd.setCursor(0,0);
      lcd.print("Alarm!");

      digitalWrite(RGBred,HIGH);
      digitalWrite(RGBblue,LOW);
      tone(buzzer, 466);

      delay(500);
      tone(buzzer, 622);
      digitalWrite(RGBred,LOW);
      digitalWrite(RGBblue,HIGH);
      lcd.clear();
      delay(500);
      
    }
  digitalWrite(RGBred,LOW);
  digitalWrite(RGBblue,LOW);
  noTone(buzzer);
}



void setup() {
  Serial.begin(115200);
  
  pinMode(RGBred, OUTPUT);
  pinMode(RGBgreen, OUTPUT);
  pinMode(RGBblue, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(RGBred,LOW);
  digitalWrite(RGBblue,LOW);
  digitalWrite(RGBgreen,LOW);
 
 
  //Set the WiFi to station so that the esp can just receive information from the Thingspeak
  WiFi.mode(WIFI_STA);
  delay(100);

  //Start the connection with the WiFi
  WiFi.begin(ssid, password);

  //Checking the connection to the WiFi and breaking the operation if it takes too long
  int startTime=millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");

    if(millis()-startTime>30000) // 30-second timeout
    {
      Serial.println("\nUnable to connect to Wi-Fi");
      break;
    }
  }
  
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("\nConnected to Wi-Fi!");
  }
  
  //Retrying to connect to the Wi-Fi if the connection failed previously
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Retrying to connect to the Wi-Fi");
    WiFi.begin(ssid, password);    
    int retryTime=millis();

    while(WiFi.status() != WL_CONNECTED && millis()-retryTime<60000) //Time given to reconnect is 60s
    {
      delay(1000);
      Serial.print(".");
    }
    if(WiFi.status() == WL_CONNECTED){
      Serial.println("\nReconnected to Wi-Fi!");
    }
    else
    {
      Serial.println("\nFailed to connect to Wi-Fi!");
      digitalWrite(RGBred, HIGH);
    }
  }

  
  // Initialize connection between ESP8266 and ThingSpeak
  ThingSpeak.begin(client);

  //Setting the LCD
  lcd.init();
  lcd.clear(); 
  lcd.backlight();
}


void loop(){

  
  // Read values from ThingSpeak (field1: door status, field2: ultrasonic sensor, field3: light sensor)
  int intdoorStatus = ThingSpeak.readIntField(channelID, 1, readAPIKey); //alarm when sensor detect a person for 20 seconds
  int intthreatDetected = ThingSpeak.readIntField(channelID, 2, readAPIKey);
  int intlightDetected = ThingSpeak.readIntField(channelID, 3, readAPIKey);
  
  // Check if the read operation was successful
  if (intdoorStatus == -1 || intthreatDetected == -1) { //|| intlightDetected == -1) {
    Serial.println("Error reading from ThingSpeak.");
    delay(20000);  
    return;
  }

  // Converts the values from ThingPseak into booleans
  bool doorStatus = (intdoorStatus == 0);   
  bool threatDetected = (intthreatDetected == 1); 
  bool lightDetected = (intlightDetected == 1);  
  

  // Display the values on the serial monitor
  Serial.print("Door status: ");
  Serial.println(doorStatus ? "Closed" : "Open");
  Serial.print("Threat detected: ");
  Serial.println(threatDetected ? "Yes" : "No");
  Serial.print("Light detected: ");
  Serial.println(lightDetected ? "Yes" : "No");
    
  /*Conditions required to trigger the alarm:
      - the door is closed, the threat is detected, the light is on;
      - the door is closed, the threat is detected, the light is off;
      - the door is closed, the threat is not detected, the light is on; */
  
  if(doorStatus && (threatDetected || lightDetected)){
    Serial.println("Activate alarm!");  
    police();
  }
  else{
    digitalWrite(RGBgreen, HIGH);
    Serial.println("Alarm deactivated.");
    
    lcd.clear();
    lcd.setCursor(16,0);
    lcd.print("Alarm deactivated");
    for (int i = 0; i < 17; i++) {  
      lcd.scrollDisplayLeft();      
      delay(300);               
    }
      
  }

}
