#ifndef __PDKFRU_H__
#define __PDKFRU_H__

#include <inttypes.h>

#define HAVE_PRAGMA_PACK 1
#define HAVE_BYTESWAP_H 1

#define BUF2STR_MAXIMUM_OUTPUT_SIZE	(3*1024 + 1)

#if HAVE_BYTESWAP_H
# include <byteswap.h>
# define BSWAP_16(x) bswap_16(x)
# define BSWAP_32(x) bswap_32(x)
#else
# define BSWAP_16(x) ((((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8))
# define BSWAP_32(x) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) |\
                     (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24))
#endif


enum {
	FRU_CHASSIS_PARTNO,
	FRU_CHASSIS_SERIAL,
	FRU_BOARD_MANUF,
	FRU_BOARD_PRODUCT,
	FRU_BOARD_SERIAL,
	FRU_BOARD_PARTNO,
	FRU_PRODUCT_MANUF,
	FRU_PRODUCT_NAME,
	FRU_PRODUCT_PARTNO,
	FRU_PRODUCT_VERSION,
	FRU_PRODUCT_SERIAL,
	FRU_PRODUCT_ASSET,
};

struct fru_info {
	uint16_t size;
	uint8_t access:1;
	uint8_t max_read_size;
	uint8_t max_write_size;
};

#ifdef HAVE_PRAGMA_PACK
#pragma pack(1)
#endif
struct fru_header {
	uint8_t version;
	union {
		struct {
			uint8_t internal;
			uint8_t chassis;
			uint8_t board;
			uint8_t product;
			uint8_t multi;
		} offset;
		uint8_t offsets[5];
	};
	uint8_t pad;
	uint8_t checksum;
};
#ifdef HAVE_PRAGMA_PACK
#pragma pack(0)
#endif

#ifdef HAVE_PRAGMA_PACK
#pragma pack(1)
#endif
struct fru_multirec_header {
#define FRU_RECORD_TYPE_POWER_SUPPLY_INFORMATION 0x00
#define FRU_RECORD_TYPE_DC_OUTPUT 0x01
#define FRU_RECORD_TYPE_DC_LOAD 0x02
#define FRU_RECORD_TYPE_MANAGEMENT_ACCESS 0x03
#define FRU_RECORD_TYPE_BASE_COMPATIBILITY 0x04
#define FRU_RECORD_TYPE_EXTENDED_COMPATIBILITY 0x05
#define FRU_RECORD_TYPE_OEM_EXTENSION	0xc0
	uint8_t type;
	uint8_t format;
	uint8_t len;
	uint8_t record_checksum;
	uint8_t header_checksum;
};
#ifdef HAVE_PRAGMA_PACK
#pragma pack(0)
#endif

#ifdef HAVE_PRAGMA_PACK
#pragma pack(1)
#endif
struct fru_multirec_powersupply {
#if WORDS_BIGENDIAN
	uint16_t capacity;
#else
	uint16_t capacity:12;
	uint16_t __reserved1:4;
#endif
	uint16_t peak_va;
	uint8_t inrush_current;
	uint8_t inrush_interval;
	uint16_t lowend_input1;
	uint16_t highend_input1;
	uint16_t lowend_input2;
	uint16_t highend_input2;
	uint8_t lowend_freq;
	uint8_t highend_freq;
	uint8_t dropout_tolerance;
#if WORDS_BIGENDIAN
	uint8_t __reserved2:3;
	uint8_t tach:1;
	uint8_t hotswap:1;
	uint8_t autoswitch:1;
	uint8_t pfc:1;
	uint8_t predictive_fail:1;
#else
	uint8_t predictive_fail:1;
	uint8_t pfc:1;
	uint8_t autoswitch:1;
	uint8_t hotswap:1;
	uint8_t tach:1;
	uint8_t __reserved2:3;
#endif
	uint16_t peak_cap_ht;
#if WORDS_BIGENDIAN
	uint8_t combined_voltage1:4;
	uint8_t combined_voltage2:4;
#else
	uint8_t combined_voltage2:4;
	uint8_t combined_voltage1:4;
#endif
	uint16_t combined_capacity;
	uint8_t rps_threshold;
};
#ifdef HAVE_PRAGMA_PACK
#pragma pack(0)
#endif

