#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <comm_core.h>


char *intf_name = "Companion.sample1";
OPEL_Server *ser = NULL;

void onRead(OPEL_MSG *op_msg, int status);
void onAck(OPEL_MSG *op_msg, int status);
void onAck2(OPEL_MSG *op_msg, int status);

int main()
{
	ser = new OPEL_Server(intf_name, onRead);

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	return 0;
}
void onRead(OPEL_MSG *op_msg, int status)
{
	static int test_cnt = 0;
	printf("Server:OnRead\n");

	if(NULL == op_msg){
		printf("OP_msg = NULL\n");
		exit(1);
	}

	printf("\tGot data:%s\n", (char *)op_msg->get_data());
	
	////////////////////////////////////////////////////////////////////
	if(!strcmp("REQ from client", (char *)op_msg->get_data())){
		char send_str[] = "ACK from server";
		printf("\t\tsend:%s\n", send_str);
		ser->msg_write(send_str, strlen(send_str)+1, op_msg, NULL);
	}
	////////////////////////////////////////////////////////////////////
	else if(0 == test_cnt){
		char send_str[] = "MSG write test from server";
		printf("\t\tsend:%s\n", send_str);
		ser->msg_write(send_str, strlen(send_str)+1);
		test_cnt++;
	}
	////////////////////////////////////////////////////////////////////
	else if(1 == test_cnt){
		char send_str[] = "REQ write test from server";
		printf("\t\tsend:%s\n", send_str);
		ser->msg_write(send_str, strlen(send_str)+1, NULL, onAck);
		test_cnt++;
	}
	////////////////////////////////////////////////////////////////////
}

