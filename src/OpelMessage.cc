#include <OpelUtil.h>
#include <OpelMessage.h>

#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

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
	if(0 != PACKET_TYPE_FILE & type){
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

	if(0 != PACKET_TYPE_FILE & type){
		memcpy(&buff[16], srcFName, FNAME_LEN);
		memcpy(&buff[40], destFName, FNAME_LEN);
		tmp_int = htonl(fsize);
		memcpy(&buff[64], &tmp_int, 4);
		tmp_int = htonl(offset);
		memcpy(&tmp_int, &buff[68], 4);
	}

	return true;
}

OpelMessage::OpelMessage()
{
	data = NULL;
}
OpelMessage::~OpelMessage()
{
	if(NULL != data)
		free(data);
}

OpelMessage& OpelMessage::operator=(OpelMessage &arg)
{
	if(this != &arg){
		if(data != NULL){
			free(data);
			data = NULL;
		}

		op_header = arg.op_header;
		op_socket = arg.op_socket;
		if(arg.data != NULL){
			setData(arg.getData(), arg.getDataLen());
		}
	}
}

bool OpelMessage::initFromBuff(uint8_t buff[])
{
	return op_header.initFromBuff(buff);
}

bool OpelMessage::initToBuff(uint8_t buff[])
{
	return op_header.initToBuff(buff);
}

uint32_t OpelMessage::getReqId()
{
	if(false == op_header.initialized){
		comm_log("Not initialized, but %s invoked", __func__);
		return 0;
	}
	return op_header.req_id;
}

uint32_t OpelMessage::getDataLen()
{
	if(false == op_header.initialized){
		comm_log("Not initialized, but %s invoked", __func__);
		return 0;
	}
	return op_header.data_len;
}

uint16_t OpelMessage::getType()
{
	if(false == op_header.initialized){
		comm_log("Not initialized, but %s invoked", __func__);
		return 0;
	}
	return op_header.type;
}
uint16_t OpelMessage::getErr()
{
	if(false == op_header.initialized){
		comm_log("Not initialized, but %s invoked", __func__);
		return 0;
	}
	return op_header.err;
}
uint32_t OpelMessage::getChksum()
{
	if(false == op_header.initialized){
		comm_log("Not initialized, but %s invoked", __func__);
		return 0;
	}
	return op_header.chksum;
}
char *OpelMessage::getSrcFName()
{
	if(false == op_header.initialized || (op_header.type & PACKET_TYPE_FILE) == 0){
		comm_log("Not initialized, but %s invoked", __func__);
		return NULL;
	}
	return op_header.srcFName;
}
char *OpelMessage::getDestFName()
{
	if(false == op_header.initialized || (op_header.type & PACKET_TYPE_FILE) == 0 ){
		comm_log("Not initialized, but %s invoked", __func__);
		return NULL;
	}
	return op_header.destFName;
}

uint32_t OpelMessage::getFSize()
{
	if(false == op_header.initialized || (op_header.type & PACKET_TYPE_FILE) == 0){
		comm_log("Not initialized, but %s invoked", __func__);
		return 0;
	}
	return op_header.fsize;
}

uint32_t OpelMessage::getOffset()
{
	if(false == op_header.initialized || (op_header.type & PACKET_TYPE_FILE) == 0){
		comm_log("Not initialized, but %s invoked", __func__);
		return 0;
	}
	return op_header.offset;
}

uint8_t *OpelMessage::getData()
{
	if(false == op_header.initialized){
		comm_log("Not initialized, but %s invoked", __func__);
		return NULL;
	}
	return data;
}
OpelSocket* OpelMessage::getSocket()
{
	if(false == op_header.initialized){
		comm_log("Not initialized, but %s invoked", __func__);
	}
	return op_socket;
}

void OpelMessage::setReqid(uint32_t arg)
{
	op_header.req_id = arg;
}
void OpelMessage::setDataLen(uint32_t len)
{
	op_header.data_len = len;
}
void OpelMessage::setType(uint16_t type)
{
	op_header.type = type;
	op_header.initialized = true;
}
void OpelMessage::setErr(uint16_t err)
{
	op_header.err = err;
}
void OpelMessage::setChksum(uint32_t chksum)
{
	op_header.chksum = chksum;
}

void OpelMessage::setSrcFName(char fname[])
{
	strncpy(op_header.srcFName, fname, FNAME_LEN);
}
void OpelMessage::setDestFName(char fname[])
{
	strncpy(op_header.destFName, fname, FNAME_LEN);
}
void OpelMessage::setFSize(uint32_t size)
{
	op_header.fsize = size;
}
void OpelMessage::setOffset(uint32_t offset)
{
	op_header.offset = offset;
}
void OpelMessage::setData(uint8_t data[], uint32_t len)
{
	if(NULL != this->data){
		free(this->data);
		this->data = NULL;
	}
	if(NULL != data){
		this->data = (uint8_t *)malloc(len);
		memcpy(this->data, data, len);
		this->setDataLen(len);
	}
	op_header.initialized = true;
}
void OpelMessage::setSocket(OpelSocket *op_sock)
{
	this->op_socket = op_sock;
	op_header.initialized = true;
}
void OpelMessage::setHeader(OpelHeader &op_head)
{
	op_header = op_head;
}

