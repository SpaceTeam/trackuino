#ifndef SPIX0_h
#define SPIX0_h

#include "Arduino.h"

class SPIX0
{
  public:
  
  void Init();
  void Transceive(byte* dataToTransmit, byte numBytesToTransmit, byte* receiveBuffer, byte numBytesToReceive);
  void Receive(byte* receiveBuffer, byte numBytesToReceive);
  void Transmit(byte* dataToTransmit, byte numBytesToTransmit);
};

extern SPIX0 spi0;

#endif


