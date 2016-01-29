#ifndef __OPEL_SERVER_H__
#define __OPEL_SERVER_H__

#include <list>

#include <OpelServerSocket.h>
#include <OpelSCModel.h>

class OpelSocketList
{
	private:
		char intf_name[MAX_INTF_LEN];
		uint8_t conn_type;
		OpelServerSocket *op_server;
		std::list<OpelSocket *>sockets;
		//fd_set readfds;
		//int max_fd;

		OpelReadQueue *rqueue;

	public:
		OpelSocketList(char *intf_name, uint8_t conn_type, OpelReadQueue *rq, OpelCommHandler statCb);
		void Insert(OpelSocket *sock);
		bool Select();
		OpelSocket *getSocketById(uint16_t sock_id);
		OpelCommHandler statCb;
		bool accepted;
};

class OpelServer : public OpelSCModel
{
	private:
		OpelSocketList *osl;
		void *priv;	

		bool serverStarted;
		bool SCModelStarted;

	public:
		OpelServer(char *intf_name, OpelCommHandler defCb, OpelCommHandler statCb);
		~OpelServer();
		bool Start();
		bool Stop();
		bool SendMsg(char *str, int sock_id = 0);
		bool SendFile(char *srcFName, char *destFName, int sock_id = 0);
};


#endif
