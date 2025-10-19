#include "mapping.h"

// Zündkanal (1..32) -> physischer CH (0..31)
const uint8_t CH_FROM_ZK[32] = {
   7,6,5,4,3,2,1,0,  15,14,13,12,11,10,9,8,
  23,22,21,20,19,18,17,16,  31,30,29,28,27,26,25,24
};
// physischer CH (0..31) -> Zündkanal (1..32)
const uint8_t ZK_FROM_CH[32] = {
   8,7,6,5,4,3,2,1,  16,15,14,13,12,11,10,9,
  24,23,22,21,20,19,18,17,  32,31,30,29,28,27,26,25
};

// LED-Mapping (lokaler Kanal k -> LED-Bit) – gespiegelt auf allen Boards
const uint8_t LED_MAP[4][8] = {
  {7,6,5,4,3,2,1,0},  // Board 0
  {7,6,5,4,3,2,1,0},  // Board 1
  {7,6,5,4,3,2,1,0},  // Board 2
  {7,6,5,4,3,2,1,0}   // Board 3
};
