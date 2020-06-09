#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "ipmi_fru.h"
#include "PDKEEPROM.h"
#include "iniparser.h"
#include "parse-ex.h"

#define FRU_MULTIREC_CHUNK_SIZE     (255 + sizeof(struct fru_multirec_header))


const char * val2str(uint16_t val, const struct valstr *vs)
{
	static char un_str[32];
	int i;

	for (i = 0; vs[i].str != NULL; i++) {
		if (vs[i].val == val)
			return vs[i].str;
	}

	memset(un_str, 0, 32);
	snprintf(un_str, 32, "Unknown (0x%02X)", val);

	return un_str;
}

/* These values are IANA numbers */
/************************************************************************
* Add ID String for IANA Enterprise Number of IBM & ADLINK
* https://www.iana.org/assignments/enterprise-numbers/enterprise-numbers
************************************************************************/

typedef enum IPMI_OEM {
     IPMI_OEM_UNKNOWN    = 0,
     /* 2 for [IBM] */
     IPMI_OEM_IBM_2      = 2,
     IPMI_OEM_HP         = 11,
     IPMI_OEM_SUN        = 42,
     IPMI_OEM_NOKIA      = 94,
     IPMI_OEM_BULL       = 107,
     IPMI_OEM_HITACHI_116 = 116,
     IPMI_OEM_NEC        = 119,
     IPMI_OEM_TOSHIBA    = 186,
     IPMI_OEM_ERICSSON   = 193,
     IPMI_OEM_INTEL      = 343,
     IPMI_OEM_TATUNG     = 373,
     IPMI_OEM_HITACHI_399 = 399,
     IPMI_OEM_DELL       = 674,
     IPMI_OEM_LMC        = 2168,
     IPMI_OEM_RADISYS    = 4337,
     IPMI_OEM_BROADCOM   = 4413,
     /* 4769 for [IBM Corporation] */
     IPMI_OEM_IBM_4769   = 4769,
     IPMI_OEM_MAGNUM     = 5593,
     IPMI_OEM_TYAN       = 6653,
     IPMI_OEM_QUANTA     = 7244,
     IPMI_OEM_NEWISYS    = 9237,
     IPMI_OEM_ADVANTECH  = 10297,
     IPMI_OEM_FUJITSU_SIEMENS = 10368,
     IPMI_OEM_AVOCENT    = 10418,
     IPMI_OEM_PEPPERCON  = 10437,
     IPMI_OEM_SUPERMICRO = 10876,
     IPMI_OEM_OSA        = 11102,
     IPMI_OEM_GOOGLE     = 11129,
     IPMI_OEM_CELESTICA  = 12290,
     IPMI_OEM_PICMG      = 12634,
     IPMI_OEM_RARITAN    = 13742,
     IPMI_OEM_KONTRON    = 15000,
     IPMI_OEM_PPS        = 16394,
     /* 20301 for [IBM eServer X] */
     IPMI_OEM_IBM_20301  = 20301,
     IPMI_OEM_AMI        = 20974,
     /* 24339 for [ADLINK TECHNOLOGY INC.] */
     IPMI_OEM_ADLINK_24339 = 24339,
     IPMI_OEM_NOKIA_SOLUTIONS_AND_NETWORKS = 28458,
     IPMI_OEM_VITA       = 33196,
     IPMI_OEM_SUPERMICRO_47488 = 47488
} IPMI_OEM;

