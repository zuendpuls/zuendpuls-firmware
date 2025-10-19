#include <Arduino.h>
#include <cstring>          // für strlen
#include "config.h"
#include "buttons.h"
#include "leds.h"
#include "buzzer.h"
#include "display.h"
#include "battery.h"
#include "heater.h"
#include "radio.h"
#include "rtc.h"

// ===================== BUTTONS =====================
Button btnScharf   { PIN_BTN_SCHARF, true, true, 10, false, false, 0 };
Button btnUp       { PIN_BTN_UP,     true, true, 10, false, false, 0 };
Button btnEnter    { PIN_BTN_ENTER,  true, true, 10, false, false, 0 };
Button btnDown     { PIN_BTN_DOWN,   true, true, 10, false, false, 0 };
Button btnRes[5] = {
  { PIN_BTN_RES1, true, true, 10, false, false, 0 },
  { PIN_BTN_RES2, true, true, 10, false, false, 0 },
  { PIN_BTN_RES3, true, true, 10, false, false, 0 },
  { PIN_BTN_RES4, true, true, 10, false, false, 0 },
  { PIN_BTN_RES5, true, true, 10, false, false, 0 }
};
Button btnStart    { PIN_BTN_START,  true, true, 10, false, false, 0 };

// ===================== LEDs / DISPLAY =====================
LedSet  leds;
Display dsp;

// ===================== RTC / RADIO STATE =====================
RTCX::RtcTime g_rtc;
char g_timeStr[9] = "??:??:??";   // HH:MM:SS
uint32_t g_rtcMs  = 0;

bool     g_radioOk      = false;
uint8_t  g_radioErrCode = 0;      // 0=OK, 1=Version, 2=Params, 3=TX

// ===================== UI STATE =====================
volatile bool g_armed = false;
UiPage   g_page    = UI_BOX;
String   g_lastKey = "none";
uint32_t g_uiTick  = 0;

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);
  delay(50);

  // Mode-Pins sicher setzen: SLEEP/Config (M1=1, M0=1)
  pinMode(PIN_RADIO_M0, OUTPUT);
  pinMode(PIN_RADIO_M1, OUTPUT);
  digitalWrite(PIN_RADIO_M0, HIGH);
  digitalWrite(PIN_RADIO_M1, HIGH);
  delay(20);

  // Subsysteme
  Buzz::begin();
  leds.begin();
  Battery::begin();
  Heater::begin();
  RTCX::begin();

  // Radio-Basis
  Radio::begin(9600);

  // Selbsttest mit Fehlercode
  Radio::SelfTestResult st = Radio::selfTest(700, 700, 1000);
  g_radioErrCode = static_cast<uint8_t>(st);
  g_radioOk = (st == Radio::ST_OK);

  Serial.printf("Radio selftest: %s (code=%u)\n", g_radioOk ? "OK" : "ERR", g_radioErrCode);

  // Buttons & Display
  btnScharf.begin();
  btnUp.begin(); btnEnter.begin(); btnDown.begin();
  for (auto &b : btnRes) b.begin();
  btnStart.begin();

  dsp.begin();
  dsp.splash("Sender init...");
  Buzz::triple();

  leds.allOff();
  g_armed = false;

  // Uhr ggf. einmalig stellen (nur kurz aktivieren, flashen, dann wieder aus)
  // RTCX::set(2025, 10, 10, 13, 53, 0);
}

// ===================== UI / INPUT =====================
static void handleUi(){
  // Seitenwechsel
  if (btnUp.fell(btnUp.update()))     { g_page = (UiPage)((g_page + UI_MAX - 1) % UI_MAX); g_lastKey = "Up";   Buzz::beep(40); }
  if (btnDown.fell(btnDown.update())) { g_page = (UiPage)((g_page + 1) % UI_MAX);         g_lastKey = "Down"; Buzz::beep(40); }
  if (btnEnter.fell(btnEnter.update())){
    g_lastKey = "Enter";
    Heater::set(!Heater::get());
    Buzz::beep(60);
  }

  // Reserve-Taster LED spiegeln
  for (int i=0;i<5;i++){
    bool edge = btnRes[i].update();
    if (btnRes[i].fell(edge)){
      g_lastKey = String("Res") + String(i+1);
      leds.ledRes[i].toggle();
      Buzz::beep(30);
    }
  }

  // Start/Pause
  if (btnStart.fell(btnStart.update())){
    g_lastKey = "Start";
    leds.ledStart.toggle();
    Buzz::beep(100);
  }

  // Schlüsselschalter (Scharf)
  bool edgeArm = btnScharf.update();
  if (btnScharf.fell(edgeArm)){
    g_armed = true;
    leds.ledScharf.on();
    Buzz::triple();
  } else if (btnScharf.rose(edgeArm)){
    g_armed = false;
    leds.ledScharf.off();
    Buzz::beep(200);
  }
}

// ===================== RTC-STRING =====================
static void updateRtcString(){
  if (RTCX::read(g_rtc) && g_rtc.valid){
    snprintf(g_timeStr, sizeof(g_timeStr), "%02u:%02u:%02u", g_rtc.hour, g_rtc.min, g_rtc.sec);
  } else {
    strncpy(g_timeStr, "--:--:--", sizeof(g_timeStr));
  }
}

// ===================== LOOP =====================
void loop() {
  handleUi();

  // Akku
  float vbat = Battery::readVoltage();
  uint8_t pct = Battery::socPercent(vbat);
  const char* battTxt = Battery::levelText(vbat);

  // RTC alle ~200 ms aktualisieren
  if (millis() - g_rtcMs > 200){
    g_rtcMs = millis();
    updateRtcString();
  }

  // LED: Ein/Aus-Ring spiegelt "armed"
  if (g_armed) leds.ledRingSW.on(); else leds.ledRingSW.off();

  // Display refresh ~100 ms
  if (millis() - g_uiTick > 100){
    g_uiTick = millis();
    dsp.drawStatus(g_page, vbat, pct, battTxt, g_lastKey.c_str(),
                   g_armed, Heater::get(), g_timeStr, g_radioOk, g_radioErrCode);
  }

  delay(1);
}
