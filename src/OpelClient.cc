#include <OpelClient.h>

#include <string.h>
#include <stdlib.h>
#include <OpelUtil.h>

static void generic_connect_handler(uv_work_t *req);
static void after_connect_handler(uv_work_t *req, int status);

OpelClientMonitor::OpelClientMonitor(char *intf_name, uint8_t conn_type, OpelReadQueue *rq)
{
	strncpy(this->intf_name, intf_name, MAX_INTF_LEN);
	this->conn_type = conn_type;
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

//////////////////////////OpelClient Impelmentation
OpelClient::OpelClient(char *intf_name, OpelCommHandler defCb, OpelCommHandler statCb)
	:OpelSCModel(intf_name, defCb, statCb)
{
	ocm = new OpelClientMonitor(intf_name, CONN_TYPE_BT, &rqueue);
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
		connected = true;
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
	OpelClientMonitor *ocm = (OpelClientMonitor *)req->data;

	ocm->Connect();
	
	while(ocm->Select()){}

	return;
}

static void after_connect_handler(uv_work_t *req, int status)
{
	comm_log("Connect handler terminated");
	return;
}
