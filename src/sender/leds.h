#pragma once
#include <Arduino.h>
#include "config.h"

struct Led {
  uint8_t pin;
  void begin() { pinMode(pin, OUTPUT); off(); }
  void on()    { digitalWrite(pin, HIGH); }
  void off()   { digitalWrite(pin, LOW); }
  void toggle(){ digitalWrite(pin, !digitalRead(pin)); }
};

struct LedSet {
  Led ledRingSW, ledScharf, ledUp, ledEnter, ledDown, ledRes[5], ledStart;

  void begin() {
    ledRingSW.pin = PIN_LED_RING_SW; ledRingSW.begin();
    ledScharf.pin = PIN_LED_SCHARF;  ledScharf.begin();
    ledUp.pin     = PIN_LED_UP;      ledUp.begin();
    ledEnter.pin  = PIN_LED_ENTER;   ledEnter.begin();
    ledDown.pin   = PIN_LED_DOWN;    ledDown.begin();
    uint8_t resPins[5] = {PIN_LED_RES1, PIN_LED_RES2, PIN_LED_RES3, PIN_LED_RES4, PIN_LED_RES5};
    for (int i=0;i<5;i++){ ledRes[i].pin = resPins[i]; ledRes[i].begin(); }
    ledStart.pin  = PIN_LED_START;   ledStart.begin();
  }

  void allOff(){
    ledRingSW.off(); ledScharf.off(); ledUp.off(); ledEnter.off(); ledDown.off();
    for (auto &l : ledRes) l.off();
    ledStart.off();
  }
};
