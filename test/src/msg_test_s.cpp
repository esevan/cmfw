#include <OpelServer.h>
#include <stdio.h>

static char test_intf[256] = "Test Interface";

void defCb(OpelMessage *op_msg, uint16_t err)
{
	if(err == 0){
		printf("Server Test: Default Callback Called\n");
		printf("%s \n", op_msg->getData());
	}
}
void statCb(OpelMessage *op_msg, uint16_t stat)
{
	printf("Server Test: Stat Cb Called\n");
}
int main()
{
	OpelServer op_server(test_intf, defCb, statCb);

	op_server.Start();
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	return 0;
}
