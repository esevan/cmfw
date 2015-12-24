#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <comm_core.h>


char *intf_name = "CompanionSample";
OPEL_Server *ser = NULL;

void onRead(OPEL_MSG *op_msg, int status);

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
	
}

