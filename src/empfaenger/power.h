#pragma once
#include <Arduino.h>
#include "config.h"
#include "mux.h"    // für optionalen Check

void power_begin();                 // setzt PIN_BTS sicher LOW
void power_set_24v(bool enable);    // true = 24V an (PIN_BTS=HIGH), false = 24V aus
bool power_is_24v();
