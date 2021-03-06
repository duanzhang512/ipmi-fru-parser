#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <fcntl.h>
#include <string.h>
#include "PDKEEPROM.h"

int GetFileLength(char *fileName, uint32_t *length)
{
	FILE * pFile;

	pFile = fopen(fileName, "rb");
	if(pFile == NULL)
	{
		perror("Error opening file");
		return -1;
	}
	else
	{
		fseek(pFile, 0, SEEK_END);
		// Get the length of sdr file
		*length = ftell(pFile);
		fclose(pFile);
	}
	return 0;
}

int ReadWriteEEPROM(char* EEPROMFileName, uint8_t* pData, uint16_t Offset, uint16_t Size, uint8_t RWFlag)
{
    FILE* pFile;
    uint32_t    EEPROMFileSize = 0;

    if( RWFlag == WRITE_EEPROM )
    {
    	pFile = fopen(EEPROMFileName, "rb+");

        if(pFile == NULL)
        {
            perror("Error opening file");
            return -1;
        }

        if(-1 == fseek(pFile, Offset, SEEK_SET))
            printf("seek error\n");

        fwrite(pData, sizeof(uint8_t), Size, pFile);

        fclose(pFile);
    }
    else
    {
        GetFileLength(EEPROMFileName, &EEPROMFileSize);

        if(Offset > EEPROMFileSize)
        {
            return -1;
        }
        else if(Offset + Size > EEPROMFileSize)
        {
            Size = EEPROMFileSize - Offset;
        }

    	pFile = fopen(EEPROMFileName, "rb");

        if(pFile == NULL)
        {
            perror("Error opening file");
            return -1;
        }

        if(-1 == fseek(pFile, Offset, SEEK_SET))
            printf("seek error\n");

        fread(pData, sizeof(uint8_t), Size, pFile);

        fclose(pFile);
    }

    return 0;
}


