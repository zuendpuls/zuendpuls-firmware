#pragma once
#include <Arduino.h>

// ===== LED-Ketten-Layout =====
constexpr bool LED_FIRST      = false; // false = [FIRE][LED] (dein Board)
constexpr bool LED_ACTIVE_LOW = false;  // LED leuchtet bei 0

// ===== ADC / Physik =====
constexpr int   ADC_BITS    = 12;
constexpr float VREF_ADC    = 3.3f;
constexpr float DIV_GAIN    = 1.879f;               // Vy = Vadc * DIV_GAIN
constexpr float TH_ZENER_VY = 4.3f;                 // Clamp-Erkennung
constexpr float ADC_MAX     = float((1 << ADC_BITS) - 1);

// ===== Erkennung (5V-Messmodus) =====
constexpr int RAW_ON  = 700;                        // Ein-Schwelle
constexpr int RAW_OFF = 200;                        // Aus-Schwelle

// ===== Pin-Map =====
struct Bus { int ser, clk, rclk; };

constexpr int BOARDS = 4;
constexpr int CH_PER = 8;
constexpr int TOTAL  = BOARDS * CH_PER;
constexpr int TOTAL_CH = TOTAL;                     // Alias

// Shiftregister-Busse (pro Board)
constexpr Bus SR[BOARDS] = {
  {32,33,34},  // Board 0 -> ZK 1..8
  {35,36,37},  // Board 1 -> ZK 9..16
  {38,39,40},  // Board 2 -> ZK 17..24
  {15,16,17}   // Board 3 -> ZK 25..32
};
constexpr int SR_OE_PIN = 12;                       // 74HC595 OE# (aktiv-LOW, gemeinsam)
constexpr int PIN_BTS   = 20;                       // 24V Enable (LOW = AUS)

// MUX
constexpr int PIN_MUX_A0 = 21, PIN_MUX_A1 = 22, PIN_MUX_A2 = 23;
constexpr int PIN_MUX_E[BOARDS] = {28,29,30,31};    // LOW = aktiv je Board
constexpr int PIN_MUX_Z[BOARDS] = {24,25,26,27};    // ADC-Eingänge je Board

// ===== Soft-Shift (ohne Serien-R: Kanten entschärfen) =====
constexpr uint8_t SHIFT_US       = 2;
constexpr uint8_t LATCH_GUARD_US = 5;

// ===== Betriebs-Optionen =====
constexpr uint32_t SELFTEST_MS          = 2500;     // Dauer LED-Selftest beim Boot
constexpr uint32_t SENSE_TIMEOUT_MS     = 180000;   // 3 min Messzeit
constexpr bool     LED_KEEP_AFTER_SENSE = true;     // true: LED-Latch bleibt an

// ===== ARMED/SAFE Umschaltung =====
// Optionaler Hardware-Eingang für "scharf":
// -1 = deaktiviert (nur Software-Umschaltung), sonst Pin-Nummer eintragen.
// Aktiv-Logik: HIGH = scharf (bei interner PULLUP typ. Taster nach GND -> active-low ändern)
constexpr int  PIN_ARM          = -1;     // z.B. 14 wenn du einen Schalter am Teensy nutzt
constexpr bool ARM_ACTIVE_HIGH  = true;   // true: HIGH = scharf, false: LOW = scharf
constexpr bool LED_KEEP_WHEN_ARMED = false;// wenn scharf -> Messung AUS; LEDs anlassen?


// ===== Auto-Fire-Demo (Trockenlauf) =====
constexpr bool     AUTO_FIRE_DEMO       = false;   // Autostart-Demo aktiv
constexpr uint32_t START_ARM_DELAY_MS   = 3000;   // 3 s bis ARMED
constexpr uint32_t FIRE_PULSE_MS        = 1000;   // Pulsdauer je Kanal
constexpr uint32_t FIRE_GAP_MS          = 400;    // Pause zwischen Kanälen

// ===== Buzzer / Batterie / Funk =====
constexpr int  BUZZER_PIN     = 6;

// Batterie-ADC (Teensy A0 = Pin 14)
constexpr int   BAT_ADC_PIN   = 14;
// Teiler (ANPASSEN!): Vbat -- R_TOP --(ADC)-- R_BOT -- GND
constexpr float BAT_R_TOP  = 100000.0f; // 100k
constexpr float BAT_R_BOT  = 12000.0f;  // 12k

// LiFePO₄ 8s
constexpr float BAT_V_MIN  = 22.0f;     // leer (unter Last)
constexpr float BAT_V_MAX  = 29.2f;     // voll
constexpr float BAT_V_WARN = 24.0f;     // Warnung
constexpr float BAT_V_CLEAR= 24.6f;     // Hysterese



// Funk-Pins (E32/LoRa-Style: M0/M1/AUX + UART)
constexpr int  RADIO_RX_PIN   = 0;   // Teensy RX1
constexpr int  RADIO_TX_PIN   = 1;   // Teensy TX1
constexpr int  RADIO_AUX_PIN  = 2;   // AUX (Input)
constexpr int  RADIO_M0_PIN   = 3;   // M0  (Output)
constexpr int  RADIO_M1_PIN   = 4;   // M1  (Output)
constexpr long RADIO_BAUD     = 9600;

// Box-Kanal/ID (nur Anzeige erstmal)
constexpr uint8_t BOX_CHANNEL = 1;   // <- deine Box-ID/Ansprechkanal eintragen


// ===== Deep-Voltage Schutz fürs ARMED (hast du schon) =====
constexpr float FIRE_INHIBIT_V = 20.5f;   // darunter kein ARMED

// ===== Temperatur / Heizung (Auto) =====
constexpr int   TEMP_ADC_PIN             = 41;     // ADC für Temperatur
constexpr bool  TEMP_USE_TMP36           = true;
constexpr float TEMP_MV_OFFSET           = 500.0f;
constexpr float TEMP_MV_PER_C            = 10.0f;

// Heizen: Hysterese (z.B. unter 5°C ein, über 15°C aus)
constexpr float TEMP_HEAT_ON_C           = 5.0f;
constexpr float TEMP_HEAT_OFF_C          = 15.0f;

// Akku-Wächter fürs Heizen (nicht unter X Volt leer saugen)
constexpr float HEAT_MIN_V_RUN           = 23.6f;  // darunter Heizen AUS
constexpr float HEAT_RESUME_V            = 24.2f;  // erst ab hier wieder erlauben

// Safety: max. Vorwärmzeit
constexpr uint32_t HEAT_MAX_PREHEAT_MS   = 15UL * 60UL * 1000UL; // 15 min

// Verhalten
constexpr bool HEATER_AUTOKILL_EN        = true;   // Auto-Logik aktiv
constexpr int  HEATER_KILL_PIN           = 11;     // Kill-Leitung zur Heizung
constexpr bool HEATER_KILL_ACTIVE_HIGH   = true;   // HIGH = Heizung AUS

// WICHTIG: Default beim Einschalten (falls Teensy tot/kalt): Heizung EIN
constexpr bool HEATER_DEFAULT_ON_AT_BOOT = true;   // default: heizen erlaubt
constexpr bool HEATER_FORCE_OFF_WHEN_ARMED = true; // bei ARMED immer Heizung AUS