#ifdef HAVE_PRAGMA_PACK
#pragma pack(1)
#endif
struct fru_multirec_dcoutput {
#if WORDS_BIGENDIAN
	uint8_t standby:1;
	uint8_t __reserved:3;
	uint8_t output_number:4;
#else
	uint8_t output_number:4;
	uint8_t __reserved:3;
	uint8_t standby:1;
#endif
	short nominal_voltage;
	short max_neg_dev;
	short max_pos_dev;
	uint16_t ripple_and_noise;
	uint16_t min_current;
	uint16_t max_current;
};
#ifdef HAVE_PRAGMA_PACK
#pragma pack(0)
#endif

#ifdef HAVE_PRAGMA_PACK
#pragma pack(1)
#endif
struct fru_multirec_dcload {
#if WORDS_BIGENDIAN
	uint8_t __reserved:4;
	uint8_t output_number:4;
#else
	uint8_t output_number:4;
	uint8_t __reserved:4;
#endif
	short nominal_voltage;
	short min_voltage;
	short max_voltage;
	uint16_t ripple_and_noise;
	uint16_t min_current;
	uint16_t max_current;
};
#ifdef HAVE_PRAGMA_PACK
#pragma pack(0)
#endif

#ifdef HAVE_PRAGMA_PACK
#pragma pack(1)
#endif
struct fru_multirec_oem_header {
	unsigned char mfg_id[3];
#define FRU_PICMG_BACKPLANE_P2P			0x04
#define FRU_PICMG_ADDRESS_TABLE			0x10
#define FRU_PICMG_SHELF_POWER_DIST		0x11
#define FRU_PICMG_SHELF_ACTIVATION		0x12
#define FRU_PICMG_SHMC_IP_CONN			0x13
#define FRU_PICMG_BOARD_P2P				0x14
#define FRU_AMC_CURRENT					0x16
#define FRU_AMC_ACTIVATION				0x17
#define FRU_AMC_CARRIER_P2P				0x18
#define FRU_AMC_P2P						0x19
#define FRU_AMC_CARRIER_INFO			0x1a
#define FRU_UTCA_FRU_INFO_TABLE			0x20
#define FRU_UTCA_CARRIER_MNG_IP			0x21
#define FRU_UTCA_CARRIER_INFO			0x22
#define FRU_UTCA_CARRIER_LOCATION		0x23
#define FRU_UTCA_SHMC_IP_LINK			0x24
#define FRU_UTCA_POWER_POLICY			0x25
#define FRU_UTCA_ACTIVATION				0x26
#define FRU_UTCA_PM_CAPABILTY			0x27
#define FRU_UTCA_FAN_GEOGRAPHY			0x28
#define FRU_UTCA_CLOCK_MAPPING			0x29
#define FRU_UTCA_MSG_BRIDGE_POLICY		0x2A
#define FRU_UTCA_OEM_MODULE_DESC		0x2B
#define FRU_PICMG_CLK_CARRIER_P2P		0x2C
#define FRU_PICMG_CLK_CONFIG			0x2D
	unsigned char record_id;
	unsigned char record_version;
};
#ifdef HAVE_PRAGMA_PACK
#pragma pack(0)
#endif

struct valstr {
	uint16_t val;
	const char * str;
};

/* FRU Board manufacturing date */
static const uint64_t secs_from_1970_1996 = 820454400;
static const char * chassis_type_desc[] __attribute__((unused)) = {
	"Unspecified", "Other", "Unknown",
	"Desktop", "Low Profile Desktop", "Pizza Box",
	"Mini Tower", "Tower",
	    "Portable", "LapTop", "Notebook", "Hand Held",
	    "Docking Station", "All in One", "Sub Notebook",
	    "Space-saving", "Lunch Box", "Main Server Chassis",
	    "Expansion Chassis", "SubChassis", "Bus Expansion Chassis",
	    "Peripheral Chassis", "RAID Chassis", "Rack Mount Chassis",
	    "Sealed-case PC", "Multi-system Chassis", "CompactPCI",
	    "AdvancedTCA", "Blade", "Blade Enclosure"
};

extern int ipmi_fru_print(char *fruFileName, uint8_t verbose);

#endif

