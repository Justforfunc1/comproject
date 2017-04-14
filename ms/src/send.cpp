#include "mqclient2.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	if ( argc < 3 || argc > 4) {
		printf("send [op] [content] [targetTag]");
		return -1;
	}
	mq_client mqClient;
	mqClient.init();

	char *to = NULL;
	if(argc == 4){
		to = argv[3];
	}
	mqClient.send(argv[1],argv[2], to);
	mqClient.destory();
}