#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for SSD1306 display connected to custom I2C pins
#define OLED_RESET    -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Custom I2C pins
#define I2C_SDA 21  // GPIO21
#define I2C_SCL 22  // GPIO22

unsigned long myTime;

void setup() {
  Serial.begin(115200);

  // Initialize I2C with custom SDA and SCL pins
  Wire.begin(I2C_SDA, I2C_SCL); 
  
  // Initialize OLED display with the I2C address (0x3C) and custom I2C pins
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Clear the buffer
  display.clearDisplay();
  
  // Display initial message
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(F("LIKE SHARE"));
  display.println(F("Yellow Purple"));
  display.display();  // Show initial message on the OLED

}

void loop() {

  Serial.print("Time since boot (ms): ");
  myTime = millis();
  Serial.println(myTime); // prints milliseconds since program started
  delay(1000);
  
}
