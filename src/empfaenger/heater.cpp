#include "heater.h"

static bool     s_killed = false;
static uint32_t s_preheat_t0 = 0;

static void drive_pin(bool kill){
  bool level = HEATER_KILL_ACTIVE_HIGH ? kill : !kill;
  digitalWrite(HEATER_KILL_PIN, level);
}

void heater_begin(){
  pinMode(HEATER_KILL_PIN, OUTPUT);
  // Default-Zustand beim Einschalten:
  if (HEATER_DEFAULT_ON_AT_BOOT){
    s_killed = false;         // Heizen ERLAUBT
  } else {
    s_killed = true;          // Heizen AUS
  }
  drive_pin(s_killed);
  s_preheat_t0 = millis();
}

void heater_reset_preheat_timer(){
  s_preheat_t0 = millis();
}

void heater_set_kill(bool kill){
  s_killed = kill;
  drive_pin(kill);
}

bool heater_is_killed(){ return s_killed; }

void heater_manage(bool armed, float tempC, float vbat){
  // 1) Bei ARMED grundsätzlich AUS (so gewünscht)
  if (HEATER_FORCE_OFF_WHEN_ARMED && armed){
    if (!s_killed) heater_set_kill(true);
    return;
  }

  // 2) Akku-Schutz
  if (vbat <= HEAT_MIN_V_RUN){
    if (!s_killed) heater_set_kill(true);   // Spannung zu niedrig -> AUS
    return;
  }

  // 3) Max. Vorwärmzeit (nur relevant wenn erlaubt)
  if (HEAT_MAX_PREHEAT_MS > 0 && (millis() - s_preheat_t0) > HEAT_MAX_PREHEAT_MS){
    if (!s_killed) heater_set_kill(true);   // Timeout -> AUS
    return;
  }

  // 4) Temperatur-Hysterese
  if (s_killed){
    // aktuell AUS -> darf wieder an?
    if ( (vbat >= HEAT_RESUME_V) && (tempC <= TEMP_HEAT_ON_C) ){
      heater_set_kill(false);               // wieder ERLAUBT (heizt)
    }
  } else {
    // aktuell EIN -> abschalten, wenn warm genug
    if (tempC >= TEMP_HEAT_OFF_C){
      heater_set_kill(true);
    }
  }
}
