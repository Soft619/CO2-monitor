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
//#define led_int

SSD1306AsciiWire oled;

//int brightness = 0;    // уставливаем начально значение яркости
//int fadeAmount = 5;    // шаг приращения/убывания яркости


// CO2 sensor:
SoftwareSerial mySerial(2,3); // RX,TX
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
  delay(7000); 

}

long t = 0;

void loop() 
{
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
        Serial.println("CRC error: " + String(crc) + " / "+ String(response[8]));
        oled.begin(&Adafruit128x32, I2C_ADDRESS);
        oled.set400kHz();
        oled.setFont(ZevvPeep8x16);
        oled.println("Sensor CRC error");
  } else {
    unsigned int responseHigh = (unsigned int) response[2];
    unsigned int responseLow = (unsigned int) response[3];
    unsigned int ppm = (256*responseHigh) + responseLow;
    Serial.print(String(t)); Serial.print(","); Serial.print(ppm); Serial.println(";");
    if (ppm <= 400 || ppm > 4900) {
        oled.begin(&Adafruit128x32, I2C_ADDRESS);
        oled.set400kHz();
        oled.setFont(ZevvPeep8x16);
        oled.println("CO2: no data");          
    } else {
     // oled.println("CO2: " + String(ppm) + " ppm"); 
     
     oled.begin(&Adafruit128x32, I2C_ADDRESS);
     oled.set400kHz();  
    // oled.set2X();
   //oled.invertDisplay(true);
     oled.setFont(Verdana_digits_24);
     oled.setLetterSpacing(8);

     // center indication 
  //  if (ppm < 1000) {
  //    oled.setCursor(2, 1);   
  //    oled.println(String(ppm));       
  //  } else {
  //   oled.setCursor(-1, 1); 
  //   oled.println(String(ppm));
  //  }
                 // center indication 
                  size_t size = oled.strWidth(ppm);
               if (ppm < 1000) {
                  oled.setCursor((128-size)/4, 1);
                  oled.println(String(ppm));
               } else {
                  oled.setCursor((128-size)/5, 1);
                  oled.println(String(ppm)); 
               }


     // яркость светодиода
     if (ppm < 450) {   
       analogWrite(LED_PIN, 0); 
      }
      else if (ppm < 600) {   
      //  brightness = ppm / 10
      //  brightness = brightness + fadeAmount
       analogWrite(LED_PIN, 10);
      }
      else if (ppm < 1000) {  
      //  brightness = ppm / 10
      //  brightness = brightness + fadeAmount 
       analogWrite(LED_PIN, 30); 
      }
      else if (ppm < 2000) {  
      //  brightness = ppm / 10
      //  brightness = brightness + fadeAmount 
        analogWrite(LED_PIN, 50); 
      }
      else {   
        analogWrite(LED_PIN, 255); 
             
     }
     
     
 //   oled.setFont(Verdana12_bold);
  //  if (ppm < 450) {   
   //    oled.println("Very good");
   //   }
   //   else if (ppm < 600) {   
    //    oled.println("Good");
   //   }
   //   else if (ppm < 1000) {   
   //    oled.println("Acceptable");
   //   }
   //   else if (ppm < 2500) {   
   //     oled.println("Bad");
   //   }
   //  else {   
   //     oled.println("Health risk");
   //   }
    }
  }
  delay(10000);
  t += 10;
}
