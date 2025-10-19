#pragma once
#include <Arduino.h>
#include "config.h"

namespace Battery {

  // LiFePO4 4S grobe LUT (Leerlauf/leichte Last). Sehr flache Mitte -> nur Näherung!
  // {Spannung in V, Prozent}
  static constexpr struct { float v; uint8_t p; } LUT[] = {
    {14.40f,100}, {13.60f, 95}, {13.40f, 90}, {13.30f, 85},
    {13.20f, 80}, {13.10f, 70}, {13.00f, 60}, {12.90f, 50},
    {12.80f, 40}, {12.70f, 30}, {12.60f, 20}, {12.40f, 15},
    {12.20f, 10}, {12.00f,  7}, {11.80f,  5}, {11.60f,  0}
  };

  // einfacher gleitender Mittelwert
  static float v_filtered = 0.0f;
  static const uint8_t avgSamples = 8;

  inline void begin(){
    analogReadResolution(ADC_BITS);
    analogReadAveraging(avgSamples);     // Teensy-Hardware-Averaging
    pinMode(PIN_ADC_BATT, INPUT);        // stabil & portable
  }

  inline float readVoltageRaw(){
    uint16_t raw = analogRead(PIN_ADC_BATT);
    float vin = ( (float)raw / ADC_COUNTS ) * ADC_VREF * BATT_DIV_FACTOR;
    // Kalibrierung anwenden
    vin = vin * BATT_CAL_SCALE + BATT_CAL_OFFSET;
    return vin;
  }

  inline float readVoltage(){
    float v = readVoltageRaw();
    if (v_filtered <= 0.01f) v_filtered = v;               // initialisieren
    v_filtered = v_filtered * 0.7f + v * 0.3f;             // EMA
    return v_filtered;
  }

  inline uint8_t socPercent(float v){
    // über/unter LUT kappen
    if (v >= LUT[0].v) return 100;
    if (v <= LUT[sizeof(LUT)/sizeof(LUT[0]) - 1].v) return 0;
    // Interpolation zwischen Stützstellen
    for (size_t i=0; i< (sizeof(LUT)/sizeof(LUT[0]) - 1); ++i){
      if (v >= LUT[i+1].v && v <= LUT[i].v){
        float x0=LUT[i].v,   y0=LUT[i].p;
        float x1=LUT[i+1].v, y1=LUT[i+1].p;
        float t = (v - x1) / (x0 - x1); // 0..1
        float p = y1 + t * (y0 - y1);
        if (p<0) p=0; if (p>100) p=100;
        return (uint8_t)(p + 0.5f);
      }
    }
    return 0;
  }

  inline const char* levelText(float v){
    if (v <= BATT_CRIT_V) return "LOW!";
    if (v <= BATT_WARN_V) return "Warn";
    return "OK";
  }
}
