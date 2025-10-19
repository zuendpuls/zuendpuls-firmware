#include "display.h"
#include <SPI.h>

Display::Display() : u8g2(nullptr) {}

void Display::begin() {
  // SSD1309 (128x64) SPI, Pins aus config.h
  u8g2 = new U8G2_SSD1309_128X64_NONAME2_F_4W_HW_SPI(
    U8G2_R0,         // Rotation
    PIN_OLED_CS,     // CS
    PIN_OLED_DC,     // DC
    PIN_OLED_RES     // RESET
  );
  u8g2->begin();
}

void Display::splash(const char* line) {
  if (!u8g2) return;
  u8g2->clearBuffer();
  u8g2->setFont(u8g2_font_6x12_tr);
  u8g2->drawStr(0, 14, line);
  u8g2->sendBuffer();
}

void Display::drawStatus(UiPage page,
                         float battV, uint8_t battPct, const char* battTxt,
                         const char* lastKey, bool armed, bool heaterOn,
                         const char* timeHHMMSS, bool radioOk, uint8_t radioErrCode)
{
  if (!u8g2) return;

  u8g2->clearBuffer();
  u8g2->setFont(u8g2_font_6x12_tr);

  // Header: Titel links
  u8g2->drawStr(0, 12, "Sender UI");

  // Header: Uhr rechts (Breite grob 8 Zeichen à 6px = 48px)
  if (timeHHMMSS) {
    const int w = 6 * 8;
    u8g2->drawStr(128 - w, 12, timeHHMMSS);
  }

  // Zeile 2: Page + ARM
  char top[40];
  snprintf(top, sizeof(top), "Page:%u  ARM:%s", (unsigned)page, armed ? "ON" : "OFF");
  u8g2->drawStr(0, 24, top);

  if (page == UI_BOX) {
    if (radioOk) {
      u8g2->drawStr(0, 38, "Box: Radio OK");
    } else {
      char ln[28];
      snprintf(ln, sizeof(ln), "Box: Radio ERR(%u)", radioErrCode);
      u8g2->drawStr(0, 38, ln);
    }
    char ln2[28];
    snprintf(ln2, sizeof(ln2), "Heater:%s", heaterOn ? "ON" : "OFF");
    u8g2->drawStr(0, 50, ln2);

  } else if (page == UI_BATT) {
    // Batterie-Text
    char ln1[48];
    snprintf(ln1, sizeof(ln1), "Batt: %.2f V  %u%%  [%s]", battV, battPct, battTxt ? battTxt : "");
    u8g2->drawStr(0, 38, ln1);

    // Batterie-Balken (100x10)
    const int x = 0, y = 52, w = 100, h = 10;
    u8g2->drawFrame(x, y - h, w, h);
    int fill = (w * battPct) / 100;
    if (fill > 0) {
      int inner = fill - 2;
      if (inner < 0) inner = 0;
      u8g2->drawBox(x + 1, y - h + 1, inner, h - 2);
    }

  } else if (page == UI_COUNT) {
    u8g2->drawStr(0, 38, "Count/Info Seite");
    char ln[48];
    snprintf(ln, sizeof(ln), "Letzter Taster: %s", lastKey ? lastKey : "none");
    u8g2->drawStr(0, 50, ln);
  }

  u8g2->sendBuffer();
}
