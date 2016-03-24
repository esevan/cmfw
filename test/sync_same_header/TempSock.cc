#include "TempSock.h"
#include "OpelBT.h"
#include <bluetooth/rfcomm.h>
#include <unistd.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif

int openServerSock() {
	int s, err;
	char test_intf[256] = "Test Interface";
	sdp_session_t *session;
	s = socket( AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM );
	err = bt_dynamic_bind_rc( s );
	
	if( err <= 0 ) {
		printf("bind failed %d\n", err);
		return -1;
	}
	else {
		printf("Bound port : %d\n", err);
	}
	
	session = bt_register_service( (uint8_t *)test_intf, err );
	if( session == NULL ) {
		printf("Session creation failed\n");
		return -1;
	}
	else {
		printf("Session creationg succeeded\n");
	}
	
	if ( listen( s, 8 ) < 0 ) {
		printf("Listening socket failed\n");
		return -1;
	}
	
	return s;
}

int acceptSock( int s ) {
	int res = 0;
	struct sockaddr_rc cli_addr = {0,};
	socklen_t opt = sizeof(cli_addr);
	int new_cli_fd = -1;

	do{
		new_cli_fd = accept( s, (struct sockaddr *)&cli_addr, &opt );

		if(new_cli_fd < 0){
			printf("Accept Failed\n");
			res = -1;
			break;
		}
	}while(0);
	
	if( res < 0 )
		return -1;

	return new_cli_fd;
}

int connectToServer() {
	int s;
	char test_intf[256] = "Test Interface";
	
	s = socket( AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM );
	
	if ( 0 < bt_connect( (uint8_t *)test_intf, s ) ) {
		return -1;
	}
	else
		return s;	
}

void closeSock( int s ) {
	close( s );
}
#ifdef __cplusplus
}
#endif
