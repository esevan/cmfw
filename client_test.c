#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <comm_core.h>
#include <string.h>

char *intf_name = "Test Interface";
OPEL_Client *cli = NULL;

void handler(OPEL_MSG *op_msg, int status);
void rhandler(OPEL_MSG *op_msg, int status);
void handler(OPEL_MSG *op_msg, int status)
{
	char *haha = "Hello MSG";
	printf("Handler has been called\n");
	if(NULL == op_msg)
		printf("noop!%d\n", status);

	if(NULL != op_msg) {
		printf("%s\n", (char *)op_msg->get_data());
		if(strcmp((char *)op_msg->get_data(), "hihi")){
			cli->msg_write("oh ho", 6, op_msg);
		}
	}
	if(!status){
		printf("Write!\n");
		cli->msg_write(haha, strlen(haha)+1);
	}

}
void rhandler(OPEL_MSG *op_msg, int status)
{
	printf("rHandler has been called\n");
	if(!status){
		printf("Error : %d\n", status);
	}
	if(NULL == op_msg)
		printf("noop!\n", status);
	printf("%s\n", op_msg->get_data());
	cli->msg_write("Hello again", strlen("Hello again")+1);
}

int main()
{
	cli = new OPEL_Client(intf_name, rhandler, handler);
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	sleep(30);
	printf("Done\n");
	return 0;
}
