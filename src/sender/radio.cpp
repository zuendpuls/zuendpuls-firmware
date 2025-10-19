#include "radio.h"
#include <cstring> // strlen

namespace {
  inline void pinWrite(uint8_t p, bool hi){ digitalWrite(p, hi?HIGH:LOW); }
  inline bool pinRead(uint8_t p){ return digitalRead(p); }

  const uint8_t CMD_READ_VERSION[3] = {0xC3,0x00,0x08};
  const uint8_t CMD_READ_PARAMS [3] = {0xC1,0x00,0x09};

  void clearSerial(HardwareSerial& s){
    while (s.available()) (void)s.read();
  }

  size_t readBytesTimeout(HardwareSerial& s, uint8_t* buf, size_t len, uint32_t to_ms){
    uint32_t start = millis(); size_t got = 0;
    while (got < len && (millis() - start) < to_ms){
      if (s.available()){
        buf[got++] = (uint8_t)s.read();
      } else {
        yield();
      }
    }
    return got;
  }

  // Suche nach Sequenz C1 00 09 im Puffer
  int findParamsHeader(const uint8_t* b, int n){
    for (int i=0; i<=n-3; ++i){
      if (b[i]==0xC1 && b[i+1]==0x00 && b[i+2]==0x09) return i;
    }
    return -1;
  }
}

namespace Radio {

  void begin(uint32_t baud){
    pinMode(PIN_RADIO_M0, OUTPUT);
    pinMode(PIN_RADIO_M1, OUTPUT);
    pinMode(PIN_RADIO_AUX, INPUT);

    // Start in SLEEP/Config (M1=1, M0=1)
    pinWrite(PIN_RADIO_M0, HIGH);
    pinWrite(PIN_RADIO_M1, HIGH);
    delay(20);

    Serial1.begin(baud);
    delay(60);
    waitAux(1000);
    clearSerial(Serial1);
  }

  bool auxLevel(){ return pinRead(PIN_RADIO_AUX); }

  void setMode(Mode m){
    switch(m){
      case MODE_NORMAL:       pinWrite(PIN_RADIO_M1, LOW);  pinWrite(PIN_RADIO_M0, LOW);  break;
      case MODE_WAKEUP:       pinWrite(PIN_RADIO_M1, LOW);  pinWrite(PIN_RADIO_M0, HIGH); break;
      case MODE_POWER_SAVING: pinWrite(PIN_RADIO_M1, HIGH); pinWrite(PIN_RADIO_M0, LOW);  break;
      case MODE_SLEEP:        pinWrite(PIN_RADIO_M1, HIGH); pinWrite(PIN_RADIO_M0, HIGH); break;
    }
    delay(12);
    waitAux(1000);
    clearSerial(Serial1);
  }

  bool waitAux(uint32_t timeout_ms){
    uint32_t t0 = millis();
    while ((millis()-t0) < timeout_ms){
      if (pinRead(PIN_RADIO_AUX)) return true;
    }
    return false;
  }

  Version readVersion(uint32_t timeout_ms){
    Version v;
    setMode(MODE_SLEEP);
    clearSerial(Serial1);

    Serial1.write(CMD_READ_VERSION, 3);
    Serial1.flush();
    delay(10); // kleinen Moment geben

    uint8_t buf[8] = {0};
    size_t n = readBytesTimeout(Serial1, buf, sizeof(buf), timeout_ms);

    if (n >= 4 && buf[0] == 0xC3){
      v.ok     = true;
      v.ver    = buf[1];
      v.module = buf[2];
      v.freq   = buf[3];
    }
    return v;
  }

  Params readParams(uint32_t timeout_ms){
    Params p;
    setMode(MODE_SLEEP);
    clearSerial(Serial1);

    Serial1.write(CMD_READ_PARAMS, 3);
    Serial1.flush();
    delay(10);

    // Wir wissen nicht genau, wie lang die Antwort ausfällt:
    // daher großzügiger Puffer und dann nach Header scannen.
    uint8_t tmp[32] = {0};
    size_t n = readBytesTimeout(Serial1, tmp, sizeof(tmp), timeout_ms);

    // Versuch 1: Header suchen (C1 00 09)
    int k = findParamsHeader(tmp, (int)n);
    if (k >= 0){
      // danach sollten mind. 6 Nutzbytes kommen
      if (k + 9 <= (int)n){
        p.ADDH   = tmp[k+3];
        p.ADDL   = tmp[k+4];
        p.SPED   = tmp[k+5];
        p.CHAN   = tmp[k+6];
        p.OPTION = tmp[k+7];
        p.ok = true;
        return p;
      }
    }

    // Versuch 2: Einige Module liefern nur 6 Bytes (ohne Header) direkt.
    if (n == 6){
      p.ADDH   = tmp[0];
      p.ADDL   = tmp[1];
      p.SPED   = tmp[2];
      p.CHAN   = tmp[3];
      p.OPTION = tmp[4];
      p.ok = true;
      return p;
    }

    // Versuch 3: Wenn >=6 Bytes insgesamt, nimm die letzten 6 als Payload.
    if (n >= 6){
      p.ADDH   = tmp[n-6];
      p.ADDL   = tmp[n-5];
      p.SPED   = tmp[n-4];
      p.CHAN   = tmp[n-3];
      p.OPTION = tmp[n-2];
      p.ok = true;
      return p;
    }

    // sonst: fail
    p.ok = false;
    return p;
  }

  bool sendTestPacket(const uint8_t* data, size_t len, uint32_t timeout_ms){
    setMode(MODE_NORMAL);
    size_t sent = Serial1.write(data, len);
    Serial1.flush();
    // Warte bis AUX wieder HIGH (TX fertig)
    return (sent == len) && waitAux(timeout_ms);
  }

  SelfTestResult selfTest(uint32_t ver_to_ms, uint32_t par_to_ms, uint32_t tx_to_ms){
    Version rv = readVersion(ver_to_ms);
    if (!rv.ok) return ST_ERR_VERSION;

    Params rp = readParams(par_to_ms);
    if (!rp.ok) return ST_ERR_PARAMS;

    const char* msg = "SENDER-HELLO";
    bool txok = sendTestPacket((const uint8_t*)msg, strlen(msg), tx_to_ms);
    if (!txok) return ST_ERR_TX;

    return ST_OK;
  }

} // namespace Radio
