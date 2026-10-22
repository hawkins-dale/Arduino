#include <Wire.h>

void scan(int sda, int scl) {
  Wire.begin(sda, scl);
  Serial.printf("Scanning SDA=%d SCL=%d\n", sda, scl);
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  Found device at 0x%02X\n", addr);
    }
  }
  Wire.end();
}

void setup() {
  Serial.begin(115200);
  Serial.write("Starting: ");
  delay(1000);
  Serial.write("scanning 21,22 ");
  scan(21, 22);   // documented pins
  Serial.write("scanning 5, 4 ");
  scan(5, 4);     // common alternate on ideaspark boards
  Serial.write("scanning 21,22 ");
  scan(22, 21);   // documented pins

}

void loop() {}