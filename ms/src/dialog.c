#include "mqclient2.h"
#include <stdio.h>
#include <stdlib.h>


#define PROFILE 

int main(int argc, char **argv)
{
	if ( argc != 5) {
		printf("dialog [op] [content] [targetTag] [timeout]\n");
		return -1;
	}
	int timeout = atoi(argv[4]);
	char * resp;
	mq_client mqClient;
	mqClient.init();

	mqClient.dialog_wait( argv[1], argv[2], argv[3],timeout,&resp);
	printf("RESP[%s]\n",resp);
	delete resp;
	
	mqClient.destory();
}
