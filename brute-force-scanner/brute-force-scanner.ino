#include <Wire.h>

const int pins[] = {0, 2, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33};
const int NPINS = sizeof(pins) / sizeof(pins[0]);
const int resetCandidates[] = {-1, 16, 15};   // -1 = no reset pulse

bool probe(int sda, int scl, byte addr) {
  Wire.begin(sda, scl);
  Wire.setTimeOut(5);
  Wire.beginTransmission(addr);
  bool ok = (Wire.endTransmission() == 0);
  Wire.end();
  return ok;
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("Starting brute-force I2C scan...");
  bool found = false;

  for (int r : resetCandidates) {
    if (r >= 0) {
      pinMode(r, OUTPUT);
      digitalWrite(r, LOW);  delay(20);
      digitalWrite(r, HIGH); delay(50);
      Serial.printf("Pulsed reset on GPIO %d\n", r);
    }
    for (int i = 0; i < NPINS; i++) {
      for (int j = 0; j < NPINS; j++) {
        if (i == j) continue;
        int sda = pins[i], scl = pins[j];
        if (sda == r || scl == r) continue;
        for (byte addr : {0x3C, 0x3D}) {
          if (probe(sda, scl, addr)) {
            Serial.printf("FOUND 0x%02X  SDA=%d SCL=%d  reset=%d\n", addr, sda, scl, r);
            found = true;
          }
        }
      }
    }
  }
  Serial.println(found ? "Done." : "Done. Nothing found.");
}

void loop() {}