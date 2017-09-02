#ifndef GPSX_h
#define GPSX_h

#include "Arduino.h"
#include "gps.h"
#include "Flash.h"
#include "configX.h"

class GPSX
{
  public:
  //members for saving last position
  char last_gps_time[7];
  char last_gps_aprs_lat[9];
  char last_gps_aprs_lon[10];
  float last_gps_altitude;
  uint8_t last_gps_fixq;
  
  void Init();
  void saveGPSPoint();
  void copyLastPosToBuf();
  
  void sendUBX(uint8_t *MSG, uint8_t len);
  boolean getUBX_ACK(uint8_t *MSG);
  void buildUBXpacket(byte CLASS, byte ID, uint16_t LENGTH, byte* Payload, byte* Packet);
  void sendUBX_and_getUBX_ACK(byte* Packet, int len);

  void setFlightMode();
  void CFG_RATE(uint16_t measRate, uint16_t navRate, uint16_t timeRef);
  void RXM_PMREQ(uint32_t duration, uint32_t flags);
};

extern GPSX gpsx;

#endif

