#include "radio.h"

void radio_begin(){
  pinMode(RADIO_M0_PIN, OUTPUT);
  pinMode(RADIO_M1_PIN, OUTPUT);
  pinMode(RADIO_AUX_PIN, INPUT);    // ggf. INPUT_PULLUP falls offen
  radio_set_normal_mode();
  // UART1 startet – wenn Modul stromlos ist, stört das nicht
  Serial1.begin(RADIO_BAUD);
}

void radio_set_normal_mode(){
  digitalWrite(RADIO_M0_PIN, LOW);
  digitalWrite(RADIO_M1_PIN, LOW);
  delay(5);
}

bool radio_ready(){
  int lvl = digitalRead(RADIO_AUX_PIN);
  return (lvl != 0); // HIGH=bereit (je nach Modul)
}
