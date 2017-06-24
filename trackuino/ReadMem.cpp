#include "ReadMem.h"

bool ReadMem::CheckCom()
{
  Serial.begin(1000000);

  bool comCheck = false;
  uint32_t timeout = millis();
  do
  {
    Serial.print("should i send my data?\n");
    delay(100);
    while (Serial.available() > 2)
    {
      if (Serial.read() == 'y')
      {
        if (Serial.read() == 'e')
        {
          if (Serial.read() == 's')
          {
            comCheck = true;
          }
        }
      }
    }
  } while (comCheck == false && millis() - timeout < COM_TIMEOUT);

  if (!comCheck) Serial.end();

  return comCheck;
}

void ReadMem::ReadAndSendData()
{
  byte page[PAGE_SIZE];
  int i,a;
  
  Serial.print(String(flash.memPos) + "\n");

  for (uint32_t addr = 0; addr < flash.memPos; addr += PAGE_SIZE)
  {
    Serial.print("addr=" + String(addr) + "\n");
    flash.READ(addr, page, PAGE_SIZE);
    i = 0;
    a = 0;
    while (i < PAGE_SIZE - POINT_LEN_GPS && i < PAGE_SIZE - POINT_LEN_SENSORS)
    {
      switch (page[i])
      {
        case POINT_INDICATOR_GPS:

          Serial.print("GPS\n");
          for(int a=i+1; a<i+POINT_LEN_GPS; a++) Serial.write(page[a]);
          Serial.write(0x9d);

          i += POINT_LEN_GPS;
          break;

        case POINT_INDICATOR_SENSORS:

          Serial.print("SEN\n");
          for(int a=i+1; a<i+POINT_LEN_SENSORS; a++) Serial.write(page[a]);
          Serial.write(0x9d);

          i += POINT_LEN_SENSORS;
          break;

        default:
          i++;
          break;
      }
    }
  }

  Serial.end();
}
