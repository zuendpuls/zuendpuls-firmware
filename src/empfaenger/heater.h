#pragma once
#include <Arduino.h>
#include "config.h"

void heater_begin();                    // setzt Default (siehe Config)
void heater_set_kill(bool kill);        // true = Heizung AUS
bool heater_is_killed();

void heater_reset_preheat_timer();      // beim Boot/SAFE resetten
void heater_manage(bool armed, float tempC, float vbat); // Auto-Logik
