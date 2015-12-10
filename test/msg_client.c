#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <comm_core.h>
#include <string.h>
#include <sys/time.h>

char *intf_name = "Test Interface";
OPEL_Client *cli = NULL;

void onConnect(OPEL_MSG *op_msg, int status);
void onRead(OPEL_MSG *op_msg, int status);
void onAck(OPEL_MSG *op_msg, int status);

int main()
{
	gettimeofday(&init_time, NULL);
	cli = new OPEL_Client(intf_name, onRead, onConnect);
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	sleep(30);
	printf("Done\n");
	return 0;
}

void onConnect(OPEL_MSG *op_msg, int status)
{
	printf("OnConnect:\n");
	if(!status){
	/////////////////////////////////////////////////////
		printf("\tConnect Succedded\n");
	
		char send_str[] = "Msg from client";
		cli->msg_write(send_str, strlen(send_str)+1);
	////////////////////////////////////////////////////
	}
	else{
		printf("\tConnect Failed(%d)\n", status);
		delete cli;
		cli = new OPEL_Client(intf_name, onRead, onConnect);
	}
}
void onRead(OPEL_MSG *op_msg, int status)
{
	printf("OnRead:\n");
	if(NULL == op_msg){
		printf("\tOP_MSG = NULL\n");
		return;
	}
	if(!status){
		printf("\tGot Msg:%s\n", (char *)op_msg->get_data());
		//////////////////////////////////////////////////////////////////
		if(!strcmp((char *)op_msg->get_data(), "REQ write test from server")){
			char send_str[] = "Ack from client";
			printf("\t\tSend: %s\n", send_str);
			cli->msg_write(send_str, strlen(send_str)+1, op_msg, NULL);
		}
		//////////////////////////////////////////////////////////////////
		else{
			char send_str[] = "Req message from client";
			printf("\t\tSend: %s\n", send_str);
			cli->msg_write(send_str, strlen(send_str)+1, NULL, onAck);
		}
		//////////////////////////////////////////////////////////////////
	}
	else{
		printf("\tRead Failed(%d)\n", status);
		exit(1);
	}
}
void onAck(OPEL_MSG *op_msg, int status)
{
	printf("OnAck:\n");
	if(!status && NULL != op_msg){
		//////////////////////////////////////////////////////////////////
		printf("\tGot data : %s\n", (char *)op_msg->get_data());
		char send_str[] = "Client req&ack test done";
		printf("\t\tSend :%s\n", send_str);
		cli->msg_write(send_str, strlen(send_str)+1);
		//////////////////////////////////////////////////////////////////
	}
	else
		printf("\tFailed\n");
}