const struct valstr ipmi_oem_info[] = {

   { IPMI_OEM_UNKNOWN,                "Unknown" },
   { IPMI_OEM_HP,                     "Hewlett-Packard" },
   { IPMI_OEM_SUN,                    "Sun Microsystems" },
   { IPMI_OEM_INTEL,                  "Intel Corporation" },
   { IPMI_OEM_LMC,                    "LMC" },
   { IPMI_OEM_RADISYS,                "RadiSys Corporation" },
   { IPMI_OEM_TYAN,                   "Tyan Computer Corporation" },
   { IPMI_OEM_NEWISYS,                "Newisys" },
   { IPMI_OEM_SUPERMICRO,             "Supermicro" },
   { IPMI_OEM_GOOGLE,                 "Google" },
   { IPMI_OEM_KONTRON,                "Kontron" },
   { IPMI_OEM_NOKIA,                  "Nokia" },
   { IPMI_OEM_PICMG,                  "PICMG" },
   { IPMI_OEM_PEPPERCON,              "Peppercon AG" },
   { IPMI_OEM_DELL,                   "DELL Inc" },
   { IPMI_OEM_NEC,                    "NEC" },
   { IPMI_OEM_MAGNUM,                 "Magnum Technologies" },
   { IPMI_OEM_FUJITSU_SIEMENS,        "Fujitsu Siemens" },
   { IPMI_OEM_TATUNG,                 "Tatung" },
   { IPMI_OEM_AMI,                    "AMI" },
   { IPMI_OEM_RARITAN,                "Raritan" },
   { IPMI_OEM_AVOCENT,                "Avocent" },
   { IPMI_OEM_OSA,                    "OSA" },
   { IPMI_OEM_TOSHIBA,                "Toshiba" },
   { IPMI_OEM_HITACHI_116,            "Hitachi" },
   { IPMI_OEM_HITACHI_399,            "Hitachi" },
   { IPMI_OEM_NOKIA_SOLUTIONS_AND_NETWORKS, "Nokia Solutions and Networks" },
   { IPMI_OEM_BULL,                   "Bull Company" },
   { IPMI_OEM_PPS,                    "Pigeon Point Systems" },
   { IPMI_OEM_BROADCOM,               "Broadcom Corporation" },
   { IPMI_OEM_ERICSSON,               "Ericsson AB"},
   { IPMI_OEM_CELESTICA,              "Celestica International Inc."},
   { IPMI_OEM_QUANTA,                 "Quanta" },
   { IPMI_OEM_VITA,                   "VITA" },
   { IPMI_OEM_ADVANTECH,              "Advantech" },
   /************************************************************************
    * Add ID String for IANA Enterprise Number of IBM & ADLINK
    * https://www.iana.org/assignments/enterprise-numbers/enterprise-numbers
    *  2
    *    IBM
    *      Kristine Adamson
    *        adamson&us.ibm.com
    *  4769
    *    IBM Corporation
    *      Victor Sample
    *        vsample&us.ibm.com
    *  20301
    *    IBM eServer X
    *      Lynn Fore
    *        sls&us.ibm.com
    *  24339
    *    ADLINK TECHNOLOGY INC.
    *      Ryan Hsu
    *        ryan.hsu&adlinktech.com
    ************************************************************************/
   { IPMI_OEM_IBM_2,                  "IBM" },
   { IPMI_OEM_IBM_4769,               "IBM Corporation" },
   { IPMI_OEM_IBM_20301,              "IBM eServer X" },
   { IPMI_OEM_ADLINK_24339,           "ADLINK Technology Inc." },
   { 0xffff , NULL },
};


/* buf2str_extended - convert sequence of bytes to hexadecimal string with
 * optional separator
 *
 * @param buf - data to convert
 * @param len - size of data
 * @param sep - optional separator (can be NULL)
 *
 * @returns     buf representation in hex, possibly truncated to fit
 *              allocated static memory
 */
const char *
buf2str_extended(const uint8_t *buf, int len, const char *sep)
{
	static char str[BUF2STR_MAXIMUM_OUTPUT_SIZE];
	char *cur;
	int i;
	int sz;
	int left;
	int sep_len;

	if (buf == NULL) {
		snprintf(str, sizeof(str), "<NULL>");
		return (const char *)str;
	}
	cur = str;
	left = sizeof(str);
	if (sep) {
		sep_len = strlen(sep);
	} else {
		sep_len = 0;
	}
	for (i = 0; i < len; i++) {
		/* may return more than 2, depending on locale */
		sz = snprintf(cur, left, "%2.2x", buf[i]);
		if (sz >= left) {
			/* buffer overflow, truncate */
			break;
		}
		cur += sz;
		left -= sz;
		/* do not write separator after last byte */
		if (sep && i != (len - 1)) {
			if (sep_len >= left) {
				break;
			}
			strncpy(cur, sep, left - sz);
			cur += sep_len;
			left -= sep_len;
		}
	}
	*cur = '\0';

	return (const char *)str;
}

