#pragma once
#include <Arduino.h>
#include "config.h"

namespace Buzz {
  inline void begin(){ pinMode(PIN_BUZZER, OUTPUT); digitalWrite(PIN_BUZZER, LOW); }
  inline void beep(uint16_t ms){ digitalWrite(PIN_BUZZER, HIGH); delay(ms); digitalWrite(PIN_BUZZER, LOW); }
  inline void triple(){ beep(80); delay(80); beep(80); delay(80); beep(120); }
}
