#ifndef __OPEL_MESSAGE_H__
#define __OPEL_MESSAGE_H__
	
#define PACKET_TYPE_MSG		1
#define PACKET_TYPE_FILE	2
//#define PACKET_TYPE_ACK		4
#define PACKET_TYPE_SPE		8

#define FNAME_LEN 24

#define COMM_HEADER_SIZE 72

#include <stdint.h>

#include <OpelSocket.h>

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
class OpelMessage
{
	private:
		OpelHeader op_header;
		OpelSocket* op_socket;
		
		uint8_t* data;

	public:
		OpelMessage();
		~OpelMessage();

		OpelMessage& operator=(OpelMessage& arg);

		bool initFromBuff(uint8_t buff[]);
		bool initToBuff(uint8_t buff[]);
		
		uint32_t getReqId();
		uint32_t getDataLen();
		uint16_t getType();
		uint16_t getErr();
		uint32_t getChksum();
		char *getSrcFName();
		char *getDestFName();
		uint32_t getFSize();
		uint32_t getOffset();
		uint8_t *getData();
		OpelSocket *getSocket();

		bool isMsg();
		bool isFile();
		bool isLastDataOfFile();

		void setReqid(uint32_t arg);
		void setDataLen(uint32_t len);
		void setType(uint16_t type);
		void setErr(uint16_t err);
		void setChksum(uint32_t chksum);

		void setSrcFName(char fname[]);
		void setDestFName(char fname[]);
		void setFSize(uint32_t size);
		void setOffset(uint32_t offset);
		void setData(uint8_t data[], uint32_t len);
		void setSocket(OpelSocket *op_sock);
		void setHeader(OpelHeader &op_head);
};
#endif
