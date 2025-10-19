#pragma once
#include <Arduino.h>
#include "config.h"

void  temp_begin();
float temp_read_c();        // °C (bei TMP36) bzw. pseudo °C (sonst siehe Kommentar)
float temp_read_mv();       // mV am ADC-Knoten
