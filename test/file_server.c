#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <comm_core.h>
#include <sys/time.h>


struct timeval init_time;
struct timeval tmp_time;
char *intf_name = "Test Interface";
OPEL_Server *ser = NULL;

void onRead(OPEL_MSG *op_msg, int status);
void onCmfw(OPEL_MSG *op_msg, int status);
void file_ack_handler(OPEL_MSG *op_msg, int status);

int main()
{
	ser = new OPEL_Server(intf_name, onRead);

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	return 0;
}
void onRead(OPEL_MSG *op_msg, int status)
{
	static int test_cnt = 0;
	printf("onRead\n");

	if(NULL == op_msg){
		printf("OP_msg = NULL\n");
		exit(1);
	}

	printf("get_data:%s\n", (char *)op_msg->get_data());
	gettimeofday(&init_time, NULL);

	if(!strcmp("File Transfer Test Start", op_msg->get_data())){
		ser->file_write("./res/cmfw.tar.gz",NULL,onCmfw);
	}
	else if(!strcmp("cmfw.tar.gz", op_msg->get_data())){
		ser->file_write("./res/cmfw.tar.gz", op_msg, NULL);
	}
	else
		printf("Invalid msg\n");

}

void onCmfw(OPEL_MSG *op_msg, int status)
{
	printf("onCmfw\n");
	if(!status && NULL != op_msg){
		printf("Got data : %s\n", (char *)op_msg->get_data());
		if(op_msg->is_special()){
			printf("Server file transfer pass\n");
			ser->msg_write("Really?", strlen("Really?")+1, op_msg);
		}
	}
	else
		printf("Failed\n");

}

void file_ack_handler(OPEL_MSG *op_msg, int status)
{

}

