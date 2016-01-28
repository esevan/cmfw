#include <OpelClient.h>
#include <stdio.h>

static char test_intf[256] = "Test Interface";

void defCb(OpelMessage *op_msg, uint16_t err)
{
	if(err == 0){
		printf("Client Test: Default Callback called\n");
		printf("%s \n", op_msg->getData());
	}
	else
		printf("Error happens");
}

void statCb(OpelMessage *op_msg, uint16_t stat)
{
	printf("Client Test: STat Cb called\n");
}

int main()
{
	OpelClient op_client(test_intf, defCb, statCb);

	op_client.Start();
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	return 0;
}
