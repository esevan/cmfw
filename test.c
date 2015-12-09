#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <comm_core.h>


char *intf_name = "Test Interface";
OPEL_Server *ser = NULL;

void default_read_handler(OPEL_MSG *op_msg, int status);
void ack_handler(OPEL_MSG *op_msg, int status);
void file_ack_handler(OPEL_MSG *op_msg, int status);

int main()
{
	ser = new OPEL_Server(intf_name, default_read_handler);

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	sleep(30);

	return 0;
}
void default_read_handler(OPEL_MSG *op_msg, int status)
{
	static int test_cnt = 0;
	printf("default_read_hnadler\n");

	if(NULL == op_msg){
		printf("OP_msg = NULL\n");
		exit(1);
	}

	printf("get_data:%s\n", (char *)op_msg->get_data());

	if(!strcmp("Req message from client", (char *)op_msg->get_data())){
		char send_str[] = "Message for server's reply";
		printf("ack message test :%s\n", send_str);
		ser->msg_write(send_str, strlen(send_str)+1, op_msg, NULL);
	}
	else if(0 == test_cnt){
		char send_str[] = "Message for server's sending";
		printf("send message test:%s\n", send_str);
		ser->msg_write(send_str, strlen(send_str)+1);
		test_cnt++;
	}
	else if(1 == test_cnt){
		char send_str[] = "Message for ack test";
		printf("send & ack message test:%s\n", send_str);
		ser->msg_write(send_str, strlen(send_str)+1, NULL, ack_handler);
		test_cnt++;
	}
}

void ack_handler(OPEL_MSG *op_msg, int status)
{
	printf("ack_handler called\n");
	if(!status && NULL != op_msg){
		printf("Got data : %s\n", (char *)op_msg->get_data());
		printf("Sending file...");
		ser->file_write("~/workspace/Tizen_Tutorial.pptx", NULL, file_ack_handler);
	}
	else
		printf("Failed\n");

}

void file_ack_handler(OPEL_MSG *op_msg, int status)
{
	printf("File send done!\n");
}

