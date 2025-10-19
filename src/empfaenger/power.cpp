#include "power.h"

static bool s_is24v = false;

void power_begin(){
  pinMode(PIN_BTS, OUTPUT);
  digitalWrite(PIN_BTS, LOW);   // Start immer 24V AUS
  s_is24v = false;
}

// harte Umschaltung mit kleinen Schutzzeiten
void power_set_24v(bool enable){
  if (enable == s_is24v) return;

  if (enable){
    // 24V EIN
    digitalWrite(PIN_BTS, HIGH);
    delay(5);                   // kleiner Guard
    s_is24v = true;
  } else {
    // 24V AUS
    digitalWrite(PIN_BTS, LOW);
    delay(20);                  // Zeit zum Entladen der Rail
    s_is24v = false;
  }
}

bool power_is_24v(){ return s_is24v; }
