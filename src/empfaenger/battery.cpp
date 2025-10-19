#include "battery.h"

static bool s_low = false;

void battery_begin(){
  pinMode(BAT_ADC_PIN, INPUT);
  analogReadResolution(ADC_BITS);
  analogReadAveraging(8);
}

static float adc_to_vbat(int raw){
  float v_adc = (raw / ADC_MAX) * VREF_ADC;
  float div_gain = (BAT_R_TOP + BAT_R_BOT) / BAT_R_BOT;
  return v_adc * div_gain;
}

float battery_read_voltage(){
  long acc=0;
  for(int i=0;i<16;i++){ acc += analogRead(BAT_ADC_PIN); delayMicroseconds(150); }
  int raw = acc / 16;
  return adc_to_vbat(raw);
}

int battery_percent(){
  float v = battery_read_voltage();
  if (v <= BAT_V_MIN) return 0;
  if (v >= BAT_V_MAX) return 100;
  return int( (v - BAT_V_MIN) * 100.0f / (BAT_V_MAX - BAT_V_MIN) + 0.5f );
}

bool battery_low(){
  float v = battery_read_voltage();
  if (!s_low && v <= BAT_V_WARN) s_low = true;
  else if (s_low && v >= BAT_V_CLEAR) s_low = false;
  return s_low;
}
