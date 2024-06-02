#include "QuicServer.h"
#include <cstdio>

int main(){

	QuicServer *server = new QuicServer("10.10.3.201", 6121, "./certs/server.cert", "./certs/server.key");

	server->Start();
	
	while (getchar() != 'e') {
		
	}

	server->Close();

	free(server);
	return 0;	
}
