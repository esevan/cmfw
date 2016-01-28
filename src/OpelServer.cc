#include <OpelServer.h>

#include <string.h>
#include <stdlib.h>
#include <OpelUtil.h>


static void generic_accept_handler(uv_work_t *req);
static void after_accept_handler(uv_work_t *req, int status);


/////////////////////////////OpelSocketList Implementation
OpelSocketList::OpelSocketList(char *intf_name, uint8_t conn_type, OpelReadQueue *rqueue)
{
	strncpy(this->intf_name, intf_name, MAX_INTF_LEN);
	this->conn_type = conn_type;
	op_server = new OpelServerSocket(intf_name, conn_type);
	if(op_server->init()){
		comm_log("OpelServer initialized");
	}
	else
		comm_log("Failed to initialize server");
	max_fd = op_server->getFd();
	FD_ZERO(&readfds);
	FD_SET(op_server->getFd(), &readfds);
	this->rqueue = rqueue;
}

void OpelSocketList::Insert(OpelSocket *sock)
{
	static uint16_t sock_id = 0;
	sock->setId(sock_id++);
	sockets.push_back(sock);
	FD_SET(sock->getFd(), &readfds);
	if(max_fd < sock->getFd()){
		max_fd = sock->getFd();
	}
	rqueue->enqueue(sock);
}

bool OpelSocketList::Select()
{
	//When read thread is busy, then do busy waiting.
	while(rqueue->waitStat == false) {}
	fd_set rfs = readfds;
	comm_log("Start select()");
	if(select(max_fd+1, &rfs, NULL, NULL, NULL) < 0){
		comm_log("Select error:%s(%d)", strerror(errno), errno);
		return false;
	}

	if(FD_ISSET(op_server->getFd(), &rfs)){
		int err;
		OpelSocket *os = op_server->Accept(&err);
		if(err != COMM_S_OK){
			comm_log("Accept failed");
			return false;
		}
		Insert(os);

		return true;
	}
	else{
		for(std::list<OpelSocket *>::iterator it = sockets.begin(); it != sockets.end(); it++){
			if(FD_ISSET((*it)->getFd(), &rfs)){
				rqueue->enqueue(*it);
			}
		}
	}
}

OpelSocket *OpelSocketList::getSocketById(uint16_t sock_id)
{
	OpelSocket *res = NULL;
	for(std::list<OpelSocket *>::iterator it = sockets.begin(); it!=sockets.end(); it++){
		if((*it)->getId() == sock_id){
			res = *it;
			break;
		}
	}

	return res;
}
////////////////////////////////////////////////////

///////////////////////////////////////////////////OpelServer Implementation
OpelServer::OpelServer(char *intf_name, OpelCommHandler defCb, OpelCommHandler statCb) : OpelSCModel(intf_name, defCb, statCb)
{
	osl = new OpelSocketList(intf_name, CONN_TYPE_BT, &rqueue);
	uv_work_t *req = (uv_work_t *)malloc(sizeof(uv_work_t));
	priv = (void *)req;
	req->data = (OpelSocketList *)osl;

	SCModelStarted = false;
	serverStarted = false;
}
OpelServer::~OpelServer()
{
	Stop();
	delete osl;
	uv_work_t *req = (uv_work_t *)priv;
	free (req);
}

bool OpelServer::Start()
{
	if(serverStarted == false){
		if(SCModelStarted == false){
			SCModelStarted = OpelSCModel::Start();
		}
		uv_work_t *req = (uv_work_t *)priv;
		uv_queue_work(uv_default_loop(), req, generic_accept_handler, after_accept_handler);
		serverStarted = true;
	}
	else{
		comm_log("Already started");
		return false;
	}

	return true;
}

bool OpelServer::Stop()
{
	uv_cancel((uv_req_t *)priv);
	serverStarted = false;
	OpelSCModel::Stop();
	SCModelStarted=  false;
}

static void generic_accept_handler(uv_work_t *req)
{		
	OpelSocketList *osl = (OpelSocketList *)req->data;
	comm_log("Accept_handler started");
	while(true){
		if(osl->Select())
			comm_log("Selected!");
		else
			comm_log("Accept failed");
	}
}

static void after_accept_handler(uv_work_t *req, int status)
{
	comm_log("Accept_handler terminated");
}

bool OpelServer::SendMsg(char *str, int sock_id)
{
	if(NULL == str){
		comm_log("str = NULL");
		return false;
	}
	OpelMessage op_msg;

	OpelSocket *op_sock = osl->getSocketById(sock_id);
	if(op_sock == NULL){
		comm_log("No sock_id %x exists", sock_id);
		return false;
	}
	op_sock->get();
	op_msg.setSocket(op_sock);
	op_msg.setData((uint8_t *)str, strlen(str)+1);
	op_msg.setType(PACKET_TYPE_MSG);
	
	return OpelSCModel::Send(&op_msg);
}

bool OpelServer::SendFile(char *srcFName, char *destFName, int sock_id)
{
	if(NULL == srcFName || NULL == destFName){
		comm_log("Invalid file name");
		return false;
	}
	OpelMessage op_msg; 

	op_msg.setSrcFName(srcFName);
	op_msg.setDestFName(destFName);
	op_msg.setSocket(osl->getSocketById(sock_id));
	op_msg.setType(PACKET_TYPE_FILE);

	return OpelSCModel::Send(&op_msg);
}
////////////////////////////////////////////////////////
