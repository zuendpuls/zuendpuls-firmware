#pragma once
#include <Arduino.h>
#include "config.h"

void fire_begin();
void fire_clear_all();                     // alle FIRE=0
void fire_pulse_ch(uint8_t ch, uint32_t ms, bool show_led); // ch=0..31
void fire_pulse_zk(uint8_t zk, uint32_t ms, bool show_led); // zk=1..32
