#include "fire.h"
#include "mapping.h"
#include "leds.h"

static uint8_t fireMaskBoard[BOARDS] = {0,0,0,0};

void fire_begin(){
  for(int b=0;b<BOARDS;b++){ fireMaskBoard[b]=0; io_write_board(b, 0x00, 0x00); }
}

void fire_clear_all(){
  for(int b=0;b<BOARDS;b++){ fireMaskBoard[b]=0; io_write_board(b, 0x00, 0x00); }
}

void fire_pulse_ch(uint8_t ch, uint32_t ms, bool show_led){
  if (ch >= TOTAL_CH) return;
  int b = ch / CH_PER;
  int k = ch % CH_PER;
  uint8_t fireBit = (uint8_t)(1u<<k);
  uint8_t ledBit  = (uint8_t)(1u<<LED_MAP[b][k]);

  // Setzen
  fireMaskBoard[b] |= fireBit;
  io_write_board(b, fireMaskBoard[b], show_led ? ledBit : 0x00);

  delay(ms); // Pulsdauer

  // Rücksetzen
  fireMaskBoard[b] &= ~fireBit;
  io_write_board(b, fireMaskBoard[b], 0x00);
}

void fire_pulse_zk(uint8_t zk, uint32_t ms, bool show_led){
  if (zk < 1 || zk > 32) return;
  uint8_t ch = CH_FROM_ZK[zk-1];
  fire_pulse_ch(ch, ms, show_led);
}
