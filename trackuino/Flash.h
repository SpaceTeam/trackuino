#ifndef Flash_h
#define Flash_h

#include "Arduino.h"
#include "SPIX1.h"
#include "configX.h"

class Flash
{
  public:
    unsigned long memPos; //actual adress to write next page
    uint16_t pagePos;
    byte* buf[POINT_LEN_SENSORS];

    bool Init(); //Initialize flash; pointSize = number of bytes one point contains

    void Flash::REMS(byte* manufacturer_id, byte* device_id);
    void BRWR(byte BAR); //Bank Register Write (BAR[7] is Extended Address Control Bit EXTADD and is necessary to be set to use 4 byte adresses)
    byte RDSR1(); //ReaD Status Register 1
    void CLSR(); //CLear Status Register

    void WREN(); //Write Enable command: necessary to enable PP command

    void READ(unsigned long address, byte* receiveBuffer, int amount); //Read amount bytes from flash (4 byte address)
    void PP(unsigned long address, byte* data, int amount); //Page Program: Writes data to flash (maximum 256 bytes at once, best performance 256 bytes, 4 byte address)

    void SEr(unsigned long address); //Sector Erase: deletes sector
    void BE(); //Bulk Erase: deletes whole memory

    void RESET(); //software reset flash

    bool CheckIfEmpty(byte* data, int len, int maxWrittenBytes); //help funcion which checks if all data bytes equal 0xff
    void Write(byte* data, int len);
};

extern Flash flash;

#endif


