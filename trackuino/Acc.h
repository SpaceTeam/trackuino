#ifndef Acc_h
#define Acc_h

#include "Arduino.h"
#include "SPIX0.h"

#define ACC_LSB 4e-3

class Acc
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


