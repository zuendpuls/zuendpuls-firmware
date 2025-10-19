#pragma once
#include <Arduino.h>
#include "config.h"

void  battery_begin();
float battery_read_voltage();          // Vbat in Volt
int   battery_percent();               // 0..100
bool  battery_low();                   // Warnung aktiv (mit Hysterese)
