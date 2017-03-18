/**********************************************************
 *  \file DBHandle.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
 **********************************************************/
#ifndef _DB_HANDLE_H
#define _DB_HANDLE_H

#include "common.h"
#include "DataLoader.h"
#include "CacheHandle.h"
#include <db_cxx.h>
#include <boost/algorithm/string.hpp>



class DBHandle: public lpsys::Singleton<DBHandle>, public lpsys::Logging {
	static const char* BASE_DB[];
	static const int base_size;

private:
	map<string, Db*> _handles;

	string m_base_path;
	DbEnv *_dbEnv;

	CacheHandle::ptr_t cache_db;

	int table_open(const char* db_name, Db*& _db, DBTYPE db_type = DB_BTREE, uint32 flags = 0);
	int table_close(Db* _db);
	int table_truncate(Db* _db);
	int table_sync(Db* _db);
public:
	DBHandle();
	~DBHandle();

	int config(string& path, string& host, uint32 port, uint32 timeout);
	int preOpen();
	int open();
	int destory();

	inline const char* getDBPath() {
		return m_base_path.c_str();
	}

	Db* openTable(const char db_name[]);
	void closeTable(const char db_name[]);bool clearTable(const char db_name[]);bool syncTable(const char db_name[]);

	int clearDB();

	CacheHandle::ptr_t& getCache();
	uint64 getCnt(uint32 key);
	uint64 plus(uint32 key);
	bool clearCache();

	//一组DBT设置类
	template<class K, class V> struct dbt_function {
		void setK(Dbt& _dbt, K& k) {
			_dbt.set_data(&(k));
			_dbt.set_size(sizeof(K));
		}

		void setV(Dbt& _dbt, V& v) {
			_dbt.set_data(&(v));
			_dbt.set_size(sizeof(V));
		}

		void getK(Dbt& _dbt, K& k) {
			k = (K*) (_dbt.get_data());
		}

		void getV(Dbt& _dbt, V& v) {
			memcpy(&v, _dbt.get_data(), _dbt.get_size());
		}

		void update(V& oldvo, V& newvo) {
			oldvo = newvo;
		}

		void destory() {
		}
	};

	template<class V> struct stringKey_function: public dbt_function<string, V> {
		char strKey[500];
		void setK(Dbt& _dbt, string& k) {
			snprintf(strKey, sizeof(strKey), "%s", k.c_str());
			_dbt.set_data(strKey);
			_dbt.set_size(k.size() + 1);
		}
	};

	//变态的g++要求只能在.h中
	template<class K, class V, typename F> bool setVO(K id, V& vo, const char* db_name, F _f) {
		Db* _db = openTable(db_name);
		if (_db == NULL) {
			return false;
		}

		bool iRet = false;
		try {
			Dbt key;
			_f.setK(key, id);

			Dbt value;
			_f.setV(value, vo);

			int ret = _db->put( NULL, &key, &value, 0);
			if (ret == 0) {
				iRet = true;
			} else {
				m_logger->warning("table %s insert failed![%s]", db_name, db_strerror(ret));
			}
		} catch (DbException &e) {
			m_logger->warning("table %s insert failed![%s]", db_name, e.what());
		} catch (std::exception &e) {
			m_logger->warning("table %s insert failed![%s]", db_name, e.what());
		}
		_f.destory();
		return iRet;
	}

	template<class K, class V> bool getVO(K id, V& vo, const char* db_name) {
		return getVO(id, vo, db_name, dbt_function<K, V>());
	}

	template<class K, class V, typename F> bool getVO(K id, V& vo, const char* db_name, F _f) {
		Db* _db = openTable(db_name);
		if (_db == NULL) {
			return false;
		}

		bool iRet;
		Dbt value;
		try {
			Dbt key;
			_f.setK(key, id);

			value.set_flags(DB_DBT_MALLOC);

			int ret = _db->get(NULL, &key, &value, 0);
			if (ret == 0) {
				_f.getV(value, vo);
				free(value.get_data());
				iRet = true;
			} else if (ret == DB_NOTFOUND) {
				iRet = false;
			} else {
				m_logger->warning("table %s get data failed![error:%s]", db_name, db_strerror(ret));
				iRet = false;
			}
		} catch (DbException &e) {
			m_logger->warning("table %s get failed![%s]", db_name, e.what());
			iRet = false;
		} catch (std::exception &e) {
			m_logger->warning("table %s get failed![%s]", db_name, e.what());
			iRet = false;
		}
		_f.destory();
		return iRet;
	}

	template<class K, class V> struct dbt_getter {
		map<K, V>* m_plist;
		dbt_getter() {
			m_plist = new map<K, V>();
		}

		void load(K* _id, V* _po) {
			(*m_plist)[*_id] = *_po;
		}
	};

