#include <Wire.h>

int resetPins[] = {16, 4, 15};
int numPins = 3;

void scanI2C(int sda, int scl) {
  Serial.print("Testing I2C Pins -> SDA: ");
  Serial.print(sda);
  Serial.print(", SCL: ");
  Serial.println(scl);
  
  Wire.begin(sda, scl);
  // Lower the clock speed slightly to guarantee stable discovery
  Wire.setClock(100000); 

  byte error, address;
  int devices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("  >>> SUCCESS! Found device at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" <<<");
      devices++;
    }
  }
  if (devices == 0) {
    Serial.println("  No devices found on this pair.");
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  delay(2000);
  Serial.println("\n===== BEGIN IDEASPARK ADVANCED COMBINATION SCAN =====");

  // Loop through each possible reset pin
  for (int i = 0; i < numPins; i++) {
    int pin = resetPins[i];
    
    // Test 1: Try driving the reset pin HIGH
    Serial.printf("\n--- Setting GPIO %d to HIGH ---\n", pin);
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    delay(200); // Give the OLED controller time to wake up
    
    scanI2C(21, 22);
    scanI2C(5, 4);

    // Test 2: Try driving the reset pin LOW (Active Low Reset layout)
    Serial.printf("\n--- Setting GPIO %d to LOW ---\n", pin);
    digitalWrite(pin, LOW);
    delay(200);
    
    scanI2C(21, 22);
    scanI2C(5, 4);
    
    // Clean up pin state before checking next
    digitalWrite(pin, HIGH); 
  }
  
  Serial.println("\n===== SCAN COMPLETE =====");
}

void loop() {
  // Put nothing here so the scan only runs once at boot
}
