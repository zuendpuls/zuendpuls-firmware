#pragma once
#include <Arduino.h>
#include "config.h"

void sense_begin();
void sense_scan_once();                 // aktualisiert LEDs (Latch) und loggt Änderungen
void sense_stop(bool clear_leds);       // Messung „aus“ + LEDs optional löschen
void sense_enable();                    // Messung wieder einschalten
bool sense_is_enabled();
int  sense_count_connected();           // <-- NEU: Anzahl erkannter Kanäle
