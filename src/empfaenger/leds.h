#pragma once
#include <Arduino.h>
#include "config.h"

void leds_begin();
void leds_write_board(int b, uint8_t ledByte); // FIRE bleibt 0x00
void leds_all_off();
void leds_selftest(uint32_t duration_ms);
void leds_tristate_outputs(bool tristate);

// NEU: gleichzeitiges Schreiben von FIRE & LED für ein Board
void io_write_board(int b, uint8_t fireMask, uint8_t ledMask);
