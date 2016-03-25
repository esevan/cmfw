#include "TempSock.h"
#include "OpelHeader.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int cli, s, res, n;
	char buf[1024] = {0, };
	char msg[1024] = "Message Test is ongoing";
	FILE *fp_file_to_transfer;
	
	OpelHeader *op_header = new OpelHeader();
	
	s = connectToServer();
	assert( s > 0 );
	
	if( argc == 1 ) {
		strcpy(buf + COMM_HEADER_SIZE, msg);
		
		op_header->data_len = strlen( msg ) + 1;
		op_header->type = PACKET_TYPE_MSG;
		op_header->initialized = true;
	
		op_header->initToBuff( (uint8_t *) buf );
		while (1) {
			res = write( cli, buf, COMM_HEADER_SIZE + op_header->data_len);
			usleep(1000);		
		}
	}
	else if ( argc == 2 ) {
		fp_file_to_transfer = fopen( argv[1], "r" );
		assert( fp_file_to_transfer != NULL );
		strcpy( op_header->srcFName, argv[1] );
		strcpy( op_header->destFName, argv[1] );
		fseek( fp_file_to_transfer, 0L, SEEK_END );
		op_header->fsize = ftell( fp_file_to_transfer );
		fseek( fp_file_to_transfer, 0L, SEEK_SET );
		op_header->offset = 0;
		op_header->type = PACKET_TYPE_FILE;
		op_header->initialized = true;
		
		while ( !feof( fp_file_to_transfer ) ) {
			res = fread( buf+COMM_HEADER_SIZE, sizeof ( char ), 1008-COMM_HEADER_SIZE, fp_file_to_transfer );
			assert( ferror( fp_file_to_transfer ) == 0 );
			
			op_header->data_len = res;
			op_header->initToBuff( (uint8_t *)buf );
			
			op_header->offset += res;
			res = write( cli, buf, COMM_HEADER_SIZE + op_header->data_len );
			assert( res == COMM_HEADER_SIZE + op_header->data_len );
		}
		printf("File transfer ended");
	}
	
	
	
	
	close ( s );
	close ( cli );
	
	return 0;	
}
