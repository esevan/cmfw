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
		printf("%s \n", op_msg->getData());
		if(a++ < 10)
			op_client.Respond(op_msg, "Okshdfiodshjfklj");
	}
	else
		printf("Error happens");
}

void statCb(OpelMessage *op_msg, uint16_t stat)
{
	printf("Client Test: STat Cb called\n");
	if(stat == STAT_CONNECTED){
		connected = true;
		op_client.SendMsg("First");
	}
	else if(stat == STAT_DISCON){
		connected = false;
	}
}

int main()
{

	op_client.Start();
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
/*
	while(true)
	{
		char buf[1024];
		scanf("%s", buf);
		if(connected)
			op_client.SendMsg(buf);
		else
			printf("No connection\n");
	}
	*/

	return 0;
}
