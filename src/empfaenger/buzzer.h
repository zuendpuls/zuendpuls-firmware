#pragma once
#include <Arduino.h>
#include "config.h"

void buzzer_begin();
void buzzer_beep(uint16_t ms);
void buzzer_pattern_armed();      // kurzes Muster bei ARMED
void buzzer_pattern_safe();       // kurzes Muster bei SAFE
void buzzer_tick_lowbat();        // kurzer Tick bei LowBatt
