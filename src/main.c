#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "eeprom.h"

#define TOOL_VERSION		"0.3"

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

static const char *optString = "f:v:n:V";

int main(int argc, char **argv)
{
	char ch = 0, optEnd = -1;
	// Default SDR file name: nsu12a.bin
	char *fruFileName = "fru.bin";
    unsigned long fruFileLen = 0;

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

    GetFileLength(fruFileName, &fruFileLen);

    if(verbose > 1)
        printf("%s length: 0x%lx\n", fruFileName, fruFileLen);

	return 0;
}

