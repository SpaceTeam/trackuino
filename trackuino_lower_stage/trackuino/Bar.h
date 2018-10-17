#ifndef Bar_h
#define Bar_h

#include "Arduino.h"
#include "SPIX0.h"
#include "configX.h"

class Bar
{
  public:
  uint16_t C1, C2, C3, C4, C5, C6;
  int32_t TEMP, dT, P;
  uint32_t timeLastTempMeas, timeout, D1, D2;

  void Init();
  
  void RESET();
  void PROM_READ(byte addr, byte* receiveBuffer);

  void PerformPressMeas();
  void PerformTempMeas();
  uint32_t ReadResult();
  void CalcPress();
  void CalcTemp();
  bool ReadD1();
  void PerformTempMeasAndReadD2();
};

#endif


