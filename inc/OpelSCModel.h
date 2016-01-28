#ifndef __OPEL_SC_MODEL_H__
#define __OPEL_SC_MODEL_H__

#include <list>

#include <uv.h>

#include <OpelMessage.h>
#include <OpelSocket.h>

typedef void (*OpelCommHandler)(OpelMessage *, uint16_t);

class OpelCommQueue
{
	private:
		std::list<OpelMessage *> q;
		uv_mutex_t lock;
		uv_cond_t notEmpty;

	public:
		OpelCommQueue();
		~OpelCommQueue();

		bool waitStat;
		bool enqueue(OpelMessage *msg);
		bool dequeue(OpelMessage *msg);
		bool isEmpty();
};

class OpelReadQueue
{
	private:
		std::list<OpelSocket *> q;
		uv_mutex_t lock;
		uv_cond_t notEmpty;

	public:
		OpelReadQueue();
		~OpelReadQueue();

		bool waitStat;
		bool enqueue(OpelSocket *op_sock);
		OpelSocket *dequeue();
		bool isEmpty();
};

class OpelSCModel
{
	private:
		char intf_name[MAX_INTF_LEN];
		OpelCommHandler defCb;
		OpelCommHandler statCb;
	
		void *priv;

	protected:
		OpelReadQueue rqueue;
		OpelCommQueue mqueue;
		OpelCommQueue fqueue;

	public:
		OpelSCModel(char *intf_name, OpelCommHandler defCb, OpelCommHandler statCb);
		~OpelSCModel();
		bool Start();
		bool Stop();
		bool Send(OpelMessage *msg);

		//virtual OpelSocket *getSocket(uint16_t sock_id);
		//virtual void putSocket(uint16_t sock_id);
};

#endif
