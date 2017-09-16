#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "PDKEEPROM.h"

#define TOOL_VERSION		"0.0"

uint8_t verbose = 0;


// Show Help Information Of This Tool
void ShowHelp(char *str)
{
	printf("*************************************************************\n");
	fprintf(stdout, "*                SDR PARSER TOOL V%s                       *\n", TOOL_VERSION);
	printf("*                   Fred 2016/11/24                         *\n");
	printf("*************************************************************\n");
	printf("Usage: %s [OPTIONS...]\n", str);
	printf("OPTIONS:\n");
	printf("\t-f        Specify input FRU file\n");
    printf("\t-o        Specify offset of FRU file, hex\n");
    printf("\t-s        Size to read from FRU file, hex\n");
    printf("\t-r        Read FRU file\n");
    printf("\t-w        Write FRU file\n");
    printf("\t-d        Hex string data to write, de for 0xde\n");
	printf("\t-v        Verbose show multiple level message\n");
	printf("\t            0 -- Total record number\n");
	printf("\t            1 -- Check all record ID\n");
	printf("\t            2 -- Byte level parser\n");
	printf("\t            3 -- Bit level parser\n");
	printf("\t-h|?      This help text\n");
	printf("\t-V        Print version\n");
}

void ShowVersion(void)
{
	fprintf(stdout, "version %s\n", TOOL_VERSION);
}

int BinStringToRawData(uint8_t *array, char *string, uint16_t *len)
{
    uint8_t data[EEPROM_MAX_SIZE];
    uint16_t i;
    uint16_t str_len, array_len = 0;
    char temp;

    str_len = strlen(string);

    for(i=0; i<str_len; i++)
    {
        temp = *string++;
        if(temp >= '0' && temp <= '9')
        {
            data[array_len++] = temp - '0';
        }
        else if(temp >= 'a' && temp <= 'f')
        {
            data[array_len++] = temp - 'a' + 10;
        }
        else if(temp >= 'A' && temp <= 'F')
        {
            data[array_len++] = temp - 'A' + 10;
        }
    }

    if(verbose > 1)
    {
        printf("String to data: ");
        for(i=0; i<str_len; i++)
            printf(" %01x", data[i]);
        printf("\n");
        printf("array_len: %d\n", array_len);
    }

    for(i=0; i<array_len/2; i++)
    {
        *array++ = (data[2*i] << 4) + data[2*i+1];
    }

    *len = array_len / 2;

    return 0;
}

int PrintDataToChar(uint8_t dat)
{
    if((dat >= '0' && dat <= '9') || \
       (dat >= 'a' && dat <='z')  || \
       (dat >= 'A' && dat <='Z'))
        printf("%c", dat);
    else
        printf(".");

    return 0;
}

static const char *optString = "f:o:s:rwd:v:V";

int main(int argc, char **argv)
{
	char ch = 0, optEnd = -1;
	// Default SDR file name: nsu12a.bin
	char *fruFileName = "fru.bin";
    char *fruDataWrite;

    uint8_t Data[EEPROM_MAX_SIZE] = {0};

    uint16_t offset = 0;
    uint16_t size = 0;
    uint8_t rwflag;

	opterr = 0;

	if(argc == 1)
	{
		ShowHelp(argv[0]);
		exit(EXIT_SUCCESS);
	}

	while((ch = getopt(argc, argv, optString)) != optEnd)
	{
		switch(ch)
		{
			case 'f':
				fruFileName = optarg;
			break;
			case 'o':
				offset = strtol(optarg, NULL, 16);
			break;
			case 's':
				size = strtol(optarg, NULL, 16);
			break;
			case 'r':
				rwflag = READ_EEPROM;
			break;
			case 'd':
				fruDataWrite = optarg;
			break;
			case 'w':
				rwflag = WRITE_EEPROM;
			break;
			case 'v':
				verbose = strtol(optarg, NULL, 10);
			break;
			case 'V':
				ShowVersion();
				exit(EXIT_SUCCESS);
			break;
			default :
				ShowHelp(argv[0]);
				exit(EXIT_SUCCESS);
			break;
		}
	}

    if(rwflag == READ_EEPROM)
    {
        // ./bin/parser -f example/fru.bin -r -o 0 -s 256 -v 2

        uint32_t fruFileLen;

        GetFileLength(fruFileName, &fruFileLen);

        if(offset + size > fruFileLen)
        {
            if(offset > fruFileLen)
                size = 0;
            else
                size = fruFileLen - offset;
        }

        ReadWriteEEPROM(fruFileName, Data, offset, size, rwflag);

        uint16_t i;

        printf("Data read, size: %d\n", size);
        for(i=0;i<offset%16;i++)
            printf("   ");

        for(i=offset;i<size+offset;i++)
        {
            printf(" %02x", Data[i-offset]);

            if((i+1) % 8 == 0)
                printf(" ");

            if((i+1) % 16 == 0)
                printf("\n");
        }
        printf("\n");
    }
    else
    {
        // ./bin/parser -f example/fru.bin -w -o 2 -d "01 06" -v 2

        size = strlen(fruDataWrite);

        if(verbose > 1)
            printf("string: %s, size: %d\n", fruDataWrite, size);

        BinStringToRawData(Data, fruDataWrite, &size);

        if(verbose > 1)
        {
            printf("FRU file name: %s\n", fruFileName);
            printf("Data: ");
            uint16_t i;
            for(i=0;i<size;i++)
            {
                printf(" %02x", Data[i]);
            }
            printf("\n");
            printf("Offset: %02x\n", offset);
            printf("Size  : %02x\n", size);
        }

        ReadWriteEEPROM(fruFileName, Data, offset, size, rwflag);
    }

	return 0;
}

