#include "TempSock.h"
#include "OpelHeader.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

int main()
{
	int cli, s, res, n;
	char buf[1024] = {0, };
	char msg[1024] = "Message Test is ongoing";
	
	OpelHeader *op_header = new OpelHeader();
	
	s = connectToServer();
	assert( s > 0 );
	
	strcpy(buf + COMM_HEADER_SIZE, msg);
	
	op_header->data_len = strlen( msg ) + 1;
	op_header->type = PACKET_TYPE_MSG;
	op_header->initialized = true;
	
	op_header->initToBuff( (uint8_t *) buf );
	
	while (1) {
		res = write( cli, buf, COMM_HEADER_SIZE + op_header->data_len);
		usleep(1000);		
	}
	
	close ( s );
	close ( cli );
	
	return 0;	
}
