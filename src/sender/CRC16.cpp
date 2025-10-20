#include "CRC16.h"
uint16_t crc16_ccitt(const uint8_t* d, size_t n){
  uint16_t c=0xFFFF; for(size_t i=0;i<n;i++){ c^=(uint16_t)d[i]<<8;
    for(uint8_t j=0;j<8;j++) c=(c&0x8000)?(c<<1)^0x1021:(c<<1); }
  return c;
}
