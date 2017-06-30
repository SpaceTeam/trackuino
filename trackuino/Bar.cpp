#include "Bar.h"

void Bar::RESET()
{
  byte sendData[] = {0x1e};

  PORTD &= ~(1 << 5);

  spi0.Transmit(sendData, 1);

  //wait while MISO is low and reset is performed
  uint32_t ts = micros();
  do
  {
    if (micros() - ts > BAR_TIMEOUT) break;
    SPDR0 = 0;
    while (!(SPSR0 & 0x80));
  } while (SPDR0 == 0);

  PORTD |= (1 << 5);
}

void Bar::PROM_READ(byte addr, byte* receiveBuffer)
{
  byte sendData[] = { B10100000 | ((addr & 0x7) << 1) };

  PORTD &= ~(1 << 5);

  spi0.Transceive(sendData, 1, receiveBuffer, 2);

  PORTD |= 1 << 5;
}

void Bar::Init()
{
  RESET();

  delay(10);

  byte resp[2];

  PROM_READ(1, resp);
  C1 = ((uint16_t)resp[0] << 8) | resp[1];

  PROM_READ(2, resp);
  C2 = ((uint16_t)resp[0] << 8) | resp[1];

  PROM_READ(3, resp);
  C3 = ((uint16_t)resp[0] << 8) | resp[1];

  PROM_READ(4, resp);
  C4 = ((uint16_t)resp[0] << 8) | resp[1];

  PROM_READ(5, resp);
  C5 = ((uint16_t)resp[0] << 8) | resp[1];

  PROM_READ(6, resp);
  C6 = ((uint16_t)resp[0] << 8) | resp[1];

  //Serial.print("\nC1="+String(C1)+"; C2="+String(C2)+"; C3="+String(C3)+"; C4="+String(C4)+"; C5="+String(C5)+"; C6="+String(C6));
  timeLastTempMeas = 0;
}

void Bar::PerformPressMeas()
{
  byte sendData[] = { 0x40 };

  PORTD &= ~(1 << 5);

  spi0.Transmit(sendData, 1);

  PORTD |= (1 << 5);
  
  timeout = micros();
}

void Bar::PerformTempMeas()
{
  byte sendData[] = { 0x50 };

  PORTD &= ~(1 << 5);

  spi0.Transmit(sendData, 1);

  PORTD |= (1 << 5);

  timeout = micros();
}

uint32_t Bar::ReadResult()
{
  byte sendData[] = {0};
  byte response[3];

  PORTD &= ~(1 << 5);

  spi0.Transceive(sendData, 1, response, 3);

  PORTD |= (1 << 5);

  uint32_t D = ((uint32_t)response[0] << 16) | ((uint32_t)response[1] << 8) | response[2];

  return D;
}

void Bar::CalcPress()
{
  int64_t OFF = ((int64_t) C2 << 16) + (((int64_t) dT * C4 ) >> 7);
  int64_t SENS = ((int32_t) C1 << 15) + (((int64_t) dT * C3 ) >> 8);

  //  int64_t OFF2 = 0;
  //  int64_t SENS2 = 0;
  //
  //  if (TEMP < 2000)
  //  {
  //    OFF2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 2;
  //    SENS2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 4;
  //  }
  //
  //  if (TEMP < -1500)
  //  {
  //    OFF2 = OFF2 + 7 * ((TEMP + 1500) * (TEMP + 1500));
  //    SENS2 = SENS2 + 11 * ((TEMP + 1500) * (TEMP + 1500)) / 2;
  //  }
  //
  //  OFF = OFF - OFF2;
  //  SENS = SENS - SENS2;

  P = ((((D1 * SENS) >> 21) - OFF) >> 15);
  //Serial.print("\nOFF="+String((int32_t)(OFF/1000000))+"; SENS="+String((int32_t)(SENS/1000000)));
}

void Bar::CalcTemp()
{
  dT = (uint32_t) D2 - ((uint32_t) C5 << 8);

  TEMP = 2000 + (int32_t)( ((int64_t) dT * C6) >> 23);

  //  int32_t TEMP2 = 0;
  //
  //  if (TEMP < 2000)
  //  {
  //    TEMP2 = (dT * dT) / (2 << 30);
  //  }
  //
  //  TEMP = TEMP - TEMP2;

  //Serial.print("\ndT="+String(dT));
}

bool Bar::ReadD1()
{
  if(micros() - timeout < BAR_PERFORM_TIME) return false;
  
  D1 = ReadResult();

  if (D1 == 0)
  {
    if (micros() - timeout < BAR_TIMEOUT) return false;
    else
    {
      PerformPressMeas();
      return true;
    }
  }

  return true;
}

void Bar::PerformTempMeasAndReadD2()
{
  //perform temperature measurement
  PerformTempMeas();
//    byte sendData[] = { 0x50 };
//
//  PORTD &= ~(1 << 5);
//
//  spi0.Transmit(sendData, 1);
//  do
//  {
//    SPDR0 = 0;
//    while (!(SPSR0 & 0x80));
//  }while(SPDR0==0);
//
//  PORTD |= (1 << 5);

  delayMicroseconds(BAR_PERFORM_TIME);
  do
  {
    if (micros() - timeout > BAR_TIMEOUT) break;
    D2 = ReadResult();
  } while (D2 == 0);
}

