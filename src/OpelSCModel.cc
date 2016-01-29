#include <OpelSCModel.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include <OpelUtil.h>

#ifndef _OPEL_PARAM_SET_
#define _OPEL_PARAM_SET_
typedef struct _OpelParamSet {
	uv_work_t rreq;
	uv_work_t mreq;
	uv_work_t freq;
} OpelParamSet;
typedef struct _RParam {
	OpelReadQueue *orq;
	OpelMessage *op_msg;
	uint16_t stat;
	OpelCommHandler defCb;
	OpelCommHandler statCb;
} RParam;
typedef struct _WParam {
	OpelCommQueue *ocq;
	OpelCommHandler defCb;
	OpelMessage *op_msg;
	uint16_t stat;
} WParam;
#endif

using namespace std;

static void generic_read_handler(uv_work_t *req);
static void after_read_handler(uv_work_t *req, int stat);
static void generic_mwrite_handler(uv_work_t *req);
static void after_mwrite_handler(uv_work_t *req, int stat);
static void generic_fwrite_handler(uv_work_t *req);
static void after_fwrite_handler(uv_work_t *req, int stat);

////////////////////////////////////////////////OpelCommQueue Implementation
OpelCommQueue::OpelCommQueue()
{
	uv_mutex_init(&lock);
	uv_cond_init(&notEmpty);
	waitStat = false;
}
OpelCommQueue::~OpelCommQueue()
{
	while(!isEmpty()){
		dequeue(NULL);
	}
}
bool OpelCommQueue::enqueue(OpelMessage *msg)
{
	if(NULL == msg){
		comm_log("msg NULL");
		return false;
	}
	msg->getSocket()->get();
	OpelMessage *op_msg = new OpelMessage();
	if(NULL == op_msg){
		comm_log("Memory Allocation failed");
	}
	(*op_msg) = *msg;

	uv_mutex_lock(&lock);
	q.push_back(op_msg);
	if(waitStat){
		uv_cond_signal(&notEmpty);
		waitStat = false;
	}
	uv_mutex_unlock(&lock);

	return true;
}

bool OpelCommQueue::dequeue(OpelMessage *msg)
{
	uv_mutex_lock(&lock);
	if(q.empty()){
		if(msg == NULL){
			uv_mutex_unlock(&lock);
			return false;
		}
		else{
			waitStat = true;
			uv_cond_wait(&notEmpty, &lock);
			if(q.empty() || q.front() == NULL){
				uv_mutex_unlock(&lock);
				return false;
			}
		}
	}
	OpelMessage *op_msg = q.front();
	if(NULL == op_msg){
		comm_log("Invalid situation");
		uv_mutex_unlock(&lock);
		return false;
	}
	q.pop_front();
	if(NULL != msg)
		*msg = *op_msg;
	uv_mutex_unlock(&lock);

	delete op_msg;

	return true;
}

bool OpelCommQueue::isEmpty()
{
	bool res = false;
	uv_mutex_lock(&lock);
	res = q.empty();
	uv_mutex_unlock(&lock);
	return res;
}


//////////////////////////////////////////////////////////////////OpelReadQueue Implementation
OpelReadQueue::OpelReadQueue()
{
	uv_mutex_init(&lock);
	uv_cond_init(&notEmpty);
	waitStat = false;
}
OpelReadQueue::~OpelReadQueue()
{
	while(!isEmpty()){
		OpelSocket *op_sock = dequeue();
		op_sock->put();
	}
}
bool OpelReadQueue::enqueue(OpelSocket *op_sock)
{
	if(NULL == op_sock){
		comm_log("sock NULL");
		return false;
	}
	uv_mutex_lock(&lock);
	op_sock->get();
	q.push_back(op_sock);
	if(waitStat){
		uv_cond_signal(&notEmpty);
		waitStat = false;
	}
	uv_mutex_unlock(&lock);

	return true;
}
OpelSocket *OpelReadQueue::dequeue()
{
	OpelSocket *op_sock = NULL;
	uv_mutex_lock(&lock);
	if(q.empty()){
		waitStat = true;
		uv_cond_wait(&notEmpty, &lock);
		if(q.empty() || q.front() == NULL){
			uv_mutex_unlock(&lock);
			return NULL;
		}
	}

	op_sock = q.front();
	q.pop_front();
	uv_mutex_unlock(&lock);

	return op_sock;
}
bool OpelReadQueue::isEmpty()
{
	bool res = false;

	uv_mutex_lock(&lock);
	res = q.empty();
	uv_mutex_unlock(&lock);
	return res;
}

