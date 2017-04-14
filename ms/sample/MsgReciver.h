#ifndef _MSG_RECIVER_H
#define _MSG_RECIVER_H

#include "common.h"
#include "mqclient2.h"
#include "MsgDealler.h"

class MsgReciver: public mq_client {
public:
	int start();
};

#endif