	//注意：isChg==true时，不允许变更变形数据的长度。
	template<class K, class V, class F> bool walkPO(const char db_name[], F _f,
	bool isChg = false) {
		Db* _db = openTable(db_name);
		if (_db == NULL) {
			return false;
		}

		bool iRet = false;
		int ret;
		Dbc* cur = NULL;
		try {
			int rows_cnt = 0;
			_db->cursor(NULL, &cur, 0);

			Dbt key, value;

			//搜索记录
			while ((ret = cur->get(&key, &value, DB_NEXT)) == 0) {
				_f.load((K *) (key.get_data()), (V *) (value.get_data()));
				if (isChg) {
					cur->put(&key, &value, DB_CURRENT);
				}
				rows_cnt++;
			}
			m_logger->info("table %s total record number %d.", db_name, rows_cnt);
			iRet = true;
		} catch (DbException &e) {
			m_logger->warning("table %s get failed![%s]", db_name, e.what());
		} catch (std::exception &e) {
			m_logger->warning("table %s get failed![%s]", db_name, e.what());
		}

		//关闭游标
		if (cur != NULL) {
			try {
				cur->close();
			} catch (DbException &e) {
				m_logger->warning("table %s get failed![%s]", db_name, e.what());
			} catch (std::exception &e) {
				m_logger->warning("table %s get failed![%s]", db_name, e.what());
			}
		}
		//保存
		table_sync(_db);

		return iRet;
	}

	template<class K, class V, class F> bool overwrite(const char db_name[], map<K, V>& list, F _f) {
		Db* _db = openTable(db_name);
		if (_db == NULL) {
			return false;
		}

		//添加新数据
		int ret;
		int i = 0;
		for (typename map<K, V>::iterator iter = list.begin(); iter != list.end(); iter++, i++) {
			try {
				Dbt key;
				K k = (*iter).first;
				_f.setK(key, k);

				Dbt value;
				_f.setV(value, (*iter).second);

				ret = _db->put(NULL, &key, &value, 0);
				if (ret != 0) {
					m_logger->warning("table %s insert failed!(line=%d)[error:%s]", db_name, i, db_strerror(ret));
				}
			} catch (DbException &e) {
				m_logger->warning("table %s get failed![%s]", db_name, e.what());
			} catch (std::exception &e) {
				m_logger->warning("table %s get failed![%s]", db_name, e.what());
			}
			_f.destory();
		}

		return true;
	}

	template<class K, class V> bool change(const char db_name[], map<K, V>& list) {
		return change(db_name, list, dbt_function<K, V>());
	}
	template<class K, class V, class F> bool change(const char db_name[], map<K, V>& list, F _f) {
		Db* _db = openTable(db_name);
		if (_db == NULL) {
			return false;
		}

		//添加新数据
		int ret;
		int i = 0;
		for (typename map<K, V>::iterator iter = list.begin(); iter != list.end(); iter++, i++) {
			try {
				Dbt key;
				K k = (*iter).first;
				_f.setK(key, k);

				Dbt value;
				value.set_flags(DB_DBT_MALLOC);

				ret = _db->get(NULL, &key, &value, 0);
				if (ret == DB_NOTFOUND) { //新增
					_f.setV(value, (*iter).second);
				} else if (ret != 0) { //错误
					m_logger->warning("table %s get data failed![error:%s]", db_name, db_strerror(ret));
				} else { //替换
					V v;
					_f.getV(value, v);
					free(value.get_data());
					_f.update(v, (*iter).second);
					_f.setV(value, v);
				}
				ret = _db->put(NULL, &key, &value, 0);
				if (ret != 0) {
					m_logger->warning("table %s update failed!(line=%d)[error:%s]", db_name, i, db_strerror(ret));
				}
			} catch (DbException &e) {
				m_logger->warning("table %s get failed![%s]", db_name, e.what());
			} catch (std::exception &e) {
				m_logger->warning("table %s get failed![%s]", db_name, e.what());
			}
			_f.destory();
		}

		return true;
	}

	//--------------- VO <-> PO function -----------------
	static string outputList(list<uint32>& _list) {
		ostringstream oss;
		for (list<uint32>::iterator iter = _list.begin(); iter != _list.end(); iter++) {
			oss << (*iter) << ",";
		}
		return oss.str();
	}

	static void inputList(char str[], list<uint32>& _list) {
		vector<string> parts;
		boost::split(parts, str, boost::is_any_of(","));
		uint32 id;
		for (vector<string>::iterator iter = parts.begin(); iter != parts.end(); iter++) {
			id = (uint32) atoi(iter->c_str());
			if (id != 0) {
				_list.push_back(id);
			}
		}
	}

	template<class K, class V> static string outputMap(map<K, V>& _m) {
		ostringstream oss;
		for (typename map<K, V>::iterator iter = _m.begin(); iter != _m.end(); iter++) {
			oss << iter->first << ":";
			oss << iter->second << ";";
		}
		return oss.str();
	}

