#ifndef __PDKEEPROM_H__
#define __PDKEEPROM_H__

#include <inttypes.h>

#define WRITE_EEPROM        1
#define READ_EEPROM         2

#define EEPROM_MAX_SIZE     8096 // 8K

extern int GetFileLength(char *fileName, uint32_t *length);
extern int ReadWriteEEPROM(char* EEPROMFileName, uint8_t* pData, uint16_t Offset, uint16_t Size, uint8_t RWFlag);

#endif

