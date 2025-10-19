#include "sense.h"
#include "leds.h"
#include "mux.h"
#include "mapping.h"

static bool    connected[TOTAL_CH] = {0};
static uint8_t ledMask[BOARDS]     = {0,0,0,0};
static bool    sensing_enabled     = true;

static inline float rawToVadc(int raw){ return (raw / ADC_MAX) * VREF_ADC; }
static inline float vadcToVy(float v){ return v * DIV_GAIN; }

void sense_begin(){
  analogReadResolution(ADC_BITS);
  analogReadAveraging(8);
  for(int b=0;b<BOARDS;b++){ ledMask[b]=0; leds_write_board(b,0x00); }
  for(int i=0;i<TOTAL_CH;i++) connected[i]=false;
  sensing_enabled = true;
}

void sense_stop(bool clear_leds){
  sensing_enabled = false;
  mux_disable_all();
  if (clear_leds){
    leds_all_off();
    leds_tristate_outputs(true);   // OE HIGH -> Ausgänge AUS (stromsparend)
  }
}

void sense_enable(){
  sensing_enabled = true;          // LEDs/Masken bleiben bestehen
  leds_tristate_outputs(false);    // sichtbar lassen
}


int sense_count_connected(){
  int n=0;
  for(int i=0;i<TOTAL_CH;i++) if (connected[i]) n++;
  return n;
}

bool sense_is_enabled(){ return sensing_enabled; }

void sense_scan_once(){
  if (!sensing_enabled) return;

  for(int ch=0; ch<TOTAL_CH; ++ch){
    int b = ch / CH_PER, k = ch % CH_PER;

    mux_select(b,k);

    // 8x Mittelwert
    int raw = 0;
    for(int i=0;i<8;i++){ raw += analogRead(PIN_MUX_Z[b]); delayMicroseconds(150); }
    raw /= 8;

    mux_disable_all();

    float vy = vadcToVy(rawToVadc(raw));
    bool prev  = connected[ch];
    bool now   = prev;
    bool clamp = (vy >= TH_ZENER_VY);

    if (!clamp){
      if (raw > RAW_ON)       now = true;
      else if (raw < RAW_OFF) now = false;
    } else now = false;

    if (now != prev){
      connected[ch] = now;

      // LED-Mapping: lokaler Kanal k -> LED-Bit auf Board b
      uint8_t ledBit = LED_MAP[b][k];
      if (now)  ledMask[b] |=  (1u << ledBit);
      else      ledMask[b] &= ~(1u << ledBit);

      leds_write_board(b, ledMask[b]);

      uint8_t zk = ZK_FROM_CH[ch];
      Serial.print("ZK"); Serial.print(zk);
      Serial.print(" (CH"); Serial.print(ch); Serial.print(") -> ");
      Serial.print(now ? "DURCHGANG" : "offen");
      Serial.print("  raw="); Serial.print(raw);
      Serial.print("  Vy=");  Serial.print(vy,3); Serial.println("V");
    }
  }
}
