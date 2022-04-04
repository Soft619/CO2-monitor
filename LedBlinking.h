#include <SoftwareSerial.h>
#include <Wire.h>

// I2C OLED
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <Adafruit_GFX.h>
#include <gfxfont.h>

#define I2C_ADDRESS 0x3C
#define sice
#define LED_PIN 9

SSD1306AsciiWire oled;

// CO2 sensor:
SoftwareSerial mySerial(2,3); // RX,TX  

// Autocalibration setup
int calib = 0;  // set autocalibration on (1) or off (0)  
byte calibon[9] = {0xff, 0x01, 0x79, 0xA0, 0x00, 0x00, 0x00, 0x00}; //autocalibration on command
byte caliboff[9] = {0xff, 0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00}; //autocalibration off command

byte cmd[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79}; 
unsigned char response[9];

void setup() {
  
  // Serial
  Serial.begin(9600);
  mySerial.begin(9600);

  // OLED
  Wire.begin();         
  oled.begin(&Adafruit128x32, I2C_ADDRESS);
  oled.set400kHz();  
  oled.setFont(ZevvPeep8x16);
 
  oled.clear();  
  oled.println("   CO2 MONITOR");
  oled.println("Sviridov AV 2022");
  delay(5000); 
}

long t = 0;

void loop() 
{
  
if (calib >= 1) { 
  mySerial.write(calibon, 9);
} else {
  mySerial.write(caliboff, 9);
}
  mySerial.write(cmd, 9);
  memset(response, 0, 9);
  mySerial.readBytes(response, 9);
  int i;
  byte crc = 0;
  for (i = 1; i < 8; i++) crc+=response[i];
  crc = 255 - crc;
  crc++;

  oled.clear();  
    if ( !(response[0] == 0xFF && response[1] == 0x86 && response[8] == crc) ) {
       // Serial.println("CRC error: " + String(crc) + " / "+ String(response[8]));
        oled.begin(&Adafruit128x32, I2C_ADDRESS);
        oled.set400kHz();
        oled.setFont(ZevvPeep8x16);
        oled.println("Sensor CRC error");
        oled.println("Reseting...");
        delay(5000);
        asm volatile("jmp 0x00");
    } else {
        unsigned int responseHigh = (unsigned int) response[2];
        unsigned int responseLow = (unsigned int) response[3];
        unsigned int ppm = (256*responseHigh) + responseLow;
    Serial.print(String(t)); Serial.print(","); Serial.print(ppm); Serial.println(";");
    
    if (ppm <= 200 || ppm > 6000) {
        oled.begin(&Adafruit128x32, I2C_ADDRESS);
        oled.set400kHz();
        oled.setFont(ZevvPeep8x16);
        oled.println("CO2: no data");
        delay(10000);          
    } else {
        oled.begin(&Adafruit128x32, I2C_ADDRESS);
        oled.set400kHz();  
        oled.setFont(Verdana_digits_24);
        oled.setLetterSpacing(8);

  // center indication 
        size_t size = oled.strWidth(ppm);

    if (ppm < 1000) {
        oled.setCursor((128-size)/4, 1);
          if (calib >= 1) {
              oled.setFont(ZevvPeep8x16);
              oled.setLetterSpacing(20);
              oled.println(String(ppm));
          } else {
              oled.setFont(Verdana_digits_24);
              oled.setLetterSpacing(8);
              oled.println(String(ppm));
          }
    } else {
        oled.setCursor((128-size)/5, 1);
          if (calib >= 1) {
              oled.setFont(ZevvPeep8x16);
              oled.setLetterSpacing(20);
              oled.println(String(ppm));
          } else {
              oled.setFont(Verdana_digits_24);
              oled.setLetterSpacing(8);
              oled.println(String(ppm));
          }
    }

  // led brightness
     if (ppm < 950) {   
        analogWrite(LED_PIN, 0);
        delay(10000); 
     } else if (ppm < 1600) {   
        analogWrite(LED_PIN, 10);
        delay(10000);
     } else if (ppm < 2000) {  
        analogWrite(LED_PIN, 50);
        delay(10000); 
     } else if (ppm < 2500) {  
        analogWrite(LED_PIN, 150);
        delay(10000); 
     } else {   
  // led blinking
        analogWrite(LED_PIN, 255);
        delay(1000); 
        analogWrite(LED_PIN, 0);
        delay(1000);
        analogWrite(LED_PIN, 255);
        delay(1000); 
        analogWrite(LED_PIN, 0);
        delay(1000);
        analogWrite(LED_PIN, 255);
        delay(1000); 
        analogWrite(LED_PIN, 0);
        delay(1000);
        analogWrite(LED_PIN, 255);
        delay(1000); 
        analogWrite(LED_PIN, 0);
        delay(1000);
        analogWrite(LED_PIN, 255);
        delay(1000); 
        analogWrite(LED_PIN, 0);
        delay(1000);
     }
   }
 }
  // delay(10000);
  t += 10;
}
