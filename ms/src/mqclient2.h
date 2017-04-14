/*****************************************
*mq ��Ϣ���ж���ӿ�
*****************************************/
#ifndef _MQCLIENT_H_
#define _MQCLIENT_H_

#include <stdio.h>

#define MQCLIENT_OK				0	//�����ɹ�
#define MQCLIENT_CONN			-1	//����ʧ��
#define MQCLIENT_SEND_ERROR		-2	//����ʧ��
#define MQCLIENT_RECV_ERROR		-3	//����ʧ��
#define MQCLIENT_DATAINFO_ERROR	-4	//���ݽṹ�쳣
#define MQCLIENT_ARG_ERROR		-5	//�����쳣
#define MQCLIENT_INI_ERROR		-6	//�����쳣
#define MQCLIENT_RECV_TIMEOUT	-7	//���ճ�ʱ
#define MQCLIENT_INIT_ERROR		-8	//��ʼ������
#define MQCLIENT_THREAD_ERROR	-9	//�̴߳���
#define MQCLIENT_SESSION		-10	//�Ự��������

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
	
	//----- ��ʼ�� ----
	//path: mqclient.ini·����ȱʡNULLΪ��ǰĿ¼��
	//msd: mqclient�ڲ�����
	int init(const char *path=NULL);

	//------- ���� ------
	void destory();

	//---- ������Ϣ ------
	//msd:	mqclient�ڲ�����
	//op:	������
	//content:	��Ϣ��
	//targetPath:	�ض�����·����ȱʡ��NULL
	int send(const char* op,const char* content,const char* targetPath);

	//---- ͬ���ȴ��Ի����أ���ʱ�˳� ----
	//timeout: �ȴ����صĳ�ʱʱ�䣬��λΪ��
	//resp����Ӧ����
	int dialog_wait(const char* op, const char* content, const char* targetPath, int timeout, char **respContent);

	//---- �첽�ȴ��Ի����أ����ػص� ----
	//*dest_fun: �ص�����
	//timeout: ��ȴ�ʱ�䣬��λΪ��,ȱʡΪ0�����õȴ�
	//respTarget��Ӧ����
	//int mq_dialog_reply(void * msd,char* op,char* content,char* targetPath,int timeout
	//			,int (*dest_func)(const char* op,const char* content,const char* respTarget));

	//------ �첽��Ϣ����  -------
	int add_listener(const char* listenPathTag, DestFunc dest_func);

	int stop();
	//---- ��ͣ������Ϣ ----
	//flag : true  ��ͣ, false �ָ�
	int pause(bool flag=false);
	
	void exec_callback(const char* op,const char* content,const char* respTarget);
	
	void new_log();
};

#endif
