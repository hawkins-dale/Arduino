#include <WiFi.h>
#include <HTTPClient.h>

// ---- Configuration ---------------------------------------------------------
const char* WIFI_SSID     = "Gilgamesh";
const char* WIFI_PASSWORD = "12345678";

// Your Mac's IP address (or its .local name, e.g. "my-mac.local").
// A DHCP reservation on your router keeps the IP from changing.
const char* SERVER_HOST = "192.168.4.23";
const int   SERVER_PORT = 5050;


const unsigned long LOG_INTERVAL_MS = 10UL * 1000UL;  // 10 seconds


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
  Serial.print("About to POST");
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
  Serial.write("starting up");
  // put your setup code here, to run once:
  connectWiFi();
}

void loop() {
  // put your main code here, to run repeatedly:

  connectWiFi();
  if (WiFi.status() == WL_CONNECTED) {
      float t, h;
      
      t=millis();
      h=42.5;

      postReading(t, h);
      
    }

    delay(LOG_INTERVAL_MS);
}

