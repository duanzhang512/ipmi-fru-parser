#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <fcntl.h>
#include <string.h>


int GetFileLength(char *fileName, unsigned long *length)
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


