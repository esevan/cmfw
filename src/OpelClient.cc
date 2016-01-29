#include <OpelClient.h>

#include <string.h>
#include <stdlib.h>
#include <OpelUtil.h>

static void generic_connect_handler(uv_work_t *req);
static void after_connect_handler(uv_work_t *req, int status);

OpelClientMonitor::OpelClientMonitor(char *intf_name, uint8_t conn_type, OpelReadQueue *rq, bool *connected, OpelCommHandler statCb)
{
	strncpy(this->intf_name, intf_name, MAX_INTF_LEN);
	this->conn_type = conn_type;
	this->connected = connected;
	this->statCb = statCb;
	rqueue = rq;
	op_sock = new OpelSocket(intf_name, CONN_TYPE_BT);
	if(NULL == op_sock || op_sock->getFd() < 0){
		comm_log("Client socket failed to be created");
		return;
	}
	FD_SET(op_sock->getFd(), &readfds);
	max_fd = op_sock->getFd();
}
OpelClientMonitor::~OpelClientMonitor()
{
	op_sock->Close();
	delete op_sock;
}
bool OpelClientMonitor::Select()
{
	//When read thread is busy, then do busy waiting.
	while(rqueue->waitStat == false) {}
	fd_set rfs = readfds;
	FD_SET(op_sock->getFd(), &rfs);
	comm_log("Start select()");
	if(select(max_fd+1, &rfs, NULL, NULL, NULL)<0){
		comm_log("Select error:%s(%d)", strerror(errno), errno);

		return false;
	}

	rqueue->enqueue(op_sock);
	return true;
}
bool OpelClientMonitor::Connect()
{
	if(op_sock == NULL){
		comm_log("Opel socket has not been created");
		return false;
	}

	return (op_sock->Connect() == COMM_S_OK);
}
OpelSocket *OpelClientMonitor::getSocket()
{
	return op_sock;
}

//////////////////////////OpelClient Impelmentation
OpelClient::OpelClient(char *intf_name, OpelCommHandler defCb, OpelCommHandler statCb)
	:OpelSCModel(intf_name, defCb, statCb)
{
	ocm = new OpelClientMonitor(intf_name, CONN_TYPE_BT, &rqueue, &connected, statCb);
	uv_work_t *req = (uv_work_t *)malloc(sizeof(uv_work_t));
	priv = (void *)req;

	req->data = (void *)(ocm);

	SCModelStarted = false;
	connected = false;
}

OpelClient::~OpelClient()
{
	Stop();
	delete ocm;
}

bool OpelClient::Start()
{
	if(connected == false){
		if(SCModelStarted == false){
			SCModelStarted = OpelSCModel::Start();
		}
		else{
			comm_log("SCModel started, so skip this");
		}
		uv_queue_work(uv_default_loop(), (uv_work_t *)priv, generic_connect_handler, after_connect_handler);
	}
	else{
		comm_log("Already connected");
		return false;
	}

	return true;
}


bool OpelClient::Stop()
{
	uv_cancel((uv_req_t *)priv);
	connected = false;
	OpelSCModel::Stop();
	SCModelStarted = false;
}

static void generic_connect_handler(uv_work_t *req)
{
	comm_log("Connect handler UP");
	OpelClientMonitor *ocm = (OpelClientMonitor *)req->data;

	if(*(ocm->connected) == false){
		*(ocm->connected) = ocm->Connect();
		return;
	}
	
	while(ocm->Select()){}

	return;
}

static void after_connect_handler(uv_work_t *req, int status)
{
	comm_log("Connect handler terminated");
	OpelClientMonitor *ocm = (OpelClientMonitor *)req->data;

	if(status == UV_ECANCELED)
		return;
	if(*(ocm->connected) == true){
		OpelMessage op_msg;

		ocm->statCb(&op_msg, STAT_CONNECTED);
		uv_queue_work(uv_default_loop(), req, generic_connect_handler, after_connect_handler);
	}
	else{
		ocm->statCb(NULL, STAT_DISCON);
	}
	return;
}

bool OpelClient::SendMsg(char *str)
{
	if(NULL == str){
		comm_log("Str == NULL");
		return false;
	}

	OpelMessage op_msg;
	OpelSocket *op_sock = ocm->getSocket();

	op_msg.setSocket(op_sock);
	op_msg.setData((uint8_t *)str, strlen(str)+1);
	op_msg.setType(PACKET_TYPE_MSG);

	return OpelSCModel::Send(&op_msg);
}

bool OpelClient::SendFile(char *srcFName, char *destFName)
{
	if(NULL == srcFName || NULL == destFName){
		comm_log("Invalid file name");
		return false;
	}

	OpelMessage op_msg;
	OpelSocket *op_sock = ocm->getSocket();

	op_msg.setSocket(op_sock);
	op_msg.setSrcFName(srcFName);
	op_msg.setDestFName(destFName);
	op_msg.setType(PACKET_TYPE_FILE);

	return OpelSCModel::Send(&op_msg);
}
