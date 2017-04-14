/*****************************************
*mq 消息队列对外接口
*****************************************/
#ifndef _MQCLIENT_H_
#define _MQCLIENT_H_

#include <stdio.h>

#define MQCLIENT_OK				0	//操作成功
#define MQCLIENT_CONN			-1	//连接失败
#define MQCLIENT_SEND_ERROR		-2	//发送失败
#define MQCLIENT_RECV_ERROR		-3	//接收失败
#define MQCLIENT_DATAINFO_ERROR	-4	//数据结构异常
#define MQCLIENT_ARG_ERROR		-5	//参数异常
#define MQCLIENT_INI_ERROR		-6	//配置异常
#define MQCLIENT_RECV_TIMEOUT	-7	//接收超时
#define MQCLIENT_INIT_ERROR		-8	//初始化错误
#define MQCLIENT_THREAD_ERROR	-9	//线程错误
#define MQCLIENT_SESSION		-10	//会话创建错误

class mq_client;

typedef int (*DestFunc)(mq_client *dest,const char* op,const char* content,const char* respTarget);

class mq_client {

protected:
	static mq_client* _instance;
	void *m_pCtrl;
	DestFunc m_destFunc;

public:
	static mq_client* getInstance();
	static void release();
	mq_client();
	
	//----- 初始化 ----
	//path: mqclient.ini路径，缺省NULL为当前目录下
	//msd: mqclient内部数据
	int init(const char *path=NULL);

	//------- 销毁 ------
	void destory();

	//---- 发送消息 ------
	//msd:	mqclient内部数据
	//op:	操作号
	//content:	消息体
	//targetPath:	特定传送路径，缺省至NULL
	int send(const char* op,const char* content,const char* targetPath);

	//---- 同步等待对话返回，超时退出 ----
	//timeout: 等待返回的超时时间，单位为秒
	//resp：响应报文
	int dialog_wait(const char* op, const char* content, const char* targetPath, int timeout, char **respContent);

	//---- 异步等待对话返回，返回回调 ----
	//*dest_fun: 回调函数
	//timeout: 最长等待时间，单位为秒,缺省为0，永久等待
	//respTarget：应答者
	//int mq_dialog_reply(void * msd,char* op,char* content,char* targetPath,int timeout
	//			,int (*dest_func)(const char* op,const char* content,const char* respTarget));

	//------ 异步消息接收  -------
	int add_listener(const char* listenPathTag, DestFunc dest_func);

	int stop();
	//---- 暂停接收消息 ----
	//flag : true  暂停, false 恢复
	int pause(bool flag=false);
	
	void exec_callback(const char* op,const char* content,const char* respTarget);
	
	void new_log();
};

#endif
