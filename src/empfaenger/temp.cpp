#include "temp.h"

static float adc_to_mv(int raw){
  float v = (raw / ADC_MAX) * VREF_ADC;   // Volt
  return v * 1000.0f;                     // mV
}

void temp_begin(){
  pinMode(TEMP_ADC_PIN, INPUT);
  analogReadResolution(ADC_BITS);
  analogReadAveraging(8);
}

float temp_read_mv(){
  long acc=0;
  for(int i=0;i<16;i++){ acc += analogRead(TEMP_ADC_PIN); delayMicroseconds(150); }
  int raw = acc / 16;
  return adc_to_mv(raw);
}

float temp_read_c(){
  float mv = temp_read_mv();
  if (TEMP_USE_TMP36){
    return (mv - TEMP_MV_OFFSET) / TEMP_MV_PER_C;
  } else {
    // Placeholder für NTC: aktuell nur skaliert (kein echter °C)
    // -> später mit Rref/Beta-Parametern ersetzen
    return (mv - 500.0f) / 10.0f;
  }
}
