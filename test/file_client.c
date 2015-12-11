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
void onAck2(OPEL_MSG *op_msg, int status);

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
		printf("Connect Succedded\n");
		gettimeofday(&init_time, NULL);
		char send_str[] = "File Transfer Test Start";
		printf("%s\n", send_str);
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
	if(NULL == op_msg){
		printf("OP_MSG = NULL\n");
		return;
	}
	if(!status){
		gettimeofday(&tmp_time, NULL);
		char fname[] = "./res/cmfw.tar.gz";

		if(op_msg->is_msg() || op_msg->is_special()){
			printf("Got Msg:%s\n", (char *)op_msg->get_data());
			cli->file_write(fname, op_msg, onAck);
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
		if(op_msg->is_msg() || op_msg->is_special()){
			printf("Got data : %s\n", (char *)op_msg->get_data());
			cli->msg_write("cmfw.tar.gz", strlen("cmfw.tar.gz")+1, NULL, onAck2);
		}
		
	}
	else
		printf("Failed\n");
}

void onAck2(OPEL_MSG *op_msg, int status)
{
	if(!status && NULL != op_msg){
		if(op_msg->is_msg() || op_msg->is_special()){
			printf("OnAck2]Got data: %s\n", (char *)op_msg->get_data());
			printf("Congratulation");
		}
	}
}
