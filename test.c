#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <comm_core.h>


char *intf_name = "Test Interface";
OPEL_Server *ser = NULL;

void handler(OPEL_MSG *op_msg, int status)
{
	if(NULL == op_msg)
		printf("Wow %d\n", status);

	printf("%s\n", (char *)op_msg->get_data());
}
int main()
{
	ser = new OPEL_Server(intf_name, handler);

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	return 0;
}

