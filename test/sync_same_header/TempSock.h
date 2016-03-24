#ifndef _temp_sock_h_
#define _temp_sock_h_

#ifdef __cplusplus
extern "C"{
#endif
int openServerSock();
int acceptSock( int s );
int connectToServer();

void closeSock( int s );
#ifdef __cplusplus
}
#endif
#endif
