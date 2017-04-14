/*
 *
 * Author: whm
 ***********************************************************/

#ifndef _MSG_DEALLER_H
#define _MSG_DEALLER_H

#include "common.h"
#include "DataVO.h"
#include <boost/property_tree/json_parser.hpp>
#include "DBHandle.h"

//处理器基类
class MsgDealler: public pssys::Logging {
public:
	virtual ~MsgDealler() {
	}
	;
	virtual int doMsg(string& op, const ptree& pt)=0;
	void parseSubTree(const ptree& pt, const char* tag, string op);
};

//处理器工厂
class MsgDeallerFactory {

public:
	static MsgDealler* getDealler(string& op);
};

//--------------------------------------
//哑元
class DummyDealler: public MsgDealler {
public:
	virtual int doMsg(string& op, const ptree& pt);
};

class OwnerDealler: public MsgDealler {
public:
	virtual int doMsg(string& op, const ptree& pt);
};

class PlanDealler: public MsgDealler {
public:
	virtual int doMsg(string& op, const ptree& pt);
};

class UnitDealler: public MsgDealler {
public:
	virtual int doMsg(string& op, const ptree& pt);
};

class IdeaDealler: public MsgDealler {
public:
	virtual int doMsg(string& op, const ptree& pt);
};

class UnionDealler: public MsgDealler {
public:
	virtual int doMsg(string& op, const ptree& pt);
};

class SiteDealler: public MsgDealler {
public:
	virtual int doMsg(string& op, const ptree& pt);
};

class AdbarDealler: public MsgDealler {
public:
	virtual int doMsg(string& op, const ptree& pt);
};

class TargetStopDealler: public MsgDealler {
public:
	virtual int doMsg(string& op, const ptree& pt);
};

//查询
class QueryDealler: public MsgDealler {
	string respMsg;
public:
	inline string getResp() {
		return respMsg;
	}
	;
	virtual int doMsg(string& op, const ptree& pt);

private:
	int searchOwner(string& op, const ptree& pt);
	int searchPlan(string& op, const ptree& pt);
	int searchUnit(string& op, const ptree& pt);
	int searchIdea(string& op, const ptree& pt);
	int searchUnion(string& op, const ptree& pt);
	int searchSite(string& op, const ptree& pt);
	int searchAdbar(string& op, const ptree& pt);
};

#endif
