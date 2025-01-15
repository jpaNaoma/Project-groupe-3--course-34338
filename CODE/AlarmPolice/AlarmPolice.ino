/*#include "pitches.h" */
#include <LiquidCrystal.h>

const uint8_t RGBred=7;
const uint8_t RGBgreen=8;
const uint8_t RGBblue=9;
const uint8_t buzzer=6;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/* function police: alarm system, combination of red and blue light and speaker alarm*/
void police(){
  
  for (int i=0; i<20; i++)
    {
      lcd.setCursor(0,0);
      lcd.print("ALARM!");
      lcd.noBlink();
      digitalWrite(RGBred,HIGH);
      digitalWrite(RGBgreen,LOW);
      digitalWrite(RGBblue,LOW);
      tone(buzzer, 466);

      delay(500);
      lcd.blink();
      tone(buzzer, 622);
      digitalWrite(RGBred,LOW);
      digitalWrite(RGBgreen,LOW);
      digitalWrite(RGBblue,HIGH);
      lcd.clear();
      delay(500);
      
      
      
      i++;
    }
  digitalWrite(RGBred,LOW);
  digitalWrite(RGBgreen,LOW);
  digitalWrite(RGBblue,LOW);
  noTone(buzzer);
}


void setup() {
  // put your setup code here, to run once:
  // set up the LCD's number of columns and rows:
lcd.begin(16, 2);
pinMode(RGBred, OUTPUT);
pinMode(RGBgreen, OUTPUT);
pinMode(RGBblue, OUTPUT);
pinMode(buzzer, OUTPUT);
police();

}

void loop() {
  // put your main code here, to run repeatedly:
    
    
}
