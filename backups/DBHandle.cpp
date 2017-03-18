/**********************************************************
 *  \file DBHandle.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
 **********************************************************/
#include "DBHandle.h"

const char* DBHandle::BASE_DB[] = { OWNER_DB, PLAN_DB, UNIT_DB, IDEA_DB, UNION_DB, SITE_DB, ADBAR_DB, ADTEMPLATE_DB, UNION_FILTER_DB };
const int DBHandle::base_size = sizeof(DBHandle::BASE_DB) / sizeof(*DBHandle::BASE_DB);

DBHandle::DBHandle() :
		_dbEnv(NULL) {
}

DBHandle::~DBHandle() {
	destory();
}

int DBHandle::config(string& path, string& host, uint32 port, uint32 timeout) {
	if (access(path.c_str(), 0) == -1) { //查看文件是不是存在
		m_logger->warning("Berkeley DB dir don't exist![dir=%s]", path.c_str());
		return -1;
	}

	m_base_path = path;

	cache_db = CacheHandle::Create(host, port, timeout);

	return open();
}

int DBHandle::open() {
	m_logger->debug("Berkeley DB init! [path=%s]", m_base_path.c_str());

	//环境参数
	u_int32_t env_flags = DB_CREATE | DB_INIT_MPOOL | DB_INIT_CDB | DB_THREAD; //CDS
	try {
		_dbEnv = new DbEnv(0);
		_dbEnv->open(m_base_path.c_str(), env_flags, 0); // 打开环境
	} catch (DbException &e) {
		m_logger->warning("bdb env open failed![path=%s,error = %s]", m_base_path.c_str(), e.what());
		return -3;
	} catch (std::exception &e) {
		m_logger->warning("bdb env open failed![path=%s,error = %s]", m_base_path.c_str(), e.what());
		return -4;
	}
	return 0;
}

//------------ 内部基础方法 ------------
int DBHandle::table_open(const char* db_name, Db*& _db, DBTYPE db_type, uint32 flags) {
	_db = NULL;
	int ret = 0;
	try {
		_db = new Db(_dbEnv, 0);
		if (flags != 0)
			_db->set_flags(flags);
		_db->open(NULL, db_name, NULL, db_type, DB_CREATE | DB_THREAD, 0664);
	} catch (DbException &e) {
		m_logger->warning("open table %s failed![error = %s]", db_name, e.what());
		delete _db;
		_db = NULL;
		ret = -1;
	} catch (std::exception &e) {
		m_logger->warning("open table %s failed![error = %s]", db_name, e.what());
		delete _db;
		_db = NULL;
		ret = -2;
	}
	return ret;
}

//功能:关闭指定的bdb表
int DBHandle::table_close(Db* _db) {
	const char *db_name = NULL, *db_name2 = NULL;
	try {
		_db->get_dbname(&db_name, &db_name2);
		_db->close(0);
		delete _db;
	} catch (DbException &e) {
		m_logger->warning("close table %s failed![error = %s]", db_name, e.what());
		return -1;
	} catch (std::exception &e) {
		m_logger->warning("close table %s failed![error = %s]", db_name, e.what());
		return -2;
	}
	return 0;
}

//功能:清空bdb表数据
int DBHandle::table_truncate(Db* _db) {
	const char *db_name = NULL, *db_name2 = NULL;
	u_int32_t count;
	try {
		_db->get_dbname(&db_name, &db_name2);
		_db->truncate(NULL, &count, 0);
	} catch (DbException &e) {
		m_logger->warning("table %s truncate failed![%s]", db_name, e.what());
		return -1;
	} catch (std::exception &e) {
		m_logger->warning("table %s truncate failed![%s]", db_name, e.what());
		return -2;
	}
	m_logger->debug("table %s truncate ok![affect rows=%u]", db_name, count);
	return 0;
}

//功能:同步bdb表数据到文件
int DBHandle::table_sync(Db* _db) {
	const char *db_name = NULL, *db_name2 = NULL;
	try {
		_db->get_dbname(&db_name, &db_name2);
		_db->sync(0);
	} catch (DbException &e) {
		m_logger->warning("table %s sync failed![%s]", db_name, e.what());
		return -1;
	} catch (std::exception &e) {
		m_logger->warning("table %s sync failed![%s]", db_name, e.what());
		return -2;
	}
	return 0;
}

