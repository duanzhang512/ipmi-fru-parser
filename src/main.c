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
    printf("\t-o        Specify offset of FRU file\n");
    printf("\t-s        Size to read/write FRU file\n");
    printf("\t-r        Read FRU file\n");
    printf("\t-w        Write FRU file\n");
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

static const char *optString = "f:o:s:rwv:V";

int main(int argc, char **argv)
{
	char ch = 0, optEnd = -1;
	// Default SDR file name: nsu12a.bin
	char *fruFileName = "fru.bin";
    uint32_t fruFileLen = 0;

    uint8_t Data[EEPROM_MAX_SIZE] = {0};

    uint16_t offset = 0;
    uint16_t size = 0;
    uint8_t rwflag;

    uint16_t i;


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

#if 0
    GetFileLength(fruFileName, &fruFileLen);
    if(verbose > 1)
        printf("%s length: 0x%x\n", fruFileName, fruFileLen);
#else
    if(0)
    {
        fruFileLen=fruFileLen;
    }
#endif
#if 1
    ReadWriteEEPROM(fruFileName, Data, offset, size, rwflag);

    if(rwflag == READ_EEPROM)
    {
        printf("Read:  ");
        for(i=0; i<size; i++)
           printf(" %02x", Data[i]);
        printf("\n");
    }
#endif

	return 0;
}

