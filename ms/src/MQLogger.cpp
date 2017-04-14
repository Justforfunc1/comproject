#include "MQLogger.h"

//public
MQLogger* MQLogger::_instance = NULL;
	
void MQLogger::formatTimeStr(char *buf) {
	time_t rawtime;
	struct tm* curtime;

	time(&rawtime);
	curtime = localtime(&rawtime);

	char tmp[10];
	char *p = strstr(buf,"YYYY");
	if ( p !=NULL ){
		sprintf(tmp,"%04d",curtime->tm_year + 1900);
		strncpy(p,tmp,4);
	}

	p = strstr(buf,"MM");
	if ( p !=NULL ){
		sprintf(tmp,"%02d",curtime->tm_mon + 1);
		strncpy(p,tmp,2);
	}

	p = strstr(buf,"DD");
	if ( p !=NULL ){
		sprintf(tmp,"%02d", curtime->tm_mday);
		strncpy(p,tmp,2);
	}
	
	p = strstr(buf,"HH");
	if ( p !=NULL ){
		sprintf(tmp,"%02d", curtime->tm_hour);
		strncpy(p,tmp,2);
	}

	p = strstr(buf,"MI");
	if ( p !=NULL ){
		sprintf(tmp,"%02d", curtime->tm_min);
		strncpy(p,tmp,2);
	}
	
	p = strstr(buf,"SS");
	if ( p !=NULL ){
		sprintf(tmp,"%02d",curtime->tm_sec);
		strncpy(p,tmp,2);
	}
}

void MQLogger::createLog()
{
	if( m_logName[0] == '\0' ){
		m_logfile = stdout;
		return;
	}
	if (m_logfile != NULL && m_logfile != stdout){
		fclose(m_logfile);
	}
	//构建文件名
	char buf[100]; 
	strcpy(buf,m_logName);
	formatTimeStr(buf);
	m_logfile = fopen(buf,"a+");
	if (m_logfile == NULL )
		printf("create log[%s] error![errno=%d]",buf,errno);
}

bool MQLogger::init(const char* fname, const char* group_name){

	boost::property_tree::ptree pt;
	//读取配置文件
	try {
		boost::property_tree::ini_parser::read_ini(fname, pt);
	}catch(std::exception & e) {
		printf("read profile %s[%s]failed!(%s)",fname,group_name,e.what());
		return false;
	}
	
	string groupName = group_name + string(".");
	
	string level;
	try {
		level = pt.get<string>(groupName+"level");
	} catch(exception & e) {
		printf("profile %s lost[%s][%s]. use defalut value!",fname,group_name,"level");
	}
	
	if( level == "debug"){
        m_level = LOG_LEVEL_DEBUG;
    } else if(level == "info") {
		m_level = LOG_LEVEL_INFO;
	} else if(level == "warning") {
		m_level = LOG_LEVEL_WARNING;
	} else if(level == "error"){
		m_level = LOG_LEVEL_ERROR;
	}else
		m_level = LOG_LEVEL_INFO;
	
	string loggerName="";
	try {
		loggerName = pt.get<string>(groupName+"logger");
	} catch(exception & e) {
		printf("profile %s lost[%s][%s]. use defalut value!",fname,group_name,"logger");
	}
	
	if( loggerName == "" ){
		m_logName[0]= '\0';
	}else {
		strncpy(m_logName,loggerName.c_str(),99);
	}
	return true;
}

void MQLogger::debug(const char *format, ...){
	if ( m_level >= LOG_LEVEL_DEBUG ) {
		va_list args;
		va_start(args, format);
		logPrint( LOG_LEVEL_DEBUG, format, args);
		va_end(args);
	}
}

void MQLogger::info(const char *format, ...){
	if ( m_level >= LOG_LEVEL_INFO ) {
		va_list args;
		va_start(args, format);
		logPrint( LOG_LEVEL_INFO, format, args);
		va_end(args);
	}
}

void MQLogger::warning(const char *format, ...){
	if ( m_level >= LOG_LEVEL_WARNING ) {
		va_list args;
		va_start (args, format);
		logPrint(LOG_LEVEL_WARNING,format, args);
		va_end(args);
	}
}

void MQLogger::error(const char *format, ...){
	if ( m_level >= LOG_LEVEL_ERROR ) {
		va_list args;
		va_start(args, format);
		logPrint(LOG_LEVEL_ERROR,format, args);
		va_end(args);
		exit(-1);
	}
}

void MQLogger::logPrint(int level,const char *format, va_list args)
{
	time_t rawtime;
	struct tm* curtime;
	time(&rawtime);
	curtime = localtime(&rawtime);
	
	boost::mutex::scoped_lock lock(m_mutex);
	fprintf(m_logfile,"%4d-%02d-%02d %02d:%02d:%02d [%s] ", curtime->tm_year + 1900, curtime->tm_mon + 1, curtime->tm_mday,
												 curtime->tm_hour, curtime->tm_min, curtime->tm_sec, LOG_LEVEL_TAG[level]);
	vfprintf( m_logfile, format, args);
	fprintf( m_logfile, "\n");
	fflush(m_logfile);
}