const char *
buf2str(const uint8_t *buf, int len)
{
	return buf2str_extended(buf, len, NULL);
}


/* read_fru_area  -  fill in frubuf[offset:length] from the FRU[offset:length]
*
* @intf:   ipmi interface
* @fru: fru info
* @id:     fru id
* @offset: offset into buffer
* @length: how much to read
* @frubuf: buffer read into
*
* returns -1 on error
* returns 0 if successful
*/
int
read_fru_area(char *fruFileName, struct fru_info *fru,
			uint32_t offset, uint32_t length, uint8_t *frubuf)
{
	uint32_t finish;

	if (offset > fru->size) {
		printf("Read FRU Area offset incorrect: %d > %d\n",
			offset, fru->size);
		return -1;
	}

	finish = offset + length;
	if (finish > fru->size) {
		finish = fru->size;
		printf("Read FRU Area length %d too large, "
			"Adjusting to %d\n",
			offset + length, finish - offset);
	}

    ReadWriteEEPROM(fruFileName, frubuf, offset, length, READ_EEPROM);

    return 0;
}

/* get_fru_area_str  -  Parse FRU area string from raw data
*
* @data:   raw FRU data
* @offset: offset into data for area
*
* returns pointer to FRU area string
*/
char * get_fru_area_str(uint8_t * data, uint32_t * offset)
{
	static const char bcd_plus[] = "0123456789 -.:,_";
	char * str;
	int len, off, size, i, j, k, typecode;
	union {
		uint32_t bits;
		char chars[4];
	} u;

	size = 0;
	off = *offset;

	/* bits 6:7 contain format */
	typecode = ((data[off] & 0xC0) >> 6);

	// printf("Typecode:%i\n", typecode);
	/* bits 0:5 contain length */
	len = data[off++];
	len &= 0x3f;

	switch (typecode) {
	case 0:           /* 00b: binary/unspecified */
		/* hex dump -> 2x length */
		size = (len*2);
		break;
	case 2:           /* 10b: 6-bit ASCII */
		/* 4 chars per group of 1-3 bytes */
		size = ((((len+2)*4)/3) & ~3);
		break;
	case 3:           /* 11b: 8-bit ASCII */
	case 1:           /* 01b: BCD plus */
		/* no length adjustment */
		size = len;
		break;
	}

	if (size < 1) {
		*offset = off;
		return NULL;
	}
	str = malloc(size+1);
	if (str == NULL)
		return NULL;
	memset(str, 0, size+1);

	if (len == 0) {
		str[0] = '\0';
		*offset = off;
		return str;
	}

	switch (typecode) {
	case 0:        /* Binary */
		strncpy(str, buf2str(&data[off], len), len*2);
		break;

	case 1:        /* BCD plus */
		for (k=0; k<len; k++)
			str[k] = bcd_plus[(data[off+k] & 0x0f)];
		str[k] = '\0';
		break;

	case 2:        /* 6-bit ASCII */
		for (i=j=0; i<len; i+=3) {
			u.bits = 0;
			k = ((len-i) < 3 ? (len-i) : 3);
#if WORDS_BIGENDIAN
			u.chars[3] = data[off+i];
			u.chars[2] = (k > 1 ? data[off+i+1] : 0);
			u.chars[1] = (k > 2 ? data[off+i+2] : 0);
#define CHAR_IDX 3
#else
			memcpy((void *)&u.bits, &data[off+i], k);
#define CHAR_IDX 0
#endif
			for (k=0; k<4; k++) {
				str[j++] = ((u.chars[CHAR_IDX] & 0x3f) + 0x20);
				u.bits >>= 6;
			}
		}
		str[j] = '\0';
		break;

	case 3:
		memcpy(str, &data[off], len);
		str[len] = '\0';
		break;
	}

	off += len;
	*offset = off;

	return str;
}



