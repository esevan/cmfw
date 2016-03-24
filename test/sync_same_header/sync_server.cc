#include "TempSock.h"
#include "OpelHeader.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
	int cli, s, res, n;
	char buf[1024] = {0, };
	struct timeval base, curr;
	FILE *fp_log;
	
	if(argc != 2) {
		printf("Usage : ./syncServer [LogFileName]\n");
		return 0;
	}
	fp_log = fopen( argv[1], "w+");
	OpelHeader *op_header = new OpelHeader();
	
	s = openServerSock();
	assert( s > 0 );
	
	cli = acceptSock( s );
	assert( cli > 0 );
	
	printf("Accepted, read start\n");
	
	res = read( cli, buf, COMM_HEADER_SIZE );
	assert( res == COMM_HEADER_SIZE );
	
	op_header->initFromBuff( (uint8_t *) buf );
	
	printf( "Type : %d\t data length : %d\n", op_header->type, op_header->data_len );
	
	res = read( cli, buf, op_header->data_len );
	assert( res == op_header->data_len );
	
	gettimeofday( &base, NULL );
	printf("Fist Msg has come, base time recored\n");
	
	n = 1;
	while (1) {
		res = read( cli, buf, COMM_HEADER_SIZE );
		assert( res == COMM_HEADER_SIZE );
		
		op_header->initFromBuff( (uint8_t *) buf );
		
		printf( "Type : %d\n data length : %d\n", op_header->type, op_header->data_len );
		
		res = read( cli, buf, op_header->data_len );
		assert( res == op_header->data_len );
		
		gettimeofday( &curr, NULL );
		
		curr.tv_usec -= base.tv_usec;
		curr.tv_sec -= base.tv_sec;
		
		if( curr.tv_usec < 0 ) {
			curr.tv_sec--;
			curr.tv_usec += 1000000;
		}
		fprintf(fp_log, "%d\t%ld.%ld\n", n++, curr.tv_sec, curr.tv_usec);
		
		if(n > 100)
			break;
	}
	
	fclose( fp_log );
	close ( s );
	close ( cli );
	
	return 0;	
}
