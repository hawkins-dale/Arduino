#include <U8g2lib.h>     // U8G2 OLED library
#include <Wire.h>        // I2C library

unsigned long myTime;

// Initialize the display using software I2C with custom pins
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 4, /* data=*/ 5, /* reset=*/ U8X8_PIN_NONE);
  
void setup() {
  Serial.begin(115200);  // Initialize serial communication for debugging

  // Initialize the OLED display
  u8g2.begin();
  
  // Display initial message
  u8g2.clearBuffer();                // Clear the internal memory
  u8g2.setFont(u8g2_font_ncenB14_tr); // Choose a suitable font
  u8g2.drawStr(0, 10, "Yellow Purple"); // Print a message
  u8g2.drawStr(0, 30, "HELLO!");  // Print a second message
  u8g2.sendBuffer();                 // Transfer internal memory to the display
}

void loop() {
  Serial.print("Time since boot (ms): ");
  myTime = millis();
  Serial.println(myTime); // prints milliseconds since program started
  delay(1000);
}