OpelSCModel::OpelSCModel(char *intf_name, OpelCommHandler defCb, OpelCommHandler statCb)
{
	strncpy(this->intf_name, intf_name, MAX_INTF_LEN);
	this->defCb = defCb;
	this->statCb = statCb;

	OpelParamSet *ops = (OpelParamSet *)malloc(sizeof(OpelParamSet));
	RParam *rparam = (RParam *)malloc(sizeof(RParam));
	WParam *mwparam = (WParam *)malloc(sizeof(WParam));
	WParam *fwparam = (WParam *)malloc(sizeof(WParam));
	
	if(NULL == ops || NULL == rparam || NULL == mwparam || NULL == fwparam){
		comm_log("Memory allocation failed");
		exit(1);
	}

	rparam->orq = &rqueue;
	rparam->defCb = defCb;
	rparam->statCb = statCb;
	rparam->op_msg = new OpelMessage();

	mwparam->ocq = &mqueue;
	mwparam->defCb = defCb;
	mwparam->op_msg = NULL;
	
	fwparam->ocq = &fqueue;
	fwparam->defCb = defCb;
	fwparam->op_msg = NULL;

	priv = (void *)ops;
	ops->rreq.data = (void *)rparam;
	ops->mreq.data = (void *)mwparam;
	ops->freq.data = (void *)fwparam;
}
OpelSCModel::~OpelSCModel()
{
	Stop();
	OpelParamSet *ops = (OpelParamSet *)priv;

	if(NULL != ops){
		RParam *rparam = (RParam *)(ops->rreq.data);
		WParam *mwparam = (WParam *)(ops->mreq.data);
		WParam *fwparam = (WParam *)(ops->freq.data);

		delete(rparam->op_msg);

		free(rparam);
		free(mwparam);
		free(fwparam);

		free(ops);
	}
}

bool OpelSCModel::Start()
{
	OpelParamSet *ops = (OpelParamSet *)priv;
	if(NULL == ops){
		comm_log("OpelSCModel is not properly initialized");
		return false;
	}
	uv_queue_work(uv_default_loop(), &(ops->rreq), \
			generic_read_handler, after_read_handler);
	uv_queue_work(uv_default_loop(), &(ops->mreq), \
			generic_mwrite_handler, after_mwrite_handler);
	uv_queue_work(uv_default_loop(), &(ops->freq), \
			generic_fwrite_handler, after_fwrite_handler);

	return true;
}

bool OpelSCModel::Stop()
{
	comm_log("Stop all threads");
	OpelParamSet *ops = (OpelParamSet *)priv;

	if(rqueue.waitStat)
		rqueue.enqueue(NULL);
	if(mqueue.waitStat)
		rqueue.enqueue(NULL);
	if(fqueue.waitStat)
		fqueue.enqueue(NULL);

	uv_cancel((uv_req_t *)&(ops->rreq));
	uv_cancel((uv_req_t *)&(ops->mreq));
	uv_cancel((uv_req_t *)&(ops->freq));

	comm_log("Done stopping all threads");

	return true;
}

bool OpelSCModel::Send(OpelMessage *msg)
{
	comm_log("Type: %d", msg->getType() & PACKET_TYPE_FILE);
	if((msg->getType() & PACKET_TYPE_MSG) != 0){
		comm_log("Msg sending");
		mqueue.enqueue(msg);
	}
	else if((msg->getType() & PACKET_TYPE_FILE) != 0){
		comm_log("File sending");
		fqueue.enqueue(msg);
	}
}

static bool process_msg(OpelMessage *op_msg, uint8_t *buff)
{
	op_msg->setData(buff, op_msg->getDataLen());
	comm_log("Message Rcv:%s", (char *)op_msg->getData());

	return true;
}
static bool process_file(OpelMessage *op_msg, uint8_t *buff)
{
	if((op_msg->getType() & PACKET_TYPE_SPE) != 0){
		comm_log("Received File : %s", op_msg->getDestFName());
		if(op_msg->getDataLen() > 0){
			op_msg->setData(buff, op_msg->getDataLen());
			comm_log("Piggy-backed msg:%s", (char *)op_msg->getData());
			return true;
		}
	}

	comm_log("Process File %s(%d[%d]/%d)", op_msg->getDestFName(), op_msg->getOffset(), op_msg->getDataLen(), op_msg->getFSize());
	FILE *file_p = NULL;
	if(op_msg->getOffset() == 0)
		file_p = fopen(op_msg->getDestFName(), "w+");
	else
		file_p = fopen(op_msg->getDestFName(), "a");
	
	if(NULL == file_p){
		comm_log("fopen error");
		return false;
	}

	fseek(file_p, op_msg->getOffset(), SEEK_SET);
	
	if(op_msg->getDataLen() != fwrite(buff, 1, op_msg->getDataLen(), file_p)){
		comm_log("Fwrite error : %s", op_msg->getDestFName());
		fclose(file_p);
		return false;
	}

	fclose(file_p);

	return true;
}

