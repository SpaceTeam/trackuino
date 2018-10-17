#ifndef ReadMem_h
#define ReadMem_h

#include "Arduino.h"
#include "SPIX1.h"
#include "Flash.h"
#include "configX.h"

class ReadMem
{
  public:
  
  bool CheckCom();
  uint8_t CheckIfEraseFlash();
  bool CheckResponse(char* expected_response, byte len);
  void ReadAndSendData(uint16_t C1, uint16_t C2, uint16_t C3, uint16_t C4, uint16_t C5, uint16_t C6);
};

#endif

