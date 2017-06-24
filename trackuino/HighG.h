#ifndef HighG_h
#define HighG_h

#include "Arduino.h"
#include "SPIX0.h"

#define HIGH_G_LSB 49e-3

class HighG
{
  public:
  int accx, accy, accz;

  void Init();
  byte ReadRegister(byte addr);
  void ReadMultipleRegisters(byte addr, byte* receiveBuffer, byte amount);
  byte WriteRegister(byte addr, byte data);
  void WriteMultipleRegisters(byte addr, byte amount, byte* data);
  bool ReadXYZ();
};

#endif