static void generic_read_handler(uv_work_t *req)
{
	comm_log("Generic Read Handler Up");
	uint16_t stat = COMM_S_OK;
	RParam *rparam = (RParam *)req->data;
	OpelReadQueue *orq = rparam->orq;
	uint8_t buff[BT_MAX_DAT_LEN-COMM_HEADER_SIZE] = {0,};
	uint8_t header_buff[COMM_HEADER_SIZE] = {0,};
	OpelMessage tmp_msg;

	do{
		//blocking dequeue
		OpelSocket *sock = orq->dequeue();
		if(NULL == sock){
			stat = COMM_E_FAIL;
			break;
		}
		tmp_msg.setSocket(sock);
		memset((void *)header_buff, 0, COMM_HEADER_SIZE);
		memset((void *)buff, 0, BT_MAX_DAT_LEN-COMM_HEADER_SIZE);
		comm_log("Reading header...");
		ssize_t rsize = sock->Read((void *)header_buff, COMM_HEADER_SIZE);
		if(rsize <= 0){
			comm_log("Socket closed %u", sock->getFd());
			stat = COMM_E_DISCON;
			break;
		}
		
		if(!tmp_msg.initFromBuff(header_buff)){
			comm_log("Init from buff error");
			stat = COMM_E_FAIL;
			break;
		}

		rsize = sock->Read((void *) buff, tmp_msg.getDataLen());
		if(rsize <= 0){
			comm_log("Socket closed %u", sock->getFd());
			stat = COMM_E_DISCON;
			break;
		}

		if((tmp_msg.getType() & PACKET_TYPE_MSG) != 0){
			process_msg(&tmp_msg, buff);
		}
		else if((tmp_msg.getType() & PACKET_TYPE_FILE) != 0){
			process_file(&tmp_msg, buff);
		}

	}while(0);

	rparam->stat = stat;
	*(rparam->op_msg) = tmp_msg;

	return;
}

static void after_read_handler(uv_work_t *req, int stat)
{
	comm_log("Generic Read Handler Down");
	RParam *rparam = (RParam *)req->data;
	OpelMessage *op_msg = rparam->op_msg;

	if(UV_ECANCELED == stat){
		return;
	}
	if(rparam->stat == COMM_E_FAIL)
	{
		op_msg->setErr(COMM_E_FAIL);
		rparam->defCb(op_msg, COMM_E_FAIL);
	}
	else if(rparam->stat == COMM_E_DISCON)
	{
		op_msg->setErr(COMM_E_DISCON);
		op_msg->getSocket()->setStat(STAT_DISCON);
		rparam->statCb(op_msg, STAT_DISCON);
	}
	else if(rparam->stat == COMM_S_OK)
	{
		rparam->defCb(op_msg, COMM_S_OK);
	}

	//get() in enqueue to rqueue, put() after handling read
	op_msg->getSocket()->put();
	/* Init message */
	OpelMessage tmp;
	*op_msg = tmp;

	uv_queue_work(uv_default_loop(), req, generic_read_handler, after_read_handler);
}

static void generic_mwrite_handler(uv_work_t *req)
{
	comm_log("MWrite Handler UP");
	WParam *mparam = (WParam *)req->data;
	OpelCommQueue *ocq = mparam->ocq;
	uint16_t stat = COMM_S_OK;
	OpelMessage op_msg;
	uint8_t buff[BT_MAX_DAT_LEN];

	while(true)
	{
		if(!ocq->dequeue(&op_msg)){
			comm_log("Dequeue unblocked but, stat is not OK");
			stat = COMM_E_FAIL;
			break;
		}

		comm_log("Write to socket: %s(%d)", (char *)op_msg.getData(), op_msg.getDataLen());

		OpelSocket *op_sock = op_msg.getSocket();
		if(NULL == op_sock)
		{
			comm_log("Null Socket send?");
			stat = COMM_E_FAIL;
			break;
		}
		memset((void *)buff, 0, BT_MAX_DAT_LEN);
		op_msg.initToBuff(buff);
		memcpy((void *)(buff+COMM_HEADER_SIZE), (void *)op_msg.getData(), op_msg.getDataLen());

		ssize_t wbytes = op_sock->Write((void *)buff, COMM_HEADER_SIZE+op_msg.getDataLen());
		if((ssize_t)op_msg.getDataLen()+COMM_HEADER_SIZE != wbytes)
		{
			comm_log("Write Error : %d written", wbytes);
			stat = COMM_E_FAIL;
			mparam->op_msg = new OpelMessage();
			*(mparam->op_msg) = op_msg;
			break;
		}
		
		op_msg.getSocket()->put();
	}

	mparam->stat = stat;
}