//----------- 外部方法 ----------------
//功能:找开所有bdb表
int DBHandle::preOpen() {
	Db* _db;
	for (int i = 0; i < base_size; i++) {
		m_logger->debug("table_create [db_name=%s]", BASE_DB[i]);

		if (table_open(BASE_DB[i], _db) != 0) {
			return -1;
		}
		_handles[string(BASE_DB[i])] = _db;
	}
	m_logger->debug("open dbd table is ok!");
	return 0;
}

//功能:关闭所有的bdb表
int DBHandle::destory() {
	for (map<string, Db*>::iterator iter = _handles.begin(); iter != _handles.end(); iter++) {
		table_close((*iter).second);
	}
	_handles.clear();
	m_logger->debug("close dbd table is ok!");

	try {
		_dbEnv->close(0);
		delete _dbEnv;
	} catch (DbException &e) {
		m_logger->warning("db env close failed![%s]", e.what());
		return -1;
	} catch (std::exception &e) {
		m_logger->warning("db env close failed![%s]", e.what());
		return -2;
	}

	m_logger->debug("bdb destory over!");
	return 0;
}

//功能:找开指定的bdb表
Db* DBHandle::openTable(const char db_name[]) {
	map<string, Db*>::iterator iter = _handles.find(string(db_name));
	if (iter == _handles.end()) { //未找到
		Db* _db;
		if (table_open(db_name, _db) != 0) {
			return NULL;
		} else {
			_handles[string(db_name)] = _db;
			return _db;
		}
	} else {
		return (*iter).second;
	}
}
//功能:关闭指定的bdb表
void DBHandle::closeTable(const char db_name[]) {
	map<string, Db*>::iterator iter = _handles.find(string(db_name));
	if (iter != _handles.end()) { //未找到
		table_close((*iter).second);
		_handles.erase(iter);
	}
}

//功能:清空指定的bdb表数据
bool DBHandle::clearTable(const char db_name[]) {
	return (table_truncate(openTable(db_name)) == 0);
}

//功能:同步指定的bdb表数据到文件
bool DBHandle::syncTable(const char db_name[]) {
	return (table_sync(openTable(db_name)) == 0);
}

int DBHandle::clearDB() {
	char cmd[300];
	//清除BDB
	sprintf(cmd, "rm -f %s/__* %s/data/* %s/logs/* ", m_base_path.c_str(), m_base_path.c_str(), m_base_path.c_str());
	m_logger->debug("bdb clear cmd[%s]!", cmd);
	int ret = system(cmd);
	if (ret != 0) {
		m_logger->warning("bdb clear error ![ret=%d]", ret);
	}
	return ret;
}

/**
 * 获取缓存操作对象
 */
CacheHandle::ptr_t& DBHandle::getCache() {
	return cache_db;
}

/**
 * 获取计数器数量
 */
uint64 DBHandle::getCnt(uint32 key) {
	Connection::ptr_t c = cache_db->Get();
	if (!c->Isvalid()) {
		delete c;
		c = NULL;
		return 0;
	}

	uint64 ret = 0;
	Reply r = c->Run(Command("GET")(key));
	if (r.type == REDIS_REPLY_STRING) {
		ret = (uint64)atoi(r.str.c_str());
	}

	cache_db->Put(c);

	return ret;
}

/**
 * 计数器自增
 */
uint64 DBHandle::plus(uint32 name) {
	Connection::ptr_t c = cache_db->Get();
	if (!c->Isvalid()) {
		delete c;
		c = NULL;
		return 0;
	}

	uint64 ret = 0;
	Reply r = c->Run(Command("INCR")(name));
	if (r.type == REDIS_REPLY_INTEGER) {
		ret = (uint64)r.integer;
	}

	cache_db->Put(c);

	return ret;
}

/**
 * 清除数据
 */
bool DBHandle::clearCache() {
	return cache_db->Flush() == 0;
}
