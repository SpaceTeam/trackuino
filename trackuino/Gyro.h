#ifndef Gyro_h
#define Gyro_h

#include "Arduino.h"
#include "SPIX0.h"
#include "Flash.h"

class Gyro
{
  public:
  int spinx, spiny, spinz;
  
  void Init();
  byte ReadRegister(byte addr);
  void ReadMultipleRegisters(byte addr, byte* receiveBuffer, byte amount);
  void WriteRegister(byte addr, byte data);
  void WriteMultipleRegisters(byte addr, byte amount, byte* data);
  bool ReadXYZ();
};

#endif


