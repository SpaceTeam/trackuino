#include "Gyro.h"
#include "SPIX0.h"

void Gyro::Init()
{
  WriteRegister(0x21,0); //no sensitive trigger, high pass filters disabled
  WriteRegister(0x22,0); //all interupts, opend drain disabled
  WriteRegister(0x23,0);
  WriteRegister(0x24,B00000000); //fifo disabled

  WriteRegister(0x20,B11111111); //full data rate, bandwidth, normal mode and all axis enabled
}

byte Gyro::ReadRegister(byte addr)
{
  byte sendData[] = {0x80 | addr};
  byte response[1];
  
  PORTD &= ~(1<<2);
  
  spi0.Transceive(sendData, 1, response, 1);

  PORTD |= 1<<2;

  return response[0];
}

void Gyro::ReadMultipleRegisters(byte addr, byte* receiveBuffer, byte amount)
{
  byte sendData[] = {0xc0 | (addr&0x3f)};
  
  PORTD &= ~(1<<2);
  
  spi0.Transceive(sendData, 1, receiveBuffer, amount);

  PORTD |= 1<<2;
}

void Gyro::WriteRegister(byte addr, byte data)
{
  byte sendData[] = {addr, data};
  
  PORTD &= ~(1<<2);
  
  spi0.Transmit(sendData, 2);

  PORTD |= 1<<2;
}

void Gyro::WriteMultipleRegisters(byte addr, byte amount, byte* data)
{
  byte sendData[] = {0x40 | (addr&0x3f)};
  
  PORTD &= ~(1<<2);

  spi0.Transmit(sendData,1);
  spi0.Transmit(data, amount);

  PORTD |= 1<<2;
}

bool Gyro::ReadXYZ()
{
  if(ReadRegister(0x27)&(1<<3)==0) return false;

  byte receiveBuffer [6];
  ReadMultipleRegisters(0x28, receiveBuffer, 6);
  spinx = ((int)receiveBuffer[1] << 8) | receiveBuffer[0];
  spiny = ((int)receiveBuffer[3] << 8) | receiveBuffer[2];
  spinz = ((int)receiveBuffer[5] << 8) | receiveBuffer[4];

  return true;
}


