#include "buzzer.h"

void buzzer_begin(){
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

void buzzer_beep(uint16_t ms){
  digitalWrite(BUZZER_PIN, HIGH);
  delay(ms);
  digitalWrite(BUZZER_PIN, LOW);
}

void buzzer_pattern_armed(){
  buzzer_beep(120); delay(100); buzzer_beep(200);
}
void buzzer_pattern_safe(){
  buzzer_beep(80); delay(60); buzzer_beep(80);
}
void buzzer_tick_lowbat(){
  buzzer_beep(30);
}
