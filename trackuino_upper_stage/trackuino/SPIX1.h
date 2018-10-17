#ifndef SPIX_h
#define SPIX_h

#include "Arduino.h"

class SPIX1
{
  public:
  
  void Init();
  void Transceive(byte* dataToTransmit, int numBytesToTransmit, byte* receiveBuffer, int numBytesToReceive);
  void Receive(byte* receiveBuffer, int numBytesToReceive);
  void Transmit(byte* dataToTransmit, int numBytesToTransmit);
  
};

extern SPIX1 spi1;

#endif


