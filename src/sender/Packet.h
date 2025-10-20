#pragma once
#include <Arduino.h>
#include "CRC16.h"

namespace zp {
static constexpr uint16_t MAGIC=0x5A51; // 'ZQ'
static constexpr uint8_t  VER=1;

enum Type:uint8_t{ HEARTBEAT=0x01,PING=0x02,PONG=0x03, STATUS=0x04, ARM=0x10, FIRE=0x11 };

#pragma pack(push,1)
struct H{
  uint16_t magic; uint8_t ver; uint8_t type; uint8_t sender_id;
  uint16_t seq; uint16_t len;
};
#pragma pack(pop)

struct Packet{
  H h; static constexpr uint16_t MAX=64;
  uint8_t p[MAX]; uint16_t crc;

  void finalize(){
    h.magic=MAGIC; h.ver=VER; h.len=min<uint16_t>(h.len,MAX);
    uint8_t tmp[sizeof(H)+MAX];
    memcpy(tmp,&h,sizeof(H)); memcpy(tmp+sizeof(H),p,h.len);
    crc=crc16_ccitt(tmp,sizeof(H)+h.len);
  }
  bool ok()const{
    if(h.magic!=MAGIC||h.ver!=VER||h.len>MAX) return false;
    uint8_t tmp[sizeof(H)+MAX];
    memcpy(tmp,&h,sizeof(H)); memcpy(tmp+sizeof(H),p,h.len);
    return crc==crc16_ccitt(tmp,sizeof(H)+h.len);
  }
};

inline bool send(HardwareSerial& s,const Packet& q){
  const size_t n=sizeof(H)+q.h.len+sizeof(q.crc);
  return s.write((const uint8_t*)&q.h,sizeof(H))==sizeof(H)
    && s.write(q.p,q.h.len)==q.h.len
    && s.write((const uint8_t*)&q.crc,sizeof(q.crc))==sizeof(q.crc);
}
inline bool recv(HardwareSerial& s, Packet& out, uint32_t to_ms=30){
  auto t0=millis(); auto need=[&](uint8_t* d,size_t n){
    size_t g=0; while(g<n){ if(s.available()) g+=s.readBytes(d+g,n-g);
      else if(millis()-t0>to_ms) return false; yield(); } return true; };
  if(!need((uint8_t*)&out.h,sizeof(H))) return false;
  if(out.h.len>Packet::MAX) return false;
  if(!need(out.p,out.h.len)) return false;
  if(!need((uint8_t*)&out.crc,sizeof(out.crc))) return false;
  return out.ok();
}
} // namespace zp
