#include <OpelServer.h>
#include <stdio.h>

void defCb(OpelMessage *, uint16_t err);
void statCb(OpelMessage *, uint16_t stat);
static char test_intf[256] = "Test Interface";
static OpelServer op_server(test_intf, defCb, statCb);

static bool connected = false;

void defCb(OpelMessage *op_msg, uint16_t err)
{
	static int a = 0;
	if(err == 0){
		printf("Server Test: Default Callback Called\n");
		if((op_msg->getType() & PACKET_TYPE_SPE) != 0){
			printf("op_msg->getDestFName() received from op_msg->getSrcFName()\n");
			op_server.SendFile("./haps", "./hops");
			op_server.SendMsg("haps --> hops");
		}

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
	op_server.Start();
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	return 0;
}
