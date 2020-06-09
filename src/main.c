#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "PDKEEPROM.h"
#include "ipmi_fru.h"

#define TOOL_VERSION		"0.1.2"

// Show Help Information Of This Tool
void ShowHelp(char *str)
{
	printf("*************************************************************\n");
	fprintf(stdout, "*                FRU BIN PARSER TOOL V%s                 *\n", TOOL_VERSION);
	printf("*************************************************************\n");
	printf("Usage: %s [OPTIONS...]\n", str);
	printf("OPTIONS:\n");
	printf("\t-f        Specify input FRU file\n");
	printf("\t-o        Specify output configure\n");
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

static const char *optString = "f:o:v:V";

int main(int argc, char **argv)
{
	char ch = 0, optEnd = -1;
	// Default SDR file name: nsu12a.bin
	char *fruFileName = "fru.bin";
    char *iniConfFile = "example/fru.conf";
    uint8_t verbose = 0;

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
				iniConfFile = optarg;
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

    ipmi_fru_print(fruFileName, iniConfFile, verbose);

	return 0;
}

