#include <OpelServer.h>
#include <stdio.h>

static char test_intf[256] = "Test Interface";
static bool connected = false;

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
	if(stat == STAT_CONNECTED){
		connected = true;
	}
	else if (stat == STAT_DISCON){
		connected = false;
	}
}
int main()
{
	OpelServer op_server(test_intf, defCb, statCb);

	op_server.Start();
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	while(true)
	{
		char buf[1024];
		scanf("%s", buf);
		if(connected)
			op_server.SendMsg(buf);
		else
			printf("No client has been connected\n");
	}

	return 0;
}
