#pragma once
#include <Arduino.h>
#include <stdint.h>
#include "config.h"

namespace Radio {

  enum Mode : uint8_t { MODE_NORMAL=0, MODE_WAKEUP=1, MODE_POWER_SAVING=2, MODE_SLEEP=3 };

  struct Version {
    bool ok{false};
    uint8_t ver{0};    // z.B. 0x32
    uint8_t module{0}; // 0x46/0x47 etc.
    uint8_t freq{0};   // 0x32=433, 0x64=868, 0xC8=915
  };

  struct Params {
    bool ok{false};
    uint8_t ADDH{0}, ADDL{0}, SPED{0}, CHAN{0}, OPTION{0};
  };

  enum SelfTestResult : uint8_t {
    ST_OK = 0,
    ST_ERR_VERSION = 1,
    ST_ERR_PARAMS  = 2,
    ST_ERR_TX      = 3
  };

  // Basis
  void begin(uint32_t baud=9600);
  void setMode(Mode m);
  bool waitAux(uint32_t timeout_ms=500);
  bool auxLevel();

  // Abfragen
  Version readVersion(uint32_t timeout_ms=700);
  Params  readParams(uint32_t timeout_ms=1200);

  // TX-Test
  bool    sendTestPacket(const uint8_t* data, size_t len, uint32_t timeout_ms=800);

  // Selbsttest (Version -> Params -> Test-TX)
  SelfTestResult selfTest(uint32_t ver_to_ms = 700, uint32_t par_to_ms = 1200, uint32_t tx_to_ms = 1000);

} // namespace Radio
