#include "mqclient2.h"
#include "MQLogger.h"
#include "MQControl.h"

mq_client* mq_client::_instance = NULL;

mq_client* mq_client::getInstance() {
	if ( !_instance )
		_instance = new mq_client();
	return _instance;
}

void mq_client::release() {
	if ( _instance ) {
		delete _instance;
		_instance = NULL;
	}
}

mq_client::mq_client():m_pCtrl(NULL)
{
}

//初始化
int mq_client::init(const char *path)
{
	MQLogger* logger = MQLogger::getInstance();
	MQControl *pCtrl = new MQControl();
	m_pCtrl = pCtrl;
	if ( pCtrl->init(path) ) {
		logger->info("mq_client_init is finished!");
		return MQCLIENT_OK;
	}else {
		logger->warning("mq_client_init failed!");
		return MQCLIENT_INIT_ERROR;
	}
}

//销毁
void mq_client::destory()
{
	delete (MQControl *)m_pCtrl;
}

//发送
int mq_client::send(const char* op,const char* content,const char* targetPath)
{
	MQLogger* logger = MQLogger::getInstance();
	if ( m_pCtrl == NULL ) {
		logger->warning("mq_client_send error! (mqclient don't init)");
		return MQCLIENT_INIT_ERROR;
	}

	if ( op == NULL || content == NULL){
		logger->warning("mq_client_send arguments error! (OP or content can't be empty)");
		return MQCLIENT_ARG_ERROR;
	}
	return ((MQControl *)m_pCtrl)->send(op, content, targetPath);
}

//添加侦听
int mq_client::add_listener(const char* listenPathTag, DestFunc dest_func)
{
	MQLogger* logger = MQLogger::getInstance();
	if ( m_pCtrl == NULL ) {
		logger->warning("mq_client_send error! (mqclient don't init)");
		return MQCLIENT_INIT_ERROR;
	}
	
	if (listenPathTag == NULL){
		listenPathTag = "DEFAULT";
	}
	m_destFunc = dest_func;
	return ((MQControl *)m_pCtrl)->add_listener(listenPathTag, this);
}

void mq_client::exec_callback(const char* op,const char* content,const char* respTarget)
{
	m_destFunc(this, op, content, respTarget);
}

//停止侦听
int mq_client::stop()
{
	MQLogger* logger = MQLogger::getInstance();
	if ( m_pCtrl == NULL  ){
		logger->warning("mq_stop error! (mqclient don't init)");
		return MQCLIENT_ARG_ERROR;
	}
	return ((MQControl *)m_pCtrl)->stop_listener();
}

//暂停侦听
int mq_client::pause(bool flag)
{
	MQLogger* logger = MQLogger::getInstance();
	if ( m_pCtrl == NULL  ){
		logger->warning("mq_pause error! (mqclient don't init)");
		return MQCLIENT_ARG_ERROR;
	}
	return ((MQControl *)m_pCtrl)->pause_listener(flag);
}

//对话等待
int mq_client::dialog_wait(const char* op,const char* content,const char* targetPath,int timeout,char **respContent)
{
	MQLogger* logger = MQLogger::getInstance();
	if ( m_pCtrl == NULL ) {
		logger->warning("mq_dialog_wait error! (mqclient don't init)");
		return MQCLIENT_INIT_ERROR;
	}
	if ( op == NULL || content == NULL || targetPath == NULL ){
		logger->warning("mq_dialog_wait arguments error! (msd、OP, content, targetPath can't be empty)");
		return MQCLIENT_ARG_ERROR;
	}
	return ((MQControl *)m_pCtrl)->dialog( op, content, targetPath, timeout, respContent);
}

//按日期切换日志文件
void mq_client::new_log()
{
	MQLogger::getInstance()->createLog();
}
