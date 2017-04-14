#include "MsgReciver.h"
#include "StrException.h"

using namespace pssys;

int cb_func(mq_client *client, const char* op, const char* content, const char* respTarget) {
	Logger *logger = new Logger(false);

	int ret = 0;
	MsgDealler* dealler = NULL;
	try {
		ptree pt;
		string opStr = op;
		try {
			stringstream stream(content);
			json_parser::read_json<ptree>(stream, pt);
		} catch (...) {
			throw StrException(-100, "message content isn't json format! [source=%s, op=%s]", respTarget, op);
		}
		if (opStr == "QUERY") {
			QueryDealler* qdealler = new QueryDealler();
			dealler = qdealler;
			qdealler->doMsg(opStr, pt);

			//返回消息
			if (client->send(op, qdealler->getResp().c_str(), respTarget) != MQCLIENT_OK) {
				throw StrException(-500, "message response failed!");
			}
		} else {
			dealler = MsgDeallerFactory::getDealler(opStr);
			dealler->doMsg(opStr, pt);
		}
	} catch (StrException &e) {
		logger->warning("消息处理异常.(%d)%s[op=%s\n content=%s\n]！！！", e.getCode(), e.getMsg().c_str(), op, content);
		ret = -1;
	} catch (...) {
		logger->warning("消息异常[source=%s\n op=%s\n content=%s\n]！！！", respTarget, op, content);
		ret = -2;
	}
	delete dealler;
	delete logger;
	return ret;
}

//启动消息队列侦听
int MsgReciver::start() {
	return add_listener(NULL, &cb_func);
}
