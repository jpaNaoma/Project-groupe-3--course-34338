// github files are from https://github.com/miguelbalboa/rfid/tree/master
/**
 * @section Pin connections
 * SDA | RX | SS is connected to SS_pin 
 * SCK is pin 52 (Mega2560)
 * MOSI is pin 51 (Mega2560)
 * MISO is pin 50 (Mega2560)
 * RST | Reset is connected to RST_pin
 * LED is connected to LED_pin and is active high
 *  The LED should have a resistor to limit current (Using 220R, variation changes light level)
 *     
 */
#include "SPI.h"
#include "MFRC522.h"  // must add files from github

#define SS_pin 53  // slave select
#define RST_pin 5  // reset pin

#define LED_pin 2 // pin for turning on and off status led

MFRC522 rfid(SS_pin, RST_pin);
MFRC522::MIFARE_Key key;

unsigned char nuidPICC[4];
unsigned char AccessPICC[4] = {0x83,0xE8,0x1F, 0x16};
char ControlChar = (1<<0); // 1st bit is locked, starts being locked
void setup() {
  Serial.begin(115200);  // Initialize serial communications with the PC
  pinMode(LED_pin, OUTPUT);
  digitalWrite(LED_pin, ControlChar && (1<<0) );

  while (!Serial); // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();                     // Init SPI bus
  rfid.PCD_Init();                 // Init MFRC522
  delay(4);                        // Optional delay. Some board do need more time after init to be ready, see Readme
  rfid.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}
void loop() {
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
  }else{
    Serial.println("This is wrong key. Go away intruder!!!");
    if( !ControlChar & (1<<0) ){
      ControlChar |= (1<<0); // Lock the door
      Serial.println("Locking the door");
    }
  }
  digitalWrite(LED_pin, ControlChar && (1<<0) );
}


/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
