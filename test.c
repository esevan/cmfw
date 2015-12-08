#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <comm_core.h>


char *intf_name = "Test Interface";
OPEL_Server *ser = NULL;

void second_handler(OPEL_MSG *op_msg, int status)
{
	printf("Second handler called\n");
	return;
}
void handler(OPEL_MSG *op_msg, int status)
{
	printf("Server handler has been called\n");
	if(NULL == op_msg)
		printf("Wow %d\n", status);

	printf("%s\n", (char *)op_msg->get_data());
	ser->msg_write("hihi", 5, op_msg, second_handler);
}
int main()
{
	ser = new OPEL_Server(intf_name, handler);

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	sleep(30);

	return 0;
}

