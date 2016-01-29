#include <OpelServer.h>
#include <stdio.h>

void defCb(OpelMessage *, uint16_t err);
void statCb(OpelMessage *, uint16_t stat);
static char test_intf[256] = "Test Interface";
static OpelServer op_server(test_intf, defCb, statCb);

static bool connected = false;

void defCb(OpelMessage *op_msg, uint16_t err)
{
	if(err == 0){
		printf("Server Test: Default Callback Called\n");
		printf("%s \n", op_msg->getData());
		op_server.SendMsg("hisdlkfjsdlfksdflksfdjlksdjfk");
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
/*void Main(uv_work_t *req)
{
	printf("Main start\n");
	OpelServer *op_server = (OpelServer *)req->data;
	while(true)
	{
		char buf[1024];
		printf("Scanning\t");
		scanf("%s", buf);
		if(connected)
			op_server->SendMsg(buf);
		else
			printf("No client has been connected\n");
	}
}
void afterMain(uv_work_t *req, int status)
{

}
*/
int main()
{
	op_server.Start();
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	return 0;
}
