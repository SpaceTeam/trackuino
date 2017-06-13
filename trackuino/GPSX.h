#ifndef GPSX_h
#define GPSX_h

#include "Arduino.h"
#include "gps.h"
#include "Flash.h"

class GPSX
{
  public:
  

  void Init();
  void sendUBX(uint8_t *MSG, uint8_t len);
  boolean getUBX_ACK(uint8_t *MSG);
  void buildUBXpacket(byte CLASS, byte ID, uint16_t LENGTH, byte* Payload, byte* Packet);
  void sendUBX_and_getUBX_ACK(byte* Packet, int len);

  void setFlightMode();
  void CFG_RATE(uint16_t measRate, uint16_t navRate, uint16_t timeRef);

  void saveGPSPoint();
};

#endif

