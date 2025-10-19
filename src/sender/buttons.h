#pragma once
#include <Arduino.h>

struct Button {
  uint8_t pin;
  bool    pullup;       // INPUT_PULLUP?
  bool    invert;       // LOW = pressed
  uint16_t debounce_ms; // stabiler Zeitraum
  bool    state;        // aktueller stabiler Zustand
  bool    lastState;
  uint32_t lastChange;

  void begin() {
    pinMode(pin, pullup ? INPUT_PULLUP : INPUT);
    bool raw = digitalRead(pin);
    state = lastState = invert ? !raw : raw;
    lastChange = millis();
  }

  // returns true on rising/falling edges
  bool update() {
    bool raw = digitalRead(pin);
    bool logical = invert ? !raw : raw;
    if (logical != lastState) {
      // potenzieller Wechsel -> Timer setzen
      if (millis() - lastChange >= debounce_ms) {
        lastChange = millis();
        lastState = logical;
        state = logical;
        return true; // Edge bestätigt
      }
    } else {
      lastChange = millis(); // stabil
    }
    return false;
  }

  bool pressed() const { return state; }
  bool fell(bool currentEdge) const {
    // pressed edge (von false -> true)
    // das prüfen in der Loop: wenn currentEdge und pressed()==true
    return currentEdge && state == true;
  }
  bool rose(bool currentEdge) const {
    return currentEdge && state == false;
  }
};
