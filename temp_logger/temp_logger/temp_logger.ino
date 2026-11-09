#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>  // Library Manager: "Adafruit AHTX0" (also installs Adafruit BusIO)

// I2C pins for the AHT20
const int I2C_SDA = 27;
const int I2C_SCL = 22;

Adafruit_AHTX0 aht;
bool sensorReady = false;

// ---- Configuration ---------------------------------------------------------
const char* WIFI_SSID     = "Gilgamesh";
const char* WIFI_PASSWORD = "12345678";

// Your Mac's IP address (or its .local name, e.g. "my-mac.local").
// A DHCP reservation on your router keeps the IP from changing.
const char* SERVER_HOST = "192.168.4.23";
const int   SERVER_PORT = 5050;

const unsigned long LOG_INTERVAL_MS = 30UL * 1000UL;  // 30 seconds
// ---------------------------------------------------------------------------

// Reads the AHT20. Returns true and fills tempC / humidityPct on success.
bool readSensor(float &tempC, float &humidityPct) {
  if (!sensorReady) {
    // Try to (re)initialise in case the sensor wasn't ready at boot.
    sensorReady = aht.begin(&Wire);
    if (!sensorReady) return false;
  }

  sensors_event_t humidity, temp;
  if (!aht.getEvent(&humidity, &temp)) return false;

  tempC = temp.temperature;
  humidityPct = humidity.relative_humidity;
  return !isnan(tempC) && !isnan(humidityPct);
}

void connectWiFi() {
  Serial.println("connecting to wifi");

  if (WiFi.status() == WL_CONNECTED) return;

  Serial.printf("Connecting to %s", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection failed, will retry.");
  }
}

bool postReading(float tempC, float humidityPct) {
  char url[160];
  snprintf(url, sizeof(url), "http://%s:%d/log?temp=%.2f&humidity=%.1f",
           SERVER_HOST, SERVER_PORT, tempC, humidityPct);

  HTTPClient http;
  http.setTimeout(5000);
  if (!http.begin(url)) {
    Serial.println("http.begin failed");
    return false;
  }

  int code = http.POST("");  // data is in the query string, so the body is empty
  http.end();

  if (code == 200) {
    Serial.printf("Logged %.2f C, %.1f %% RH\n", tempC, humidityPct);
    return true;
  }
  Serial.printf("POST failed, code: %d\n", code);
  return false;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("starting");

  Wire.begin(I2C_SDA, I2C_SCL);
  sensorReady = aht.begin(&Wire);
  Serial.println(sensorReady ? "AHT20 found." : "AHT20 not found, check wiring (will retry).");

  connectWiFi();
}

void loop() {


  if (WiFi.status() == WL_CONNECTED) {
    float t, h;
    if (readSensor(t, h)) {
      postReading(t, h);
    } else {
      Serial.println("Sensor read failed, skipping this sample.");
    }
  }

  delay(LOG_INTERVAL_MS);
}
