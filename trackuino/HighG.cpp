#include "HighG.h"
#include "SPIX0.h"

byte HighG::ReadRegister(byte addr)
{
  byte sendData[] = {0x80 | addr};
  byte response[1];

  PORTD &= ~(1 << 7);

  spi0.Transceive(sendData, 1, response, 1);

  PORTD |= 1 << 7;

  return response[0];
}

void HighG::ReadMultipleRegisters(byte addr, byte* receiveBuffer, byte amount)
{
  byte sendData[] = {0xc0 | (addr & 0x3f)};

  PORTD &= ~(1 << 7);

  spi0.Transceive(sendData, 1, receiveBuffer, amount);

  PORTD |= 1 << 7;
}

byte HighG::WriteRegister(byte addr, byte data)
{
  byte sendData[] = {addr, data};

  PORTD &= ~(1 << 7);

  spi0.Transmit(sendData, 2);

  PORTD |= 1 << 7;
}

void HighG::WriteMultipleRegisters(byte addr, byte amount, byte* data)
{
  byte sendData[] = {0x40 | (addr & 0x3f)};

  PORTD &= ~(1 << 7);

  spi0.Transmit(sendData, 1);
  spi0.Transmit(data, amount);

  PORTD |= 1 << 7;
}

void HighG::Init()
{
  WriteRegister(0x2d, 0); //go to sleep
  WriteRegister(0x31, B00001011); //set full_res and 16g range
  WriteRegister(0x27, 0); //disable act_inact
  WriteRegister(0x2a, 0); //disable tap detection
  WriteRegister(0x2c, B1101); //set 800hz rate
  WriteRegister(0x2e, 0); //disable int
  WriteRegister(0x38, 0); //disable fifo

  WriteRegister(0x2d, B00001000); //go to measurement mode
}

bool HighG::ReadXYZ()
{
  if(ReadRegister(0x30)&(1<<7) == 0) return false;

  byte receiveBuffer [6];
  ReadMultipleRegisters(0x32, receiveBuffer, 6);
  accx = ((int)receiveBuffer[1] << 8) | receiveBuffer[0];
  accy = ((int)receiveBuffer[3] << 8) | receiveBuffer[2];
  accz = ((int)receiveBuffer[5] << 8) | receiveBuffer[4];

  return true;
}
