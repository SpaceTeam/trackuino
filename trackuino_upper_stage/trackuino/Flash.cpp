#include "SPIX1.h"
#include "Flash.h"

Flash flash;

void Flash::REMS(byte* manufacturer_id, byte* device_id)
{
  byte response[2];
  byte sendData[] = {0x90, 0, 0, 0};

  PORTB &= ~1;

  spi1.Transceive(sendData, 4, response, 2);

  PORTB |= 1;

  *manufacturer_id = response[0];
  *device_id = response[1];
}

void Flash::BRWR(byte BAR)
{
  byte sendData[] = {0x17, BAR};

  PORTB &= ~1;

  spi1.Transmit(sendData, 2);

  PORTB |= 1;
}

byte Flash::RDSR1()
{
  byte sendData[] = {0x5};
  byte response[1];

  PORTB &= ~1;

  spi1.Transceive(sendData, 1, response, 1);

  PORTB |= 1;

  return response[0];
}

void Flash::CLSR()
{
  byte sendData[] = {0x30};

  PORTB &= ~1;

  spi1.Transmit(sendData, 1);

  PORTB |= 1;
}

void Flash::WREN()
{
  byte sendData[] = {0x6};

  PORTB &= ~1;

  spi1.Transmit(sendData, 1);

  PORTB |= 1;
}

void Flash::READ(unsigned long address, byte* receiveBuffer, int amount)
{
  byte sendData[] = {0x13, address >> 24, address >> 16, address >> 8, address};

  PORTB &= ~1;

  spi1.Transceive(sendData, 5, receiveBuffer, amount);

  PORTB |= 1;
}

void Flash::PP(unsigned long address, byte* data, int amount)
{
  byte cmd[] = {0x12, address >> 24, address >> 16, address >> 8, address};

  PORTB &= ~1;

  spi1.Transmit(cmd, 5);
  spi1.Transmit(data, amount);

  PORTB |= 1;
}

void Flash::SEr(unsigned long address)
{
  byte sendData[] = {0xdc, address >> 24, address >> 16, address >> 8, address};

  PORTB &= ~1;

  spi1.Transmit(sendData, 5);

  PORTB |= 1;
}

void Flash::BE()
{
  byte sendData[] = {0xc7};

  PORTB &= ~1;

  spi1.Transmit(sendData, 1);

  PORTB |= 1;
}

void Flash::RESET()
{
  byte sendData[] = {0xf0};

  PORTB &= ~1;

  spi1.Transmit(sendData, 1);

  PORTB |= 1;
}

void Flash::Init()
{
  RESET(); //software reset flash

  delay(10);

  //find boundaries of free memory using kind of a bubble sort:
  byte actualPage[PAGE_SIZE];
  unsigned long upperAddr = MEM_SIZE / PAGE_SIZE - PAGE_SIZE;
  unsigned long lowerAddr = 0;
  unsigned long actualAddr;
  do
  {
    actualAddr = lowerAddr + (upperAddr - lowerAddr) / 2;

    READ(actualAddr << 8, actualPage, PAGE_SIZE);

    if (CheckIfEmpty(actualPage, PAGE_SIZE, 5)) upperAddr = actualAddr;
    else lowerAddr = actualAddr;
  } while (upperAddr - lowerAddr > 1);
  if (upperAddr == 1)
  {
    READ(0, actualPage, PAGE_SIZE);
    if (CheckIfEmpty(actualPage, PAGE_SIZE, 5)) memPos = 0;
    else memPos = PAGE_SIZE;
  }
  else memPos = upperAddr << 8;

  pagePos = 0;
}

void Flash::EraseFlash()
{
  //delete complete memory with bulk erase command (needs max. 330 seconds)
  WREN();
  BE();
  memPos = 0;
}

bool Flash::CheckIfEmpty(byte* data, int len, int maxWrittenBytes)
{
  int datacnt = 0;
  for (int i = 0; i < len; i++)
  {
    if (data[i] != 0xff) datacnt++;
  }
  if (datacnt > maxWrittenBytes) return false;
  return true;
}

void Flash::Write(byte* data, int len)
{
  WREN();

  if (pagePos + len <= PAGE_SIZE)
  {
    PP(memPos + pagePos, data, len);
    pagePos += len;
  }
  else if (memPos < MEM_SIZE - PAGE_SIZE)
  {
    memPos += PAGE_SIZE;
    PP(memPos, data, len);
    pagePos = len;
  }
}

