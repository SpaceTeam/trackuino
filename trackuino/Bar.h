#ifndef Bar_h
#define Bar_h

#include "Arduino.h"
#include "SPIX0.h"
#include "Flash.h"

class Bar
{
  public:
  uint16_t c1, c2, c3, c4, c5, c6;
  int32_t TEMP, dT;
  uint32_t timeLastTempMeas, timeout, D1, D2;

  void Init();
  
  void RESET();
  byte* PROM_READ(byte addr);

  void PerformPressMeas();
  void PerformTempMeas();
  uint32_t ReadResult();
  int32_t GetPress(uint32_t D1);
  int32_t GetTemp(uint32_t D2);
  bool ReadD1();
  void PerformTempMeasAndReadD2();
};

#endif