/* fru_area_print_chassis  -  Print FRU Chassis Area
*
* @intf:   ipmi interface
* @fru: fru info
* @id:  fru id
* @offset: offset pointer
*/
static void fru_area_print_chassis(char *fruFileName, char *iniConfFile, struct fru_info * fru, uint32_t offset, uint8_t verbose)
{
	char * fru_area;
	uint8_t * fru_data;
	uint32_t fru_len, i;
	uint8_t tmp[2];

    INI_HANDLE ini;

    ini = IniLoadFile(iniConfFile);
    if (ini == NULL)
            return;

	fru_len = 0;

	/* read enough to check length field */
	if (read_fru_area(fruFileName, fru, offset, 2, tmp) == 0) {
		fru_len = 8 * tmp[1];
	}

	if (fru_len == 0) {
		return;
	}

	fru_data = malloc(fru_len);
	if (fru_data == NULL) {
		printf("ipmitool: malloc failure\n");
		return;
	}

	memset(fru_data, 0, fru_len);

	/* read in the full fru */
	if (read_fru_area(fruFileName, fru, offset, fru_len, fru_data) < 0) {
		free(fru_data);
		fru_data = NULL;
		return;
	}

	/*
	 * skip first two bytes which specify
	 * fru area version and fru area length
	 */
	i = 2;

	printf(" Chassis Type          : %s\n",
 		chassis_type_desc[fru_data[i] >
 		(sizeof(chassis_type_desc)/sizeof(chassis_type_desc[0])) - 1 ?
 		2 : fru_data[i]]);

    IniSetUInt(ini, "cia", "chassis_type", fru_data[i] >
 		(sizeof(chassis_type_desc)/sizeof(chassis_type_desc[0])) - 1 ?
 		2 : fru_data[i]);

 	i++;

	fru_area = get_fru_area_str(fru_data, &i);
	if (fru_area != NULL) {
		if (strlen(fru_area) > 0) {
			printf(" Chassis Part Number   : %s\n", fru_area);
            IniSetStr(ini, "cia", "part_number", fru_area);
		}
		free(fru_area);
		fru_area = NULL;
	}

	fru_area = get_fru_area_str(fru_data, &i);
	if (fru_area != NULL) {
		if (strlen(fru_area) > 0) {
			printf(" Chassis Serial        : %s\n", fru_area);
            IniSetStr(ini, "cia", "serial_number", fru_area);
		}
		free(fru_area);
		fru_area = NULL;
	}

    uint8_t custom_num = 0;

	/* read any extra fields */
	while ((fru_data[i] != 0xc1) && (i < fru_len))
	{
		int j = i;
		fru_area = get_fru_area_str(fru_data, &i);
		if (fru_area != NULL) {
			if (strlen(fru_area) > 0) {
				printf(" Chassis Extra_%d       : %s\n", (custom_num + 1), fru_area);

                char * custom_num_str;

                custom_num++;
                custom_num_str = malloc(20);
                if (custom_num_str == NULL) {
                    printf("custom_num_str malloc failure\n");
                    return;
                }

                sprintf((char *)custom_num_str, "chassis_custom_%d", custom_num);
                IniSetStr(ini, "cia", custom_num_str, fru_area);

                free(custom_num_str);
                custom_num_str = NULL;
			}
			free(fru_area);
			fru_area = NULL;
		}

		if (i == j) {
			break;
		}
	}

	if (fru_data != NULL) {
		free(fru_data);
		fru_data = NULL;
	}

    IniSaveFile(ini,iniConfFile);
    IniCloseFile(ini);
}

