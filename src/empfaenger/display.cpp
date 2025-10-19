#include "display.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static const int SCREEN_WIDTH  = 128;
static const int SCREEN_HEIGHT = 64;
static const uint8_t I2C_ADDR  = 0x3C;

static Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

static int tW(const char* s, uint8_t sz){ int n=0; while(*s){n++; s++;} return n*6*sz; }
static int tH(uint8_t sz){ return 8*sz; }
static void printCentered(const char* s, int y, uint8_t sz){
  int x = (SCREEN_WIDTH - tW(s,sz))/2; if (x<0) x=0;
  oled.setCursor(x,y); oled.setTextSize(sz); oled.print(s);
}

void display_begin(){
  Wire.begin(); Wire.setClock(400000);
  oled.begin(SSD1306_SWITCHCAPVCC, I2C_ADDR);
  oled.clearDisplay(); oled.setTextColor(SSD1306_WHITE); oled.display();
}

void display_show_boot(const char* msg){
  oled.clearDisplay();
  oled.fillRect(0,0,SCREEN_WIDTH,16, SSD1306_WHITE);
  oled.setTextColor(SSD1306_BLACK);
  printCentered("Zundsystem", 4, 1);
  oled.setTextColor(SSD1306_WHITE);
  printCentered("Selftest...", 26, 2);
  if (msg && *msg) printCentered(msg, 50, 1);
  oled.display();
}

static void badge(bool armed){
  const char* s = armed ? "ARMED" : "SAFE";
  int w = 6 * (armed?5:4) + 8;
  oled.drawRect(0,0,w,14, SSD1306_WHITE);
  oled.setCursor(4,3); oled.setTextSize(1); oled.print(s);
}

void display_show_page(uint8_t page,
                       uint8_t boxChannel,
                       int batPct, float vbat,
                       bool armed,
                       int connected, int maxZ,
                       float tempC, bool heatOn){
  if (batPct<0) batPct=0; if (batPct>100) batPct=100;
  if (connected<0) connected=0; if (connected>maxZ) connected=maxZ;

  oled.clearDisplay();
  badge(armed);

  if (page == 0){
    // BOX
    char buf[16];
    snprintf(buf,sizeof(buf),"BOX %d", boxChannel);
    printCentered(buf, 22, 3);

  } else if (page == 1){
    // BAT
    char pbuf[16]; snprintf(pbuf,sizeof(pbuf), "%d%%", batPct);
    printCentered(pbuf, 18, 3);
    char vbuf[16]; snprintf(vbuf,sizeof(vbuf), "%.1fV", vbat);
    printCentered(vbuf, SCREEN_HEIGHT - tH(2) - 2, 2);

  } else if (page == 2){
    // COUNT
    char cbuf[24]; snprintf(cbuf,sizeof(cbuf), "ZUNDER %d/%d", connected, maxZ);
    printCentered(cbuf, 22, 2);

  } else {
    // TEMP / HEAT
    char tbuf[16]; snprintf(tbuf,sizeof(tbuf), "%.0f\xB0""C", tempC); // große °C
    printCentered(tbuf, 14, 3);
    char hbuf[16]; snprintf(hbuf,sizeof(hbuf), "HEAT:%s", heatOn ? "ON" : "OFF");
    printCentered(hbuf, SCREEN_HEIGHT - tH(2) - 2, 2);
  }

  oled.display();
}
