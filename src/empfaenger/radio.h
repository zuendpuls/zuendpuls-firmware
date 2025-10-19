#pragma once
#include <Arduino.h>
#include "config.h"

void radio_begin();             // setzt M0/M1, startet UART
bool radio_ready();             // AUX = HIGH?
void radio_set_normal_mode();   // M1=0, M0=0