/* fru_area_print_board  -  Print FRU Board Area
*
* @intf:   ipmi interface
* @fru: fru info
* @id:  fru id
* @offset: offset pointer
*/
static void fru_area_print_board(char *fruFileName, char *iniConfFile, struct fru_info * fru, uint32_t offset, uint8_t verbose)
{
	char * fru_area;
	uint8_t * fru_data;
	uint32_t fru_len;
	uint32_t i;
	time_t tval;
	uint8_t tmp[2];

    INI_HANDLE ini;

    ini = IniLoadFile(iniConfFile);
    if (ini == NULL)
            return;

	fru_len = 0;

	/* read enough to check length field */
	if (read_fru_area(fruFileName, fru, offset, 2, tmp) == 0) {
		fru_len = 8 * tmp[1];
	}

	if (fru_len <= 0) {
		return;
	}

	fru_data = malloc(fru_len);
	if (fru_data == NULL) {
		printf("ipmitool: malloc failure\n");
		return;
	}

	memset(fru_data, 0, fru_len);

	/* read in the full fru */
	if (read_fru_area(fruFileName, fru, offset, fru_len, fru_data) < 0) {
		free(fru_data);
		fru_data = NULL;
		return;
	}

	/*
	 * skip first three bytes which specify
	 * fru area version, fru area length
	 * and fru board language
	 */
	i = 3;

	tval=((fru_data[i+2] << 16) + (fru_data[i+1] << 8) + (fru_data[i]));
	tval=tval * 60;
	tval=tval + secs_from_1970_1996;
	printf(" Board Mfg Date        : %s", asctime(localtime(&tval)));
    IniSetUInt(ini, "bia", "mfg_datetime", (fru_data[i+2] << 16) + (fru_data[i+1] << 8) + (fru_data[i]));
	i += 3;  /* skip mfg. date time */

	fru_area = get_fru_area_str(fru_data, &i);
	if (fru_area != NULL) {
		if (strlen(fru_area) > 0) {
			printf(" Board Mfg             : %s\n", fru_area);
            IniSetStr(ini, "bia", "manufacturer", fru_area);
		}
		free(fru_area);
		fru_area = NULL;
	}

	fru_area = get_fru_area_str(fru_data, &i);
	if (fru_area != NULL) {
		if (strlen(fru_area) > 0) {
			printf(" Board Product         : %s\n", fru_area);
            IniSetStr(ini, "bia", "product_name", fru_area);
		}
		free(fru_area);
		fru_area = NULL;
	}

	fru_area = get_fru_area_str(fru_data, &i);
	if (fru_area != NULL) {
		if (strlen(fru_area) > 0) {
			printf(" Board Serial          : %s\n", fru_area);
            IniSetStr(ini, "bia", "serial_number", fru_area);
		}
		free(fru_area);
		fru_area = NULL;
	}

	fru_area = get_fru_area_str(fru_data, &i);
	if (fru_area != NULL) {
		if (strlen(fru_area) > 0) {
			printf(" Board Part Number     : %s\n", fru_area);
            IniSetStr(ini, "bia", "part_number", fru_area);
		}
		free(fru_area);
		fru_area = NULL;
	}

	fru_area = get_fru_area_str(fru_data, &i);
	if (fru_area != NULL) {
		if (strlen(fru_area) > 0) {
			printf(" Board FRU ID          : %s\n", fru_area);
            IniSetStr(ini, "bia", "fru_file_id", fru_area);
		}
		free(fru_area);
		fru_area = NULL;
	}

    uint8_t custom_num = 0;

	/* read any extra fields */
	while ((fru_data[i] != 0xc1) && (i < fru_len))
	{
		int j = i;
		fru_area = get_fru_area_str(fru_data, &i);
		if (fru_area != NULL) {
			if (strlen(fru_area) > 0) {
				printf(" Board Extra_%d         : %s\n", (custom_num + 1), fru_area);

                char * custom_num_str;

                custom_num++;
                custom_num_str = malloc(20);
                if (custom_num_str == NULL) {
                    printf("custom_num_str malloc failure\n");
                    return;
                }

                sprintf((char *)custom_num_str, "board_custom_%d", custom_num);
                IniSetStr(ini, "bia", custom_num_str, fru_area);

                free(custom_num_str);
                custom_num_str = NULL;
			}
			free(fru_area);
			fru_area = NULL;
		}
		if (i == j)
			break;
	}

	if (fru_data != NULL) {
		free(fru_data);
		fru_data = NULL;
	}

    IniSaveFile(ini,iniConfFile);
    IniCloseFile(ini);
}