	struct string2uint32 {
		uint32 operator()(string& str) {
			return (uint32) atoi(str.c_str());
		}
	};

	struct string2string {
		string& operator()(string& str) {
			return str;
		}
	};

	template<class K, class V> static void inputMap(char str[], map<K, V>& _m) {
		inputMap(str, _m, string2string(), string2string());
	}
	template<class K, class V, typename F, typename G> static void inputMap(char str[], map<K, V>& _m, F _f, G _g) {
		vector<string> pairs;
		boost::split(pairs, str, boost::is_any_of(";"));
		for (vector<string>::iterator iter = pairs.begin(); iter != pairs.end(); iter++) {
			size_t found = iter->find(":");
			if (found != string::npos) {
				string k = iter->substr(0, found);
				string v = iter->substr(found + 1);
				_m[_g(k)] = _f(v);
			}
		}
	}

	template<typename T> static string outputSet(set<T>& _set) {
		ostringstream oss;
		for (typename set<T>::iterator iter = _set.begin(); iter != _set.end(); iter++) {
			oss << (*iter) << ",";
		}
		return oss.str();
	}

	static void inputSet(char str[], set<uint32>& _set) {
		vector<string> parts;
		boost::split(parts, str, boost::is_any_of(","));
		for (vector<string>::iterator iter = parts.begin(); iter != parts.end(); iter++) {
			uint32 id = atoi(iter->c_str());
			if (id > 0)
				_set.insert(id);
		}
	}

	static string outputMapList(map<uint32, list<uint32> > &_m) {
		map<uint32, string> mp;
		for (map<uint32, list<uint32> >::iterator iter = _m.begin(); iter != _m.end(); iter++) {
			mp[iter->first] = outputList(iter->second);
		}
		return outputMap<uint32, string>(mp);
	}

	static void intputMapint(char src[], map<uint32, uint32> &_m) {
		inputMap<uint32, uint32>(src, _m, string2uint32(), string2uint32());
	}

	static void intputMap2(char src[], map<uint32, string>& _m) {
		inputMap<uint32, string>(src, _m, string2string(), string2uint32());
	}

	static void inputMapList(char src[], map<uint32, list<uint32> > &_m) {
		map<string, string> mp;
		inputMap<string, string>(src, mp);
		for (map<string, string>::iterator iter = mp.begin(); iter != mp.end(); iter++) {
			list<uint32> &v = _m[(uint32) atoi(iter->first.c_str())];
			inputList(const_cast<char*>((iter->second).c_str()), v);
		}
	}
};

#endif

#ifndef _DB_FUNC_H
#define _DB_FUNC_H

//------------------------------------------

struct owner_function {
	void setK(Dbt& _dbt, uint32& k) {
		_dbt.set_data(&(k));
		_dbt.set_size(sizeof(uint32));
	}

	void setV(Dbt& _dbt, OwnerVO& vo) {
		_po = (OwnerPO*) new char[sizeof(OwnerPO)];
		_po->oid = vo.oid;
		_po->status = vo.status;
		_po->target_stat = vo.target_stat;
		_po->balance = vo.balance;
		_po->today_cost = vo.today_cost;
		strncpy(_po->domain, vo.domain, DOMAIN_MAXLEN);
		_po->domain[DOMAIN_MAXLEN] = '\0';
		_po->vip = vo.vip;
		_po->special_industry = vo.special_industry;
		_po->target_industry = vo.target_industry;

		_dbt.set_data(_po);
		_dbt.set_size(sizeof(OwnerPO));
	}

	void getV(Dbt& _dbt, OwnerVO& vo) {
		_po = (OwnerPO*) (_dbt.get_data());
		vo.oid = _po->oid;
		vo.status = _po->status;
		vo.target_stat = _po->target_stat;
		vo.balance = _po->balance;
		vo.today_cost = _po->today_cost;
		strcpy(vo.domain, _po->domain);
		vo.vip = _po->vip;
		vo.special_industry = _po->special_industry;
		vo.target_industry = _po->target_industry;
		_po = NULL;
	}

	OwnerPO* _po;

	owner_function() :
			_po(NULL) {
	}

	void destory() {
		if (_po != NULL)
			delete[] _po;
	}
};

struct adtemplate_getter {
	map<uint32, AdTemplateVO>* m_plist;
	adtemplate_getter() {
		m_plist = new map<uint32, AdTemplateVO>();
	}

	void load(uint32* _id, AdTemplatePO* _po) {
		AdTemplateVO& vo = (*m_plist)[*_id];
		vo.id = _po->id;
		vo.max_unit = _po->max_unit;

		int len = sizeof(AdTemplatePO);
		for (int i = 0; i < 4; i++) {
			vo.strTemplate[i + 1] = string(((char*) _po) + len);
			len += _po->strlen[i];
		}
	}
};


#endif
