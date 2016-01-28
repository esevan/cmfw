#include <OpelClient.h>
#include <stdio.h>

void defCb(OpelMessage *, uint16_t err);
void statCb(OpelMessage *, uint16_t stat);
static char test_intf[256] = "Test Interface";
static OpelClient op_client(test_intf, defCb, statCb);

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
	if(stat == STAT_CONNECTED){
		op_client.SendMsg("HiHi");
	}
}

int main()
{

	op_client.Start();
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	return 0;
}
