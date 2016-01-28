#ifndef __OPEL_CLIENT_H__
#define __OPEL_CLIENT_H__

#include <OpelSCModel.h>

class OpelClientMonitor
{
	private:
		char intf_name[MAX_INTF_LEN];
		uint8_t conn_type;
		OpelSocket *op_sock;
		fd_set readfds;
		int max_fd;

		OpelReadQueue *rqueue;

	public:
		OpelClientMonitor(char *intf_name, uint8_t conn_type, OpelReadQueue *rq, bool *connected, OpelCommHandler statCb);
		~OpelClientMonitor();
		OpelSocket *getSocket();
		bool Select();
		bool Connect();
		bool *connected;
		OpelCommHandler statCb;
};

class OpelClient : public OpelSCModel
{
	private:
		OpelClientMonitor *ocm;
		bool SCModelStarted;
		bool connected;
		void *priv;

	public:
		OpelClient(char *intf_name, OpelCommHandler defCb, OpelCommHandler statCb);
		~OpelClient();
		bool Start();
		bool Stop();
		bool SendMsg(char *str);
		bool SendFile(char *srcFName, char *destFName);
};
#endif
