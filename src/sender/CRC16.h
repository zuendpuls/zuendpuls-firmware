#pragma once
#include <Arduino.h>
uint16_t crc16_ccitt(const uint8_t* data, size_t len); // init 0xFFFF, poly 0x1021
