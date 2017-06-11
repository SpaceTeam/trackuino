#include "SPIX0.h"

SPIX0 spi0;

void SPIX0::Init()
{
  //CSs high:
  DDRD |= 1 << 2;
  PORTD |= 1 << 2;
  DDRD |= 1 << 5;
  PORTD |= 1 << 5;
  DDRD |= 1 << 6;
  PORTD |= 1 << 6;
  DDRD |= 1 << 7;
  PORTD |= 1 << 7;
  
  DDRB |= 1<<2;
  DDRB |= 1<<5;
  DDRB |= 1<<3;

  SPCR0 = B01011100;

  SPSR0;
  SPDR0;
}

void SPIX0::Transceive(byte* dataToTransmit, byte numBytesToTransmit, byte* receiveBuffer, byte numBytesToReceive)
{
  int i;

  for (i = 0; i < numBytesToTransmit; i++)
  {
    SPDR0 = dataToTransmit[i];
    while (!(SPSR0 & 0x80));
  }

  for (i = 0; i < numBytesToReceive; i++)
  {
    SPDR0 = 0;
    while (!(SPSR0 & 0x80));
    receiveBuffer[i] = SPDR0;
  }
}

void SPIX0::Receive(byte* receiveBuffer, byte numBytesToReceive)
{
  for (int i = 0; i < numBytesToReceive; i++)
  {
    SPDR0 = 0;
    while (!(SPSR0 & 0x80));
    receiveBuffer[i] = SPDR0;
  }
}

void SPIX0::Transmit(byte* dataToTransmit, byte numBytesToTransmit)
{
  for (int i = 0; i < numBytesToTransmit; i++)
  {
    SPDR0 = dataToTransmit[i];
    while (!(SPSR0 & 0x80));
  }
}


