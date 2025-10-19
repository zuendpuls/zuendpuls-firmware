#pragma once
#include <Arduino.h>
#include <U8g2lib.h>
#include "config.h"

class Display {
public:
  Display();
  void begin();
  void splash(const char* line = "Display OK");

  // Achtung: Signatur inkl. radioOk & radioErrCode!
  void drawStatus(UiPage page,
                  float battV, uint8_t battPct, const char* battTxt,
                  const char* lastKey, bool armed, bool heaterOn,
                  const char* timeHHMMSS, bool radioOk, uint8_t radioErrCode);

private:
  U8G2* u8g2; // wird in begin() alloziert
};
