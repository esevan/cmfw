#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <comm_core.h>
#include <string.h>
#include <sys/time.h>

char *intf_name = "Test Interface";
OPEL_Client *cli = NULL;
struct timeval init_time;
struct timeval tmp_time;
struct timeval conn_time;

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
	gettimeofday(&conn_time, NULL);
	if(conn_time.tv_usec-init_time.tv_usec < 0)
		printf("%ld.%ld]OnConnect\n", conn_time.tv_sec-init_time.tv_sec-1, 1000000+conn_time.tv_usec-init_time.tv_usec);
	else
		printf("%ld.%ld]OnConnect\n", conn_time.tv_sec-init_time.tv_sec, conn_time.tv_usec-init_time.tv_usec);
	if(!status){
		char send_str[] = "First Msg from client";
		printf("Connect Succedded\n");
		cli->msg_write(send_str, strlen(send_str)+1);
	}
	else{
		printf("Connect Failed(%d)\n", status);
		delete cli;
		cli = new OPEL_Client(intf_name, onRead, onConnect);
	}
}
void onRead(OPEL_MSG *op_msg, int status)
{
	printf("OnRead\n");
	if(NULL == op_msg){
		printf("OP_MSG = NULL\n");
		return;
	}
	if(!status){
		printf("Got Msg:%s\n", (char *)op_msg->get_data());
		if(!strcmp((char *)op_msg->get_data(), "Message for ack test")){
			char send_str[] = "Ack message from client";
			printf("Sending %s\n", send_str);
			cli->msg_write(send_str, strlen(send_str)+1, op_msg, NULL);
		}
		else{
			char send_str[] = "Req message from client";
			printf("Sending %s\n", send_str);
			cli->msg_write(send_str, strlen(send_str)+1, NULL, onAck);
		}
	}
	else{
		printf("Read Failed(%d)\n", status);
		exit(1);
	}
}
void onAck(OPEL_MSG *op_msg, int status)
{
	printf("OnAck called\n");
	if(!status && NULL != op_msg){
		printf("Got data : %s\n", (char *)op_msg->get_data());
		char send_str[] = "Server req&ack test";
		printf("Send :%s\n", send_str);
		cli->msg_write(send_str, strlen(send_str)+1);
	}
	else
		printf("Failed\n");
}
