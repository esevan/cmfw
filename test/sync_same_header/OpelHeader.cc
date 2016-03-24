#include "OpelHeader.h"

#include <string.h>
#include <arpa/inet.h>
OpelHeader::OpelHeader()
{
	req_id = 0;
	data_len = 0;
	type = 0;
	chksum = 0;
	srcFName[0] = '\0';
	destFName[0] = '\0';
	fsize = 0;
	offset = 0;
	initialized = false;
}

OpelHeader& OpelHeader::operator=(OpelHeader &arg)
{
	if(this != &arg){
		req_id = arg.req_id;
		data_len = arg.data_len;
		type = arg.type;
		chksum = arg.chksum;
		strncpy(srcFName, arg.srcFName, FNAME_LEN);
		strncpy(destFName, arg.destFName, FNAME_LEN);
		fsize = arg.fsize;
		offset = arg.offset;
		initialized = arg.initialized;
	}

	return *this;
}

bool OpelHeader::initFromBuff(uint8_t buff[]){
	if(NULL == buff)
		return false;

	uint32_t tmp_int;
	uint16_t tmp_short;

	memcpy(&tmp_int, &buff[0], 4);
	req_id = ntohl(tmp_int);
	memcpy(&tmp_int, &buff[4], 4);
	data_len = ntohl(tmp_int);
	memcpy(&tmp_short, &buff[8], 2);
	type = ntohs(tmp_short);
	memcpy(&tmp_short, &buff[10], 2);
	err = ntohs(tmp_short);
	memcpy(&tmp_int, &buff[12], 4);
	chksum = ntohl(tmp_int);
	if(0 != (PACKET_TYPE_FILE & type)){
		memcpy(srcFName, &buff[16], FNAME_LEN);
		memcpy(destFName, &buff[40], FNAME_LEN);
		memcpy(&tmp_int, &buff[64], 4);
		fsize = ntohl(tmp_int);
		memcpy(&tmp_int, &buff[68], 4);
		offset = ntohl(tmp_int);
	}

	initialized = true;

	return true;
}

bool OpelHeader::initToBuff(uint8_t buff[]){
	if(NULL == buff)
		return false;

	uint32_t tmp_int;
	uint16_t tmp_short;

	tmp_int = htonl(req_id);
	memcpy(&buff[0], &tmp_int, 4);
	tmp_int = htonl(data_len);
	memcpy(&buff[4], &tmp_int, 4);
	tmp_short = htons(type);
	memcpy(&buff[8], &tmp_short, 2);
	tmp_short = htons(err);
	memcpy(&buff[10], &tmp_short, 2);
	tmp_int = htonl(chksum);
	memcpy(&buff[12], &tmp_int, 4);

	if(0 != (PACKET_TYPE_FILE & type)){
		memcpy(&buff[16], srcFName, FNAME_LEN);
		memcpy(&buff[40], destFName, FNAME_LEN);
		tmp_int = htonl(fsize);
		memcpy(&buff[64], &tmp_int, 4);
		tmp_int = htonl(offset);
		memcpy(&buff[68], &tmp_int, 4);
	}

	return true;
}
