#include "mux.h"

static inline void muxAddr(uint8_t ch){
  digitalWrite(PIN_MUX_A0, ch & 1);
  digitalWrite(PIN_MUX_A1,(ch>>1)&1);
  digitalWrite(PIN_MUX_A2,(ch>>2)&1);
}

void mux_begin(){
  pinMode(PIN_MUX_A0,OUTPUT); pinMode(PIN_MUX_A1,OUTPUT); pinMode(PIN_MUX_A2,OUTPUT);
  for(int b=0;b<BOARDS;b++){ pinMode(PIN_MUX_E[b],OUTPUT); digitalWrite(PIN_MUX_E[b],HIGH); }
}
void mux_disable_all(){ for(int i=0;i<BOARDS;i++) digitalWrite(PIN_MUX_E[i],HIGH); }
void mux_select(int board, int ch){
  mux_disable_all();
  digitalWrite(PIN_MUX_E[board],LOW);
  delay(1);
  muxAddr(ch);
  delay(8);
}
