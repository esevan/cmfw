#include <OpelUtil.h>
#include <OpelServerSocket.h>
#include <OpelBT.h>
#include <bluetooth/rfcomm.h>
#include <unistd.h>
#include <string.h>


OpelServerSocket::OpelServerSocket()
{
	conn_type = 0;
	intf_name[0] = '\0';
	sock_fd = 0;
	stat = 0;
}

OpelServerSocket::OpelServerSocket(char* intf_name, uint8_t conn_type)
{
	this->conn_type = conn_type;
	strncpy(this->intf_name, intf_name, MAX_INTF_LEN);
	priv = NULL;
//	init();	
}

bool OpelServerSocket::init()
{
	if(conn_type & CONN_TYPE_BT != 0){
		sdp_session_t *session = (sdp_session_t *)priv;
		if(NULL != session){
			Close();
		}
		sock_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

		int port = bt_dynamic_bind_rc(sock_fd);
		if(!(port >= 1 && port <= 30)){
			comm_log("dynamic binding failed");
			return false;
		}
		else
			comm_log("Bound socket %d to port %d", sock_fd, port);
		session = bt_register_service((uint8_t *)intf_name, port);
		if(NULL == session){
			comm_log("session creation failed");
			return false; 
		}
		else{
			priv = (void *)session;
			comm_log("Session created");
		}

		if(listen(sock_fd, MAX_CLIENTS) < 0){
			comm_log("Listen failed");
			return false;
		}
	}
	else{
		comm_log("conn_type error");
		return false;
	}

	return true;
}

void OpelServerSocket::Close()
{
	if(priv != NULL){
		if(conn_type & CONN_TYPE_BT != 0){
			close(sock_fd);
			sdp_session_t *session = (sdp_session_t *)priv;
			sdp_close(session);
			priv = NULL;
		}
	}
}

OpelSocket* OpelServerSocket::Accept(int *err)
{
	int res = COMM_S_OK;
	struct sockaddr_rc cli_addr = {0,};
	socklen_t opt = sizeof(cli_addr);
	OpelSocket* op_sock = NULL;
	int new_cli_fd = -1;

	do{
		new_cli_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &opt);

		if(new_cli_fd < 0){
			comm_log("Accept Failed");
			res = COMM_E_FAIL;
			break;
		}

		op_sock = new OpelSocket(intf_name, conn_type);
		op_sock->setFd(new_cli_fd);

		res = COMM_S_OK;

	}while(0);

	*err = res;
	return op_sock;
}

int OpelServerSocket::getFd()
{
	return sock_fd;
}

void OpelServerSocket::set(char *intf_name, uint8_t conn_type)
{	
	strncpy(this->intf_name, intf_name, MAX_INTF_LEN);
	this->conn_type = conn_type;
}
