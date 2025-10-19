#pragma once
#include <Arduino.h>

void display_begin();
void display_show_boot(const char* msg);

// page: 0=BOX, 1=BAT, 2=COUNT, 3=TEMP/HEAT
void display_show_page(uint8_t page,
                       uint8_t boxChannel,
                       int batPct, float vbat,
                       bool armed,
                       int connected, int maxZ,
                       float tempC, bool heatOn);
