#include <comm_bt.h>
#include <OpelUtil.h>
#include <OpelSocket.h>
#include <string.h>
#include <unistd.h>

OpelSocket::OpelSocket()
{
	conn_type = 0;
	intf_name[0] = '\0';
	sock_fd = 0;
	stat = 0;
}
OpelSocket::OpelSocket(char *intf_name, uint8_t conn_type)
{
	strncpy(this->intf_name, intf_name, MAX_INTF_LEN);
	this->conn_type = conn_type;
	sock_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
}

OpelSocket& OpelSocket::operator=(OpelSocket& arg)
{
	this->conn_type = arg.conn_type;
	strncpy(this->intf_name, arg.intf_name, MAX_INTF_LEN);
	this->sock_fd = arg.sock_fd;
	this->stat = arg.stat;

	return *this;
}

ssize_t OpelSocket::Write(void *buff, size_t len)
{
	if(len > getPayloadSize())
	{
		comm_log("Write error(Do not write too much %d>%d)", len, getPayloadSize());
		return 0;
	}

	return write(sock_fd, buff, len);
}
ssize_t OpelSocket::Read(void *buff, size_t len)
{
	if(len > getPayloadSize())
	{
		comm_log("Read error(Do not read too much %d>%d)", len, getPayloadSize());
		return 0;
	}

	return read(sock_fd, buff, len);
}
int OpelSocket::Close()
{
	return close(sock_fd);
}

uint16_t OpelSocket::Connect()
{
	int res = 0;
	switch(conn_type){
		case CONN_TYPE_BT:
			res = bt_connect((uint8_t *)intf_name, sock_fd);
			if(res < 0)
				res = COMM_E_FAIL;
			else
				res = COMM_S_OK;
			break;
		case CONN_TYPE_WD:
		case CONN_TYPE_WF:
		default:
			comm_log("Not supported yet");
			res = COMM_E_FAIL;
			break;
	}

	return res;
}

int OpelSocket::getFd()
{
	if(sock_fd < 0)
		return -1;
	return sock_fd;
}
uint32_t OpelSocket::getPayloadSize()
{
	uint32_t res = 0;
	switch(conn_type){
		case CONN_TYPE_BT:
			res = BT_MAX_DAT_LEN;
			break;
		case CONN_TYPE_WD:
		case CONN_TYPE_WF:
		default:
			comm_log("Not supported yet");
			break;
	}

	return res;
}
uint16_t OpelSocket::getStat()
{
	return stat;
}
char *OpelSocket::getPrivate()
{
	char *res = NULL;
	switch(conn_type){
		case CONN_TYPE_BT:
			//getpeername()
			break;
		case CONN_TYPE_WD:
		case CONN_TYPE_WF:
		default:
			comm_log("Not supported yet");
			break;
	}
	return res;
}

void OpelSocket::setId(uint16_t id)
{
	sock_id  = id;
}

void OpelSocket::setFd(int fd)
{
	sock_fd = fd;
}
