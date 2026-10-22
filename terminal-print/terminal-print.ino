#include <TFT_eSPI.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti WiFiMulti;

class TftTerminal : public Print {
public:
  TftTerminal(TFT_eSPI &tft, uint8_t textSize = 1)
    : _tft(tft), _size(textSize) {}

  void begin(uint16_t fg = TFT_GREEN, uint16_t bg = TFT_BLACK) {
    _fg = fg; _bg = bg;
    _tft.setTextSize(_size);
    _tft.setTextColor(_fg, _bg);
    _charW = 6 * _size;               // default 6x8 font
    _lineH = 8 * _size;
    _cols  = _tft.width()  / _charW;
    _rows  = _tft.height() / _lineH;
    // _rows is capped by buffer size below
    if (_rows > MAX_ROWS) _rows = MAX_ROWS;
    clear();
  }

  void clear() {
    for (uint8_t r = 0; r < MAX_ROWS; r++) _buf[r][0] = '\0';
    _row = 0; _col = 0;
    _tft.fillScreen(_bg);
  }

  size_t write(uint8_t c) override {
    if (c == '\r') return 1;
    if (c == '\n') { newline(); return 1; }
    if (_col >= _cols) newline();     // wrap long lines
    _buf[_row][_col++] = c;
    _buf[_row][_col] = '\0';
    _tft.setTextColor(_fg, _bg);
    _tft.setTextSize(_size);
    _tft.drawChar(c, (_col - 1) * _charW, _row * _lineH, 1);
    return 1;
  }

private:
  static const uint8_t MAX_ROWS = 40;
  static const uint8_t MAX_COLS = 80;

  TFT_eSPI &_tft;
  uint8_t  _size;
  uint16_t _fg = TFT_GREEN, _bg = TFT_BLACK;
  uint8_t  _charW = 6, _lineH = 8, _cols = 0, _rows = 0;
  uint8_t  _row = 0, _col = 0;
  char     _buf[MAX_ROWS][MAX_COLS + 1];

  void newline() {
    _col = 0;
    if (_row < _rows - 1) { _row++; _buf[_row][0] = '\0'; return; }
    // At bottom: scroll the text buffer up and redraw
    for (uint8_t r = 0; r < _rows - 1; r++) strcpy(_buf[r], _buf[r + 1]);
    _buf[_rows - 1][0] = '\0';
    redraw();
  }

  void redraw() {
    _tft.setTextColor(_fg, _bg);
    _tft.setTextSize(_size);
    for (uint8_t r = 0; r < _rows; r++) {
      _tft.fillRect(0, r * _lineH, _tft.width(), _lineH, _bg);
      _tft.setCursor(0, r * _lineH);
      _tft.print(_buf[r]);
    }
  }
};

TFT_eSPI tft;
TftTerminal term(tft, 2);             // text size 2 = 26 cols x 15 rows in landscape

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network
  WiFiMulti.addAP("Gilgamesh", "12345678");


  tft.init();
  tft.setRotation(1);                 // landscape
  pinMode(21, OUTPUT);                // CYD backlight pin
  digitalWrite(21, HIGH);
  term.begin(TFT_GREEN, TFT_BLACK);

  term.println("Boot OK");
  term.printf("Free heap: %u\n", ESP.getFreeHeap());

  term.print("Waiting for WiFi... ");

    while (WiFiMulti.run() != WL_CONNECTED) {
    term.print(".");
    delay(500);

      term.println("");
    term.println("WiFi connected");
    term.println("IP address: ");
    term.println(WiFi.localIP());



  }
}

void loop() {

    const uint16_t port = 8000;
  const char *host = "192.168.4.23";  // ip or dns

  term.print("Connecting to ");
  term.println(host);

    // Use NetworkClient class to create TCP connections
    NetworkClient client;

    if (!client.connect(host, port)) {
    term.println("Connection failed.");
    term.println("Waiting 5 seconds before retrying...");
    delay(5000);
    return;
  }


  // This will send a request to the server
  //uncomment this line to send an arbitrary string to the server
  //client.print("Send this data to the server");
  //uncomment this line to send a basic document request to the server
  client.print("GET /index.html HTTP/1.1\n\n");

  int maxloops = 0;

  //wait for the server's reply to become available
  while (!client.available() && maxloops < 1000) {
    maxloops++;
    delay(1);  //delay 1 msec
  }
  if (client.available() > 0) {
    //read back one line from the server
    String line = client.readStringUntil('\r');
    term.println(line);
  } else {
    term.println("client.available() timed out ");
  }

  term.println("Closing connection.");
  client.stop();

  term.println("Waiting 5 seconds before restarting...");
  delay(5000);
}