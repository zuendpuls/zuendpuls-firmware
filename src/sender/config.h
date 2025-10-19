#pragma once
#include <Arduino.h>

// ====== PINS laut Deiner Liste ======
// Funkmodul (E32-433T33S)
constexpr uint8_t PIN_RADIO_RX   = 0;   // UART RX (Teensy)
constexpr uint8_t PIN_RADIO_TX   = 1;   // UART TX
constexpr uint8_t PIN_RADIO_AUX  = 2;
constexpr uint8_t PIN_RADIO_M0   = 3;
constexpr uint8_t PIN_RADIO_M1   = 4;

// Buzzer
constexpr uint8_t PIN_BUZZER     = 6;

// OLED (SSD1309 SPI)
constexpr uint8_t PIN_OLED_RES   = 7;
constexpr uint8_t PIN_OLED_DC    = 8;
constexpr uint8_t PIN_OLED_CS    = 9;
constexpr uint8_t PIN_OLED_MOSI  = 11;
constexpr uint8_t PIN_OLED_SCK   = 13;

// LED Ringe / LEDs
constexpr uint8_t PIN_LED_RING_SW     = 10; // Ein/Aus-Schalter LED-Ring
constexpr uint8_t PIN_LED_SCHARF      = 21; // "Armed" LED
constexpr uint8_t PIN_LED_UP          = 27;
constexpr uint8_t PIN_LED_ENTER       = 28;
constexpr uint8_t PIN_LED_DOWN        = 29;
constexpr uint8_t PIN_LED_RES1        = 30;
constexpr uint8_t PIN_LED_RES2        = 31;
constexpr uint8_t PIN_LED_RES3        = 32;
constexpr uint8_t PIN_LED_RES4        = 33;
constexpr uint8_t PIN_LED_RES5        = 34;
constexpr uint8_t PIN_LED_START       = 41;

// Taster
constexpr uint8_t PIN_BTN_SCHARF      = 22; // Schlüsselschalter (digital gelesen)
constexpr uint8_t PIN_BTN_UP          = 24;
constexpr uint8_t PIN_BTN_ENTER       = 25;
constexpr uint8_t PIN_BTN_DOWN        = 26;
constexpr uint8_t PIN_BTN_RES1        = 35;
constexpr uint8_t PIN_BTN_RES2        = 36;
constexpr uint8_t PIN_BTN_RES3        = 37;
constexpr uint8_t PIN_BTN_RES4        = 38;
constexpr uint8_t PIN_BTN_RES5        = 39;
constexpr uint8_t PIN_BTN_START       = 40;

// Messungen / Aktoren
constexpr uint8_t PIN_ADC_BATT        = 14; // A0
constexpr uint8_t PIN_ADC_HEATER      = 15; // ADC für Heizungssensor
constexpr uint8_t PIN_HEATER_EN       = 16; // Heizung On/Off

// RTC (I2C)
constexpr uint8_t PIN_RTC_SDA         = 18;
constexpr uint8_t PIN_RTC_SCL         = 19;

// ====== Konstanten ======
constexpr float    ADC_VREF           = 3.3f;
constexpr uint16_t ADC_BITS           = 12;      // Teensy kann 12 Bit sauber
constexpr float    ADC_COUNTS         = (1 << ADC_BITS) - 1; // 4095

// Batterie-Teiler: 33k (R1 oben) + 10k (R2 unten) -> Faktor = (R1+R2)/R2
constexpr float    BATT_R1_KOHM       = 33.0f;
constexpr float    BATT_R2_KOHM       = 10.0f;
constexpr float    BATT_DIV_FACTOR    = (BATT_R1_KOHM + BATT_R2_KOHM) / BATT_R2_KOHM; // 4.3

// ====== LiFePO4 (4S) Grenzwerte ======
// Voll ~14.4–14.6 V (unter Last meist ~13.6–14.0 V), sehr flache Kurve.
// Warnen ab ~12.4 V (≈ ~20–30%), kritisch ab ~11.6 V (~3.0 V/Zelle).
constexpr float    BATT_WARN_V        = 12.4f;
constexpr float    BATT_CRIT_V        = 11.6f;

// Kalibrierung (optional):
// Wenn Multimeter z.B. 12.80 V zeigt, Code aber 12.60 V:
// -> BATT_CAL_SCALE = 12.80/12.60 = 1.0159
constexpr float    BATT_CAL_SCALE     = 1.000f;  // Multiplikation
constexpr float    BATT_CAL_OFFSET    = 0.00f;   // Addition (in Volt)

enum UiPage : uint8_t { UI_BOX = 0, UI_BATT = 1, UI_COUNT = 2, UI_MAX = 3 };

// kleine Helfer
template<typename T, size_t N>
constexpr size_t array_len(const T (&)[N]) { return N; }
