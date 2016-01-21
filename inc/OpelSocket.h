#ifndef __OPEL_SOCKET_H__
#define __OPEL_SOCKET_H__

#define CONN_TYPE_BT 1
#define CONN_TYPE_WD 2
#define CONN_TYPE_WF 4

#define STAT_DISCON 1
#define STAT_CONNECTING 2
#define STAT_CONNECTED 3

#define BT_MAX_DAT_LEN 1008

#ifndef MAX_INTF_LEN
#define MAX_INTF_LEN 256
#endif

#include <stdint.h>
#include <sys/types.h>
#include <uv.h>

class OpelSocket
{
	private:
		uint8_t conn_type;
		char intf_name[MAX_INTF_LEN];
		int sock_fd;
		uint16_t sock_id;
		uint16_t stat;

	public:
		OpelSocket();
		OpelSocket(char* intf_name, uint8_t conn_type);
		
		OpelSocket& operator=(OpelSocket& arg);

		ssize_t Write(void *buff, size_t len);
		ssize_t Read(void *buff, size_t len);
		int Close();

		uint16_t Connect();
		//bool isConnected();

		int getFd();
		int getId();
		uint32_t getPayloadSize();
		uint16_t getStat();
		char *getPrivate();
		
		void setFd(int fd);
		void setId(uint16_t id);
};




#endif
