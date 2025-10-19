#include "leds.h"
#include "mapping.h"

static inline void sr_shift_byte(int ser,int clk,uint8_t v){
  for(int i=7;i>=0;--i){
    digitalWrite(ser,(v>>i)&1);
    delayMicroseconds(1);
    digitalWrite(clk,HIGH); delayMicroseconds(SHIFT_US);
    digitalWrite(clk,LOW);  delayMicroseconds(SHIFT_US);
  }
}

void leds_tristate_outputs(bool tristate){
  pinMode(SR_OE_PIN,OUTPUT);
  digitalWrite(SR_OE_PIN, tristate ? HIGH : LOW);
}

// === NEU: FIRE & LED gleichzeitig schreiben ===
void io_write_board(int b, uint8_t fireMask, uint8_t ledMask){
  uint8_t led = LED_ACTIVE_LOW ? (uint8_t)~ledMask : ledMask;
  leds_tristate_outputs(true);                 // OE HIGH
  delayMicroseconds(LATCH_GUARD_US);
  digitalWrite(SR[b].rclk,LOW);
  if (LED_FIRST){                              // [LED][FIRE]
    sr_shift_byte(SR[b].ser,SR[b].clk,led);
    sr_shift_byte(SR[b].ser,SR[b].clk,fireMask);
  } else {                                     // [FIRE][LED]
    sr_shift_byte(SR[b].ser,SR[b].clk,fireMask);
    sr_shift_byte(SR[b].ser,SR[b].clk,led);
  }
  delayMicroseconds(LATCH_GUARD_US);
  digitalWrite(SR[b].rclk,HIGH);
  delayMicroseconds(LATCH_GUARD_US);
  digitalWrite(SR[b].rclk,LOW);
  leds_tristate_outputs(false);                // OE LOW (sichtbar/aktiv)
}

void leds_begin(){
  leds_tristate_outputs(true); // Outputs AUS
  for(int b=0;b<BOARDS;b++){
    pinMode(SR[b].ser,OUTPUT); pinMode(SR[b].clk,OUTPUT); pinMode(SR[b].rclk,OUTPUT);
    digitalWrite(SR[b].ser,LOW); digitalWrite(SR[b].clk,LOW); digitalWrite(SR[b].rclk,LOW);
  }
  for(int b=0;b<BOARDS;b++) leds_write_board(b, 0x00);
}

void leds_write_board(int b, uint8_t ledByte){
  io_write_board(b, /*fire*/0x00, /*led*/ledByte);
}

void leds_all_off(){
  for(int b=0;b<BOARDS;b++) leds_write_board(b, 0x00);
}

// … (Selftest unverändert wie bei dir; benutzt LED_MAP)
void leds_selftest(uint32_t duration_ms){
  const uint32_t t0 = millis();
  // 1) Wipe pro Board vorwärts
  for(int b=0;b<BOARDS && millis()-t0 < duration_ms; ++b){
    for(int k=0;k<8 && millis()-t0 < duration_ms; ++k){
      uint8_t bit = LED_MAP[b][k];
      leds_write_board(b, (uint8_t)(1u<<bit));
      delay(50);
    }
    leds_write_board(b, 0x00);
  }
  // 2) Ping-Pong
  for(int r=0; r<2 && millis()-t0 < duration_ms; ++r){
    for(int step=0; step<8 && millis()-t0 < duration_ms; ++step){
      for(int b=0;b<BOARDS;++b){
        uint8_t bit = LED_MAP[b][step];
        leds_write_board(b, (uint8_t)(1u<<bit));
      }
      delay(60);
    }
    for(int step=6; step>0 && millis()-t0 < duration_ms; --step){
      for(int b=0;b<BOARDS;++b){
        uint8_t bit = LED_MAP[b][step];
        leds_write_board(b, (uint8_t)(1u<<bit));
      }
      delay(60);
    }
  }
  // 3) All-ON blink
  for(int n=0; n<2 && millis()-t0 < duration_ms; ++n){
    for(int b=0;b<BOARDS;++b) leds_write_board(b, 0xFF);
    delay(120);
    for(int b=0;b<BOARDS;++b) leds_write_board(b, 0x00);
    delay(120);
  }
}
