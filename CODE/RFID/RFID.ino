/**
 * @section Pin connections
 * SDA | RX | SS is connected to SS_pin 
 * SCK is pin 52 (Mega2560)
 * MOSI is pin 51 (Mega2560)
 * MISO is pin 50 (Mega2560)
 * RST | Reset is connected to RST_pin
 * LED is connected to LED_pin and is active high
 *  The LED should have a resistor to limit current (Using 220R, variation changes light level)
 * @see Servo Library is added from due to conflicting timers https://github.com/nabontra/ServoTimer2/tree/master
 * @see RFID Library & use examples are from https://github.com/miguelbalboa/rfid/tree/master, Inspiration was used.    
 * @see Radio Frequency Transmission are from https://lastminuteengineers.com/433mhz-rf-wireless-arduino-tutorial/ 
 */
#include <RH_ASK.h> // RadioHead library, downloaded from arduino library manager

#include "SPI.h"
#include "MFRC522.h"  // must add files from github
#include "ServoTimer2.h" // Files downloaded

#define SS_pin 53  // slave select
#define RST_pin 5  // reset pin
#define Servo_Pin 3 // Servo motor pin

#define LED_pin 2 // pin for turning on and off status led

 // UltraSonicSensor
#define trigPin 7
#define echoPin 6

long duration; 
float distance; 
const int minDistance = 10;
int angle = 0;

ServoTimer2 UltraSonicServo;

MFRC522 rfid(SS_pin, RST_pin);
MFRC522::MIFARE_Key key;

// Create Amplitude Shift Keying Object
RH_ASK rf_driver;

unsigned char nuidPICC[4];
unsigned char AccessPICC[4] = {0x83,0xE8,0x1F, 0x16};
char ControlChar = (1<<0); // 1st bit is locked, starts being locked

unsigned long lastTransmitTime = 0;
const unsigned long TransmitInterval = 250; // Send status every 250 ms


void RadioSend(const char *msg){
  rf_driver.send((uint8_t *)msg, strlen(msg));
  rf_driver.waitPacketSent();
}

/**
 * @brief Measures distance using the ultrasonic sensor and controls the onboard LED.
 */
void measureDistance() {
  // Send a 10us pulse to trigger pin.
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the duration of the echo.
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in cm.
  distance = duration * 0.034 / 2;

  // Light up LED if object is nearby.
  if (distance > 0 && distance <= minDistance) {
    digitalWrite(LED_BUILTIN, HIGH); ///< Turn on LED if object is close.
  } else {
    digitalWrite(LED_BUILTIN, LOW); ///< Turn off LED if no object is nearby.
  }
}



/**
* @brief Setups Serial (Not needed for final), Pins, SPI and RFID
*
*
*/
void setup() {
  rf_driver.init(); // Initialize receiver
  
  UltraSonicServo.attach(Servo_Pin);
  UltraSonicServo.write(1500);
  
  pinMode(trigPin, OUTPUT); ///< Set trigger pin as output.
  pinMode(echoPin, INPUT); ///< Set echo pin as input.
  pinMode(LED_BUILTIN, OUTPUT); ///< Set onboard LED pin as output.

  Serial.begin(115200);  // Initialize serial communications with the PC
  pinMode(LED_pin, OUTPUT);
  digitalWrite(LED_pin, ControlChar && (1<<0) );

  SPI.begin();                     // Init SPI bus
  rfid.PCD_Init();                 // Init MFRC522
  delay(4);                        // Optional delay. Some board do need more time after init to be ready, see Readme
  rfid.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

/**
* @brief Main loop for RFID part. Checks the RFID and sends over radio if it 
* should be locked or not. 
* LED shows the status if it's locked or unlocked
* Also measures if something is in close proximity or not to the ultrasonic sensor and then sending 
* via radiofrequency
*/
void loop() {
  unsigned long currentTime = millis(); // Current time
  if(currentTime - lastTransmitTime > TransmitInterval){ // Send status every TransmitInterval
    measureDistance();
    if (ControlChar & 1<<1){
      angle -= 100;
      if (angle <= 0){
        ControlChar &= ~(1<<1);
      }
    }else{
      angle += 100;
      if (angle >= 1500){
        ControlChar |= (1<<1);
      }
    }
    UltraSonicServo.write(angle); ///< Move servo to the current angle.

    if (distance > 0 && distance <= minDistance){
      if ((ControlChar & 1<<0)){ // Sends the status over radio
        RadioSend("LockedNClose");
       // Serial.println("Sending status: LockedNClose");
      }else{
        RadioSend("UnlockNClose");
       // Serial.println("Sending status: UnlockedNClose");
      }
    }else{
      if ((ControlChar & 1<<0)){ // Sends the status over radio
        RadioSend("LockedNFar");
       // Serial.println("Sending status: LockedNFar");
      }else{
        RadioSend("UnlockNFar");
      //  Serial.println("Sending status: UnlockedNFar");
      }
    }
    lastTransmitTime = currentTime; 
  }

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }
  Serial.print(F("The NUID tag is:"));
  printHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

  if (nuidPICC[0] == AccessPICC[0] && 
      nuidPICC[1] == AccessPICC[1] &&
      nuidPICC[2] == AccessPICC[2] &&
      nuidPICC[3] == AccessPICC[3] 
  ){
    Serial.println("Key is allowed, toggling the lock");
    ControlChar ^= (1<<0); 
    digitalWrite(LED_pin, ControlChar & (1<<0) );
  }else{
    Serial.println("This is wrong key. Go away intruder!");
    if( !ControlChar & (1<<0) ){
      ControlChar |= (1<<0); // Lock the door
      Serial.println("Locking the door");
      digitalWrite(LED_pin, ControlChar & (1<<0));
    }
  }
}

/**
 * @brief Helper routine to dump a byte array as hex values to Serial. Will not be needed in final project
 *  
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