static void after_mwrite_handler(uv_work_t *req, int status)
{
	comm_log("MWrite Handler DOWN");
	WParam *mparam = (WParam *)req->data;
	uint16_t stat = mparam->stat;

	if(status == UV_ECANCELED)
		return;

	if(stat != COMM_S_OK)
	{
		mparam->defCb(mparam->op_msg, stat);
		if(mparam->op_msg == NULL){
			comm_log("Write error before getting message");
		}
		else{
			mparam->op_msg->getSocket()->put();
			delete mparam->op_msg;
			mparam->op_msg = NULL;
		}
	}
	else
	{
		comm_log("Weird case");
	}

	uv_queue_work(uv_default_loop(), req, generic_mwrite_handler, after_mwrite_handler);			
}

static void generic_fwrite_handler(uv_work_t *req)
{
	comm_log("FWrite Handler UP");
	WParam *fparam = (WParam *)req->data;
	OpelCommQueue *ocq = fparam->ocq;
	uint16_t stat = COMM_S_OK;
	OpelMessage op_msg;
	uint8_t buff[BT_MAX_DAT_LEN];
	FILE *fp_file;

	while(true)
	{
		if(!ocq->dequeue(&op_msg)){
			comm_log("Dequeue unblocked but, stat is not OK");
			stat = COMM_E_FAIL;
			break;
		}

		comm_log("Write to socket: File:%s", (char *)op_msg.getSrcFName());

		OpelSocket *op_sock = op_msg.getSocket();
		if(NULL == op_sock)
		{
			comm_log("Null Socket send?");
			stat = COMM_E_FAIL;
			break;
		}
		memset((void *)buff, 0, BT_MAX_DAT_LEN);

		fp_file = fopen(op_msg.getSrcFName(), "r");
		if(NULL == fp_file){
			comm_log("Fopen error");
			stat = COMM_E_FOPEN;
			break;
		}

		fseek(fp_file, 0, SEEK_END);
		op_msg.setFSize((uint32_t)ftell(fp_file));
		rewind(fp_file);
		ssize_t rbytes;
		uint32_t acc_bytes = 0;
		while(true){
			rbytes = fread((void *)(buff+COMM_HEADER_SIZE), 1, \
					BT_MAX_DAT_LEN-COMM_HEADER_SIZE, fp_file);
			if(rbytes == 0 && feof(fp_file))
			{
				comm_log("Last msg gogo");
				op_msg.setType(PACKET_TYPE_SPE | PACKET_TYPE_FILE);
				op_msg.setOffset(acc_bytes);
				if(NULL != op_msg.getData()){
					op_msg.setDataLen(strlen((char*)op_msg.getData())+1 );
					memcpy((void *)(buff+COMM_HEADER_SIZE), (void *)op_msg.getData(),\
							op_msg.getDataLen());
				}
				else
					op_msg.setDataLen(0);

				op_msg.initToBuff(buff);
				if(COMM_HEADER_SIZE + op_msg.getDataLen() != \
						op_sock->Write((void *)buff, COMM_HEADER_SIZE + op_msg.getDataLen())){
					comm_log("write error %d", COMM_HEADER_SIZE + rbytes);
					stat = COMM_E_FAIL;
				}

				break;
			}
			if(rbytes < BT_MAX_DAT_LEN-COMM_HEADER_SIZE){
				if(!feof(fp_file)){
					comm_log("fread error");
					stat = COMM_E_FAIL;
					break;
				}
			}

			op_msg.setOffset(acc_bytes);
			op_msg.setDataLen((uint32_t)rbytes);
			acc_bytes += rbytes;
			op_msg.initToBuff(buff);

			if(COMM_HEADER_SIZE+rbytes != op_sock->Write((void *)buff, COMM_HEADER_SIZE + rbytes)){
				comm_log("socket write error %d", COMM_HEADER_SIZE + rbytes);
				stat = COMM_E_FAIL;
				break;
			}
		}

		if(stat != COMM_S_OK)
			break;

		op_msg.getSocket()->put();
	}


	fparam->stat = stat;
}

static void after_fwrite_handler(uv_work_t *req, int status)
{
	comm_log("FWrite Handler DOWN");
	WParam *mparam = (WParam *)req->data;
	uint16_t stat = mparam->stat;

	if(stat == UV_ECANCELED)
		return;

	if(status != COMM_S_OK)
	{
		mparam->defCb(mparam->op_msg, stat);
		if(mparam->op_msg == NULL){
			comm_log("Write error before getting message");
		}
		else{
			mparam->op_msg->getSocket()->put();
			delete mparam->op_msg;
			mparam->op_msg = NULL;
		}
	}
	else
	{
		comm_log("Weird case");
	}

	uv_queue_work(uv_default_loop(), req, generic_fwrite_handler, after_fwrite_handler);			

}
