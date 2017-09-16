#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <fcntl.h>
#include <string.h>
#include "PDKFRU.h"
#include "PDKEEPROM.h"

int FRU_Check(char *fruFileName)
{
    uint8_t i;
    uint8_t Data[8];
    uint8_t checksum = 0;

    ReadWriteEEPROM(fruFileName, Data, 0, 8, READ_EEPROM);

    for(i=0; i<8; i++)
    {
        checksum += Data[i];
    }

    if(checksum == 0)
        return 0;
    else
        return -1;
}

