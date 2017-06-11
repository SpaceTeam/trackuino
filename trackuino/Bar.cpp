#include "Bar.h"
#include "SPIX0.h"

void Bar::RESET()
{
  byte sendData[] = {0x1e};

  PORTD &= ~(1 << 5);

  spi0.Transmit(sendData, 1);

  while ((PINB & (1 << 4)) == 0) ; //wait until reset (until MISO goes up)

  PORTD |= (1 << 5);
}

byte* Bar::PROM_READ(byte addr)
{
  byte sendData[] = { B10100000 | ((addr & 0x7) << 1) };
  byte response[2];

  PORTD &= ~(1 << 5);

  spi0.Transceive(sendData, 1, response, 2);

  PORTD |= 1 << 5;

  return response;
}

void Bar::Init()
{
  byte* resp;

  DDRD |= 1 << 5;
  PORTD |= 1 << 5;

  RESET();
  delay(10);

  resp = PROM_READ(1);
  c1 = ((uint16_t)resp[0] << 8) | resp[1];
  delete[] resp;

  resp = PROM_READ(2);
  c2 = ((uint16_t)resp[0] << 8) | resp[1];
  delete[] resp;

  resp = PROM_READ(3);
  c3 = ((uint16_t)resp[0] << 8) | resp[1];
  delete[] resp;

  resp = PROM_READ(4);
  c4 = ((uint16_t)resp[0] << 8) | resp[1];
  delete[] resp;

  resp = PROM_READ(5);
  c5 = ((uint16_t)resp[0] << 8) | resp[1];
  delete[] resp;

  resp = PROM_READ(6);
  c6 = ((uint16_t)resp[0] << 8) | resp[1];
  delete[] resp;

  timeLastTempMeas = 0;
}

void Bar::PerformPressMeas()
{
  byte sendData[] = { 0x40 };

  PORTD &= ~(1 << 5);

  spi0.Transmit(sendData, 1);

  PORTD |= (1 << 5);
}

void Bar::PerformTempMeas()
{
  byte sendData[] = { 0x50 };

  PORTD &= ~(1 << 5);

  spi0.Transmit(sendData, 1);

  PORTD |= (1 << 5);
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

int32_t Bar::GetPress(uint32_t D1)
{
  int64_t OFF = (int64_t)c2 * 65536 + (int64_t)c4 * dT / 128;
  int64_t SENS = (int64_t)c1 * 32768 + (int64_t)c3 * dT / 256;

  int64_t OFF2 = 0;
  int64_t SENS2 = 0;

  if (TEMP < 2000)
  {
    OFF2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 2;
    SENS2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 4;
  }

  if (TEMP < -1500)
  {
    OFF2 = OFF2 + 7 * ((TEMP + 1500) * (TEMP + 1500));
    SENS2 = SENS2 + 11 * ((TEMP + 1500) * (TEMP + 1500)) / 2;
  }

  OFF = OFF - OFF2;
  SENS = SENS - SENS2;

  uint32_t P = (D1 * SENS / 2097152 - OFF) / 32768;

  return P;
}

int32_t Bar::GetTemp(uint32_t D2)
{
  dT = D2 - (uint32_t)c5 * 256;

  int32_t TEMP = 2000 + ((int64_t) dT * c6) / 8388608;

  int32_t TEMP2 = 0;

  if (TEMP < 2000)
  {
    TEMP2 = (dT * dT) / (2 << 30);
  }

  TEMP = TEMP - TEMP2;

  return TEMP;
}

bool Bar::ReadD1()
{
  D1 = ReadResult();

  if(D1 == 0) 
  {
    if(millis() - timeout < SENSOR_TIMEOUT) return false;
    else
    {
      PerformPressMeas();
      timeout = millis();
      return true;
    }
  }
  
  timeout = millis();

  return true;
}

void Bar::PerformTempMeasAndReadD2()
{
  //perform temperature measurement
  byte sendData[] = { 0x50 };
  PORTD &= ~(1 << 5);
  spi0.Transmit(sendData, 1);

  uint32_t ts = millis();
  //wait while MISO is low and measuremt is performed:
  do
  {
    if (millis() - ts > SENSOR_TIMEOUT) break;
    SPDR0 = 0;
    while (!(SPSR0 & 0x80));
  } while (SPDR0 == 0);

  PORTD |= 1 << 5;

  do
  {
    if (millis() - ts > SENSOR_TIMEOUT) break;
    D2 = ReadResult();
  } while (D2 == 0);
}

