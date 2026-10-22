#include <TFT_eSPI.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>

#define TOUCH_CS   33
#define TOUCH_IRQ  36
#define TOUCH_CLK  25
#define TOUCH_MISO 39
#define TOUCH_MOSI 32

#define TFT_RGB_ORDER TFT_BGR

TFT_eSPI tft;
SPIClass touchSPI(VSPI);
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.invertDisplay(true);
  tft.setRotation(1);          // landscape, 320x240

  touchSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
  ts.begin(touchSPI);
  ts.setRotation(1);

  uint16_t colors[] = {TFT_RED, TFT_GREEN, TFT_BLUE};
  const char* names[] = {"RED", "GREEN", "BLUE"};
  for (int i = 0; i < 3; i++) {
    tft.fillScreen(colors[i]);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString(names[i], 160, 110, 4);
    delay(800);    
  }
  tft.fillScreen(TFT_BLACK);
  tft.drawCentreString("Touch me", 160, 110, 4);
  
}

void loop() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    // raw values are roughly 200-3800; map to screen pixels
    int x = map(p.x, 200, 3800, 0, 320);
    int y = map(p.y, 200, 3800, 0, 240);
    tft.fillCircle(x, y, 3, TFT_YELLOW);
    Serial.printf("raw %d,%d -> %d,%d\n", p.x, p.y, x, y);
  }
}