/* fru_area_print_product  -  Print FRU Product Area
*
* @intf:   ipmi interface
* @fru: fru info
* @id:  fru id
* @offset: offset pointer
*/
static void
fru_area_print_product(char *fruFileName, char *iniConfFile, struct fru_info * fru, uint32_t offset, uint8_t verbose)
{
	char * fru_area;
	uint8_t * fru_data;
	uint32_t fru_len, i;
	uint8_t tmp[2];

    INI_HANDLE ini;

    ini = IniLoadFile(iniConfFile);
    if (ini == NULL)
            return;

	fru_len = 0;

	/* read enough to check length field */
	if (read_fru_area(fruFileName, fru, offset, 2, tmp) == 0) {
		fru_len = 8 * tmp[1];
	}

	if (fru_len == 0) {
		return;
	}

	fru_data = malloc(fru_len);
	if (fru_data == NULL) {
		printf("ipmitool: malloc failure\n");
		return;
	}

	memset(fru_data, 0, fru_len);


	/* read in the full fru */
	if (read_fru_area(fruFileName, fru, offset, fru_len, fru_data) < 0) {
		free(fru_data);
		fru_data = NULL;
		return;
	}

	/*
	 * skip first three bytes which specify
	 * fru area version, fru area length
	 * and fru board language
	 */
	i = 3;

	fru_area = get_fru_area_str(fru_data, &i);
	if (fru_area != NULL) {
		if (strlen(fru_area) > 0) {
			printf(" Product Manufacturer  : %s\n", fru_area);
            IniSetStr(ini, "pia", "manufacturer", fru_area);
		}
		free(fru_area);
		fru_area = NULL;
	}

	fru_area = get_fru_area_str(fru_data, &i);
	if (fru_area != NULL) {
		if (strlen(fru_area) > 0) {
			printf(" Product Name          : %s\n", fru_area);
            IniSetStr(ini, "pia", "product_name", fru_area);
		}
		free(fru_area);
		fru_area = NULL;
	}

	fru_area = get_fru_area_str(fru_data, &i);
	if (fru_area != NULL) {
		if (strlen(fru_area) > 0) {
			printf(" Product Part Number   : %s\n", fru_area);
            IniSetStr(ini, "pia", "part_number", fru_area);
		}
		free(fru_area);
		fru_area = NULL;
	}

	fru_area = get_fru_area_str(fru_data, &i);
	if (fru_area != NULL) {
		if (strlen(fru_area) > 0) {
			printf(" Product Version       : %s\n", fru_area);
            IniSetStr(ini, "pia", "version", fru_area);
		}
		free(fru_area);
		fru_area = NULL;
	}

	fru_area = get_fru_area_str(fru_data, &i);
	if (fru_area != NULL) {
		if (strlen(fru_area) > 0) {
			printf(" Product Serial        : %s\n", fru_area);
            IniSetStr(ini, "pia", "serial_number", fru_area);
		}
		free(fru_area);
		fru_area = NULL;
	}

	fru_area = get_fru_area_str(fru_data, &i);
	if (fru_area != NULL) {
		if (strlen(fru_area) > 0) {
			printf(" Product Asset Tag     : %s\n", fru_area);
            IniSetStr(ini, "pia", "asset_tag", fru_area);
		}
		free(fru_area);
		fru_area = NULL;
	}

	fru_area = get_fru_area_str(fru_data, &i);
	if (fru_area != NULL) {
		if (strlen(fru_area) > 0) {
			printf(" Product FRU ID        : %s\n", fru_area);
            IniSetStr(ini, "pia", "fru_file_id", fru_area);
		}
		free(fru_area);
		fru_area = NULL;
	}

    uint8_t custom_num = 0;

	/* read any extra fields */
	while ((fru_data[i] != 0xc1) && (i < fru_len))
	{
		int j = i;
		fru_area = get_fru_area_str(fru_data, &i);
		if (fru_area != NULL) {
			if (strlen(fru_area) > 0) {
				printf(" Product Extra_%d       : %s\n", (custom_num + 1), fru_area);

                char * custom_num_str;

                custom_num++;
                custom_num_str = malloc(20);
                if (custom_num_str == NULL) {
                    printf("custom_num_str malloc failure\n");
                    return;
                }

                sprintf((char *)custom_num_str, "product_custom_%d", custom_num);
                IniSetStr(ini, "pia", custom_num_str, fru_area);

                free(custom_num_str);
                custom_num_str = NULL;
			}
			free(fru_area);
			fru_area = NULL;
		}
		if (i == j)
			break;
	}

	if (fru_data != NULL) {
		free(fru_data);
		fru_data = NULL;
	}

    IniSaveFile(ini,iniConfFile);
    IniCloseFile(ini);
}

