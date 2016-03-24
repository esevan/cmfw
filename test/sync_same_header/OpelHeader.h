#ifndef _opel_header_h_
#define _opel_header_h_

#define FNAME_LEN 24
#define COMM_HEADER_SIZE 72
#define PACKET_TYPE_MSG		1
#define PACKET_TYPE_FILE	2
//#define PACKET_TYPE_ACK		4
#define PACKET_TYPE_SPE		8

#include <stddef.h>
#include <stdint.h>
class OpelHeader
{
	public:
		uint32_t req_id;
		uint32_t data_len;
		uint16_t type;
		uint16_t err;
		uint32_t chksum;

		char srcFName[FNAME_LEN];
		char destFName[FNAME_LEN];
		uint32_t fsize;
		uint32_t offset;
		
		bool initialized;

		OpelHeader();
		OpelHeader& operator=(OpelHeader& arg);		

		bool initFromBuff(uint8_t buff[]);
		bool initToBuff(uint8_t buff[]);
};
#endif
