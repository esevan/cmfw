#include <OpelClient.h>
#include <stdio.h>

void defCb(OpelMessage *, uint16_t err);
void statCb(OpelMessage *, uint16_t stat);
static char test_intf[256] = "Test Interface";
static OpelClient op_client(test_intf, defCb, statCb);

static bool connected = false;

void defCb(OpelMessage *op_msg, uint16_t err)
{
	static int a =0;
	if(err == 0){
		printf("Client Test: Default Callback called\n");
	}
	else
		printf("Error happens");
}

void statCb(OpelMessage *op_msg, uint16_t stat)
{
	printf("Client Test: STat Cb called\n");
	if(stat == STAT_CONNECTED){
		connected = true;
		printf("Sending file... \n");
		op_client.SendFile("./oops", "./haps");
	}
	else if(stat == STAT_DISCON){
		connected = false;
	}
}

int main()
{

	op_client.Start();
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	return 0;
}