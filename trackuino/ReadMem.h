#ifndef ReadMem_h
#define ReadMem_h

#include "Arduino.h"
#include "SPIX1.h"
#include "Flash.h"

#define COM_TIMEOUT 10000 //wait 10 secs until pc responds

class ReadMem
{
  public:

  bool CheckCom();
  void ReadAndSendData();
};

#endif

