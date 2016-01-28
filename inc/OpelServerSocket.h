#ifndef __OPEL_SERVER_SOCKET_H__
#define __OPEL_SERVER_SOCKET_H__

#include <OpelSocket.h>

#define MAX_CLIENTS 32

class OpelServerSocket
{
	private:
		uint8_t conn_type;
		char intf_name[MAX_INTF_LEN];
		int sock_fd;
		uint16_t stat;

		void *priv;

	public:
		OpelServerSocket();
		OpelServerSocket(char* intf_name, uint8_t conn_type);

		/* Return accepted socket (Dynamically Allocated, must be freed later)  */
		OpelSocket* Accept(int *err);
		void set(char *intf_name, uint8_t conn_type);
		int getFd();
		bool init();
		void Close();
};

#endif
