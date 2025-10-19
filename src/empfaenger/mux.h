#pragma once
#include <Arduino.h>
#include "config.h"

void mux_begin();
void mux_select(int board, int ch); // aktiviert nur dieses Board/ch
void mux_disable_all();