int ipmi_fru_print(char *fruFileName, char *iniConfFile, uint8_t verbose)
{
	struct fru_info fru;
	struct fru_header header;

	memset(&fru, 0, sizeof(struct fru_info));
	memset(&header, 0, sizeof(struct fru_header));

	memset(&fru, 0, sizeof(fru));

    uint32_t len;
	GetFileLength(fruFileName, &len);
    fru.size = (uint16_t) len;
	fru.access = 0; // Device is accessed by bytes

    if(verbose > 0)
    	printf("fru.size = %d bytes (accessed by %s)\n",
	    	fru.size, fru.access ? "words" : "bytes");

	if (fru.size < 1) {
		printf(" Invalid FRU size %d\n", fru.size);
		return -1;
	}

    uint8_t Data[8];
    ReadWriteEEPROM(fruFileName, Data, 0, sizeof(struct fru_header), READ_EEPROM);

	memcpy(&header, Data, 8);

	if (header.version != 1) {
		printf(" Unknown FRU header version 0x%02x\n",
			header.version);
		return -1;
	}

	/* offsets need converted to bytes
	* but that conversion is not done to the structure
	* because we may end up with offset > 255
	* which would overflow our 1-byte offset field */
    if(verbose > 0)
    {
    	printf("fru.header.version:         0x%02x\n", header.version);
    	printf("fru.header.offset.internal: 0x%02x\n", header.offset.internal * 8);
    	printf("fru.header.offset.chassis:  0x%02x\n", header.offset.chassis * 8);
    	printf("fru.header.offset.board:    0x%02x\n", header.offset.board * 8);
    	printf("fru.header.offset.product:  0x%02x\n", header.offset.product * 8);
    	printf("fru.header.offset.multi:    0x%02x\n", header.offset.multi * 8);
    }
	/*
	* rather than reading the entire part
	* only read the areas we'll format
	*/
	/* chassis area */
	if ((header.offset.chassis*8) >= sizeof(struct fru_header))
		fru_area_print_chassis(fruFileName, iniConfFile, &fru, header.offset.chassis*8, verbose);

	/* board area */
	if ((header.offset.board*8) >= sizeof(struct fru_header))
		fru_area_print_board(fruFileName, iniConfFile, &fru, header.offset.board*8, verbose);

	/* product area */
	if ((header.offset.product*8) >= sizeof(struct fru_header))
		fru_area_print_product(fruFileName, iniConfFile, &fru, header.offset.product*8, verbose);

	return 0;
}

