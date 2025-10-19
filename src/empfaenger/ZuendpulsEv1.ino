#include "config.h"
#include "mapping.h"
#include "leds.h"
#include "mux.h"
#include "sense.h"
#include "power.h"
#include "display.h"
#include "buzzer.h"
#include "radio.h"
#include "temp.h"
#include "heater.h"
#include "battery.h"



// Software-ARM (ohne Taster)
static bool armed = false;
static uint32_t sense_start_ms = 0;
static uint32_t ui_tick_ms = 0;

static uint8_t  ui_page    = 0;      // 0=BOX, 1=BAT, 2=COUNT
static uint32_t ui_page_ms = 0;

static float     g_last_tempC = 0.0f;

static void buzzer_triple(){ buzzer_beep(80); delay(80); buzzer_beep(80); delay(80); buzzer_beep(120); }

static void set_armed(bool make_armed){
  if (make_armed == armed) return;

  if (make_armed){
    // Deep-Voltage Check
    float v = battery_read_voltage();
    if (v < FIRE_INHIBIT_V){
      Serial.print("ARM VERWEIGERT: Akku zu niedrig (");
      Serial.print(v,2); Serial.println(" V).");
      buzzer_triple();
      // Anzeige aktualisieren bleibt SAFE
      return;
    }
    // -> ARMED
    armed = true;
    heater_set_kill(true);     // <--- Heizung sofort AUS
    sense_stop(!LED_KEEP_WHEN_ARMED);
    delay(5);
    power_set_24v(true);
    buzzer_pattern_armed();
    Serial.println("-> ARMED: Sense AUS, 24V EIN, HEATER AUS.");
  } else {
    // -> SAFE
    armed = false;
    power_set_24v(false);
    delay(20);
    leds_tristate_outputs(false);
    sense_enable();
    sense_start_ms = millis();
    buzzer_pattern_safe();
    Serial.println("-> SAFE: 24V AUS, Sense EIN.");
  }
}

static void print_help(){
  Serial.println("\n=== Steuerung ===");
  Serial.println("  a -> ARMED (Sense AUS, 24V EIN)");
  Serial.println("  s -> SAFE  (24V AUS, Sense EIN)");
  Serial.println("  h -> Hilfe");
}

void setup(){
  Serial.begin(115200);

  power_begin();        // 24V AUS
  mux_begin();
  leds_begin();
  display_begin();
  battery_begin();
  buzzer_begin();
  radio_begin();
  temp_begin();
  heater_begin();               // Heizung standardmäßig erlaubt (Default laut Config)
  heater_reset_preheat_timer(); // Vorwärm-Timer starten

  // Wenn du möchtest, nach dem Boot wieder freigeben:
  heater_set_kill(false);  // optional: Heizung frei nach Init

  leds_selftest(SELFTEST_MS);
  display_show_boot("OLED/Batt/RF OK");

  // Start SAFE
  sense_begin();
  sense_start_ms = millis();
  set_armed(false);

  ui_tick_ms = millis();
  Serial.print("Auto-Stop in "); Serial.print(SENSE_TIMEOUT_MS/1000); Serial.println(" s.");
  print_help();
}

void loop(){
  // serielle Umschaltung optional
  while (Serial.available()){
    char c = (char)Serial.read();
    if (c=='a'||c=='A'){ set_armed(true); }
    else if (c=='s'||c=='S'){ set_armed(false); }
    else if (c=='h'||c=='H'){ print_help(); }
  }

  // Sense/Timeout
  if (!armed){
    if (millis() - sense_start_ms < SENSE_TIMEOUT_MS){
      sense_scan_once();
      delay(80);
    } else {
      sense_stop(!LED_KEEP_AFTER_SENSE);
      delay(200);
    }
  } else {
    delay(80);
  }

  // Akku-Warnung (Ticken), max alle 2 s
  static uint32_t lowbat_tick_ms = 0;
  if (battery_low()){
    if (millis() - lowbat_tick_ms > 2000){
      buzzer_tick_lowbat();
      lowbat_tick_ms = millis();
    }
  }

  // Heizer-Automatik (alle ~500 ms)
static uint32_t last_mgr = 0;
if (millis() - last_mgr > 500){
  float v  = battery_read_voltage();
  g_last_tempC = temp_read_c();
  heater_manage(armed, g_last_tempC, v);
  last_mgr = millis();
}

// OLED: Seite alle 2000 ms weiterblättern, Redraw alle 250 ms
uint32_t now = millis();
if (now - ui_page_ms > 2000){
  ui_page = (ui_page + 1) % 4;   // jetzt 4 Seiten: 0..3
  ui_page_ms = now;
}
static uint32_t ui_draw_ms = 0;
if (now - ui_draw_ms > 250){
  int   pct = battery_percent();
  float v   = battery_read_voltage();
  int   cnt = sense_count_connected();
  bool  heatOn = !heater_is_killed();
  display_show_page(ui_page, BOX_CHANNEL, pct, v, armed, cnt, 30, g_last_tempC, heatOn);
  ui_draw_ms = now;
}
}

