#include "mqclient2.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int mq_cb(mq_client* client,const char* op,const char* content,const char* respTarget)
{
	printf("接收到消息[source=%s\n op=%s\n content=%s\n]\n", respTarget,op,content);	
	return 0;
}

int main(int argc, char **argv)
{
	if ( argc != 2 ){
		printf("recv [mq]\n");
	}
	mq_client mqClient;
	mqClient.init();
	
	mqClient.add_listener(argv[1],&mq_cb);
	//mq_listener_stop(p);
	while(1){
		printf("exit:p/r/y/n?");
		char ch=getchar();
		if(ch=='y') {
			break;
		} else if (ch=='p') {
			mqClient.pause(true);
		} else if (ch=='r') {
			mqClient.pause();
		}
		
	}

	mqClient.stop();
	mqClient.destory();
	printf("recv exits!!!\n");
}
