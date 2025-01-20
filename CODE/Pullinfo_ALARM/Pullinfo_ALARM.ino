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
const char* ssid = "OUR_SSID";
const char* password = "OUR_PASSWORD";

// ThingSpeak channel info
const unsigned long channelID = ....; //INSERT CHANNEL ID
const char* readAPIKey = "OUR_READ_API_KEY"; // INSERT API KEY

WiFiClient client;



/* function police: alarm system, combination of red and blue light and speaker alarm*/
void police(){
  lcd.clear();

  for (int i=0; i<20; i++)   //The alarm goes off for 20 seconds
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
  lcd.clear();
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
  digitalWrite(RGBgreen,LOW);
 
 
  //Set the WiFi to station so that the esp can just receive information from the Thingspeak
  WiFi.mode(WIFI_STA);

  //Start the connection with the WiFi
  WiFi.begin(ssid, password);
  

  //Checking the connection to the WiFi
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");

  // Initialize connection between ESP8266 and ThingSpeak
  ThingSpeak.begin(client);

  //Setting the LCD
  lcd.init();
  lcd.clear(); 
  lcd.backlight();
}


void loop(){

  // Read values from ThingSpeak (field1: door status, field2: ultrasonic sensor, field3: light sensor)
  bool doorStatus = ThingSpeak.readIntField(channelID, 1, readAPIKey) == 1;   
  bool threatDetected = ThingSpeak.readIntField(channelID, 2, readAPIKey) == 1; 
  bool lightDetected = ThingSpeak.readIntField(channelID, 3, readAPIKey) == 1;  
  
  // Check if the read operation was successful
  if (doorStatus == -1 || threatDetected == -1 || lightDetected == -1) {
    Serial.println("Error reading from ThingSpeak.");
    delay(20000);  
    return;
  }

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
  
  if(doorStatus && threatDetected && lightDetected ){
    Serial.println("Activate alarm!");  
    police();
  }
  else if (doorStatus && threatDetected && !lightDetected){
    Serial.println("Activate alarm!");
    police();
  }
  else if (doorStatus && !threatDetected && lightDetected ){
    Serial.println("Activate alarm!");
    police();
  }
  else{
    Serial.println("Alarm deactivated.");
    lcd.clear();
    for(int t=0; t<10;t++){
      lcd.setCursor(16,0);
      lcd.print("Alarm deactivated");
      for (int i = 0; i < 17; i++) {  
        lcd.scrollDisplayLeft();      
        delay(300);               
      }
      lcd.clear();
    }
  
  }



  delay(20000);
  
}
