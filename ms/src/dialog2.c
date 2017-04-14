#include "mqclient.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int mq_cb(void *dest,const char* op,const char* content,const char* respTarget)
{
	printf("dialog接收到消息[replyTo=%s\n op=%s\n content=%s\n]\n", respTarget,op,content);
	char c_op[100],c_content[2000],c_respTarget[100];
	strcpy(c_op,op);
	strcpy(c_content,content);
	strcpy(c_respTarget,respTarget);
	printf("[replyTo=%s\n op=%s\n content=%s\n]\n", c_respTarget,c_op,c_content);
	mq_client_send(dest,c_op,c_content, c_respTarget);
	return 0;
}

int main(int argc, char **argv)
{
	if ( argc != 2 ){
		printf("recv [mq]\n");
	}
	void *p ;
	mq_client_init(0, &p);
	mq_listener_add(p,argv[1],&mq_cb);
	
	while(1){
		printf("exit:y/n?");
		if(getchar()=='y')
			break;
	}

	mq_listener_stop(p);
	mq_client_destory(p);
	printf("recv exits!!!\n");
}
