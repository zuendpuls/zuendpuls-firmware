#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "config.h"

namespace RTCX {

struct RtcTime {
  uint8_t sec{0}, min{0}, hour{0}, day{1}, month{1};
  uint16_t year{2000};
  bool valid{false};
};

inline uint8_t bcd2bin(uint8_t v){ return v - 6 * (v >> 4); }
inline uint8_t bin2bcd(uint8_t v){ return v + 6 * (v / 10); }

inline void begin(){
  // Teensy 4.x: Standard I2C-Pins (SDA=18, SCL=19)
  Wire.begin();
  Wire.setClock(400000); // 400kHz okay für DS3231
}

// Liest Zeit aus DS3231 @ 0x68
inline bool read(RtcTime &t){
  Wire.beginTransmission(0x68);
  Wire.write((uint8_t)0x00); // Sek.-Register
  if (Wire.endTransmission(false) != 0) return t.valid = false, false;

  // 7 Bytes: sec, min, hour, dow, day, month, year
  if (Wire.requestFrom(0x68, 7u) != 7) return t.valid = false, false;

  uint8_t sec   = Wire.read();
  uint8_t min   = Wire.read();
  uint8_t hour  = Wire.read();
  (void)Wire.read();             // Wochentag ignorieren
  uint8_t mday  = Wire.read();
  uint8_t month = Wire.read();
  uint8_t year  = Wire.read();

  // Stunden: 12/24h Bit6 in hour; wir erzwingen 24h
  if (hour & 0x40){              // 12h-Format
    bool pm = hour & 0x20;
    hour = bcd2bin(hour & 0x1F);
    if (pm && hour < 12) hour += 12;
    if (!pm && hour == 12) hour = 0;
  } else {
    hour = bcd2bin(hour & 0x3F);
  }

  t.sec   = bcd2bin(sec & 0x7F);
  t.min   = bcd2bin(min & 0x7F);
  t.hour  = hour;
  t.day   = bcd2bin(mday & 0x3F);
  t.month = bcd2bin(month & 0x1F);
  t.year  = 2000 + bcd2bin(year);
  t.valid = true;
  return true;
}

// Optional: Uhr stellen (24h)
inline bool set(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss){
  if (y < 2000) y = 2000;
  Wire.beginTransmission(0x68);
  Wire.write((uint8_t)0x00);
  Wire.write(bin2bcd(ss));
  Wire.write(bin2bcd(mm));
  Wire.write(bin2bcd(hh));     // 24h
  Wire.write((uint8_t)0x01);   // Wochentag dummy
  Wire.write(bin2bcd(d));
  Wire.write(bin2bcd(m));
  Wire.write(bin2bcd((uint8_t)(y - 2000)));
  return Wire.endTransmission() == 0;
}

} // namespace RTCX
