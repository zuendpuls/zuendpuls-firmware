#pragma once
#include <Arduino.h>
#include "config.h"

namespace Heater {
  inline void begin(){
    pinMode(PIN_HEATER_EN, OUTPUT);
    digitalWrite(PIN_HEATER_EN, LOW);
    pinMode(PIN_ADC_HEATER, INPUT_DISABLE);
  }
  inline void set(bool on){ digitalWrite(PIN_HEATER_EN, on ? HIGH : LOW); }
  inline bool get(){ return digitalRead(PIN_HEATER_EN); }

  inline uint16_t readRaw(){ return analogRead(PIN_ADC_HEATER); } // später skalieren
}
