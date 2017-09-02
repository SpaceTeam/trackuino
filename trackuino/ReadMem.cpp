#include "ReadMem.h"

bool ReadMem::CheckCom()
{
  Serial.begin(1000000);

  bool comCheck = false;
  uint32_t timeout = millis();
  char answer[] = {'o', 'k'};
  byte answerLen = 2;
  do
  {
    Serial.print("send me a command\n");
    delay(100);
    comCheck = CheckResponse(answer, answerLen);
  } while (comCheck == false && millis() - timeout < COM_TIMEOUT);

  if (!comCheck) Serial.end();
  else
  {
    Serial.print("got it\n");
    Serial.print(String(flash.memPos) + "\n");
  }

  return comCheck;
}

uint8_t ReadMem::CheckIfEraseFlash()
{
  while (Serial.available() < 5) {}

  char answer[6];

  for(int i=0; i<5; i++)
  {
    answer[i] = Serial.read();
  }
  answer[5] = '\0';

  if(strcmp(answer, "erase") == 0) return 1;
  if(strcmp(answer, "reout") == 0) return 2;
  return 0;
}

void ReadMem::ReadAndSendData()
{
  int i;
  byte spiReadCmd[5];
  spiReadCmd[0] = 0x13;

  Serial.print(String(POINT_INDICATOR_RESET) + "\n");
  Serial.print(String(POINT_INDICATOR_SENSORS) + "\n");
  Serial.print(String(POINT_INDICATOR_GPS) + "\n");
  Serial.print(String(POINT_LEN_SENSORS) + "\n");
  Serial.print(String(POINT_LEN_GPS) + "\n");
  Serial.print(String(POINT_POS_BAR) + "\n");
  Serial.print(String(POINT_POS_ACC) + "\n");
  Serial.print(String(POINT_POS_HG) + "\n");
  Serial.print(String(POINT_POS_GYRO) + "\n");
  Serial.print(String(POINT_POS_TIME) + "\n");
  Serial.print(String(POINT_POS_LAT) + "\n");
  Serial.print(String(POINT_POS_LON) + "\n");
  Serial.print(String(POINT_POS_ALT) + "\n");
  Serial.print(String(POINT_POS_SATS) + "\n");
  Serial.print(String(POINT_POS_FIXQ) + "\n");
  Serial.print(String(ACC_LSB,4) + "\n");
  Serial.print(String(HIGH_G_LSB,4) + "\n");
  Serial.print(String(GYRO_LSB,4) + "\n");
  Serial.print(String(PAGE_SIZE) + "\n");

  for (uint32_t addr = 0; addr < flash.memPos; addr += PAGE_SIZE)
  {
    Serial.print("page\n");
    
    Serial.write(addr >> 24);
    Serial.write(addr >> 16);
    Serial.write(addr >> 8);
    Serial.write(addr);

    spiReadCmd[1] = addr >> 24;
    spiReadCmd[2] = addr >> 16;
    spiReadCmd[3] = addr >> 8;
    spiReadCmd[4] = addr;

    PORTB &= ~1;

    spi1.Transmit(spiReadCmd, 5);

    for (i = 0; i < PAGE_SIZE; i++)
    {
      SPDR1 = 0;
      while (!(SPSR1 & 0x80));
      Serial.write(SPDR1);
    }
    
    PORTB |= 1;
  }

  Serial.end();
}

bool ReadMem::CheckResponse(char* expected_response, byte len)
{
  bool correct_response = false;
  byte i;

  uint32_t timeout = millis();
  while (Serial.available() < len)
  {
    if (millis() - timeout > 100) return false;
  }

  while (Serial.available() > 0)
  {
    for (i = 0; i < len; i++)
    {
      if (Serial.read() != expected_response[i]) break;
      if (i == len - 1) correct_response = true;
    }
  }

  return correct_response;
}
