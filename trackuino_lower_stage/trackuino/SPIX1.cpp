//#include "Arduino.h"
#include "SPIX1.h"

SPIX1 spi1;

void SPIX1::Init()
{
  //CS high:
  DDRB |= 1;
  PORTB |= 1;
  
  DDRE |= 1<<2;
  DDRC |= 1<<1;
  DDRE |= 1<<3;

  SPCR1 = B01010000;

  SPSR1;
  SPDR1;
}

void SPIX1::Transceive(byte* dataToTransmit, int numBytesToTransmit, byte* receiveBuffer, int numBytesToReceive)
{
  int i;

  for (i = 0; i < numBytesToTransmit; i++)
  {
    SPDR1 = dataToTransmit[i];
    while (!(SPSR1 & 0x80));
  }

  for (i = 0; i < numBytesToReceive; i++)
  {
    SPDR1 = 0;
    while (!(SPSR1 & 0x80));
    receiveBuffer[i] = SPDR1;
  }
}

void SPIX1::Receive(byte* receiveBuffer, int numBytesToReceive)
{
  for (int i = 0; i < numBytesToReceive; i++)
  {
    SPDR1 = 0;
    while (!(SPSR1 & 0x80));
    receiveBuffer[i] = SPDR1;
  }
}

void SPIX1::Transmit(byte* dataToTransmit, int numBytesToTransmit)
{
  for (int i = 0; i < numBytesToTransmit; i++)
  {
    SPDR1 = dataToTransmit[i];
    while (!(SPSR1 & 0x80));
  }
}

