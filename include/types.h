/*
   header for different types
 */

#ifndef TFTP_TYPES
#define TFTP_TYPES 0

#pragma pack(1)

#define error_msg(str)  printf("Err: %s\n", str)
#define tostr(str) #str
#define DSIZE 512
typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef enum
{
	failure = 0,
	success
} status_t;

typedef enum
{
	T_RRQ = 1,
	T_WRQ,
	T_DATA,
	T_ACK,
	T_ERROR
} tftp_t;

typedef enum
{
	NOT_DEFINED = 0,
	FILE_NOT_FOUND,
	ACCESS_VIOLATION,
	DISK_FULL,
	ILLEGAL_TFTP_OPERATION,
	UNKNOWN_TRANSFER_ID,
	FILE_EXISTS,
	NO_SUCH_USER
} errpck_t;


typedef struct
{
	uint16 opcode;
	union
	{
		uint8 buff[2 + DSIZE];
		struct
		{
			union
			{
				uint16 blk_no;
				uint16 err_code;
			};
			uint8 data[DSIZE];
		};
	};
} tftp_pkt;


#endif
