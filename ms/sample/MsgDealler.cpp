/*************************
 * StrException：
 *		-100 数据解析错误
 *		-200 数据缺失
 *		-300 处理错误
 *		-400 数据一致性
 *		-500 返回报文错误
 *************************/
#include "MsgDealler.h"
#include "Tool.h"
#include "MemoryObject.h"
#include "StrException.h"

MsgDealler* MsgDeallerFactory::getDealler(string& op) {
	MsgDealler* dealler = NULL;
	if (op == "OWNER_ACTIVE" || op == "OWNER_DEL") {
		dealler = new OwnerDealler();
	} else if (op == "PLAN_ACTIVE" || op == "PLAN_DEL") {
		dealler = new PlanDealler();
	} else if (op == "UNIT_ACTIVE" || op == "UNIT_DEL") {
		dealler = new UnitDealler();
	} else if (op == "IDEA_ACTIVE" || op == "IDEA_DEL") {
		dealler = new IdeaDealler();
	} else if (op == "UNION_ACTIVE" || op == "UNION_DEL") {
		dealler = new UnionDealler();
	} else if (op == "SITE_ACTIVE" || op == "SITE_DEL") {
		dealler = new SiteDealler();
	} else if (op == "ADBAR_ACTIVE" || op == "ADBAR_DEL") {
		dealler = new AdbarDealler();
	} else if (op == "TARGET_STOP") {
		dealler = new TargetStopDealler();
	} else {
		dealler = new DummyDealler();
	}
	return dealler;
}

void MsgDealler::parseSubTree(const ptree& pt, const char* tag, string op) {
	boost::optional<const ptree&> tree = pt.get_child_optional(tag);
	if (tree) {
		MsgDealler* dealler = MsgDeallerFactory::getDealler(op);
		string err_msg;
		int p_no = 0;
		for (ptree::const_iterator iter = tree.get().begin(); iter != tree.get().end(); iter++) {
			try {
				dealler->doMsg(op, iter->second.get_child(""));
			} catch (...) {
				char tmp[100];
				sprintf(tmp, "-->%s message is invaild![line:%d]", op.c_str(), p_no);
				err_msg += tmp;
			}
			p_no++;
		}
		delete dealler;

		if (err_msg.size() > 0)
			throw StrException(-100, "%s", err_msg.c_str());
	}
}

int DummyDealler::doMsg(string& op, const ptree& pt) {
	m_logger->info("Don't deal with message![op=%s ]", op.c_str());
	return 0;
}

int OwnerDealler::doMsg(string& op, const ptree& pt) {
	if (op == "OWNER_ACTIVE") {
		m_logger->debug("OWNER_ACTIVE domsg start");

		OwnerVO vo;
		vo.oid = pt.get<uint32>("oid");
		vo.status = pt.get<int8>("owner_stat");
		vo.target_stat = (vo.status == STATUS_OK) ? 1 : 0;

		vo.balance = 0;
		vo.today_cost = 0;

		string domain = pt.get<string>("domain");
		strncpy(vo.domain, domain.c_str(), DOMAIN_MAXLEN);
		vo.domain[DOMAIN_MAXLEN] = '\0';

		vo.vip = pt.get<uint8>("vip");

		vo.special_industry = 0;
		string strSpecialIndustry = pt.get<string>("special_industry");
		if (strSpecialIndustry != "0") {
			const ptree &subtree = pt.get_child("special_industry");
			for (ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++) {
				uint32 sp_id = iter->second.get < uint32 > ("");
				if (sp_id != 0) {
					vo.special_industry |= (1 << (sp_id - 1));
				}
			}
		}

		vo.target_industry = pt.get<uint32>("company_industry"); //原用于target_industry属性,2015.04.03 edit by devon

		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint32, OwnerVO>& mo_owner = pMOm->m_ownerMO.getItemsA();
		map<uint32, OwnerVO>::iterator iter = mo_owner.find(vo.oid);
		if (iter != mo_owner.end()) {
			OwnerVO& owner = iter->second;
			owner.status = vo.status;
			owner.target_stat = vo.target_stat;
			strcpy(owner.domain, vo.domain);
			owner.vip = vo.vip;
			owner.special_industry = vo.special_industry;
			owner.target_industry = vo.target_industry;
			vo.balance = owner.balance;
			vo.today_cost = owner.today_cost;
		}
		//变更备份
		map<uint32, OwnerVO>& mo_owner2 = pMOm->m_ownerMO.getItemsB();
		map<uint32, OwnerVO>::iterator iter2 = mo_owner2.find(vo.oid);
		if (iter2 != mo_owner2.end()) {
			OwnerVO& owner = iter2->second;
			owner.status = vo.status;
			owner.target_stat = vo.target_stat;
			strcpy(owner.domain, vo.domain);
			owner.vip = vo.vip;
			owner.special_industry = vo.special_industry;
			owner.target_industry = vo.target_industry;
			vo.balance = owner.balance;
			vo.today_cost = owner.today_cost;
		} else {
			mo_owner2[vo.oid] = vo;
		}
		//变更数据库
		DBHandle* handle = DBHandle::getInstance();
		handle->setVO(vo.oid, vo, OWNER_DB, owner_function());
		handle->syncTable(OWNER_DB);
		m_logger->debug("active owner success,oid=%u", vo.oid);

		//嵌套消息处理
		parseSubTree(pt, "plans", "PLAN_ACTIVE");
	} else if (op == "OWNER_DEL") {
		OwnerVO vo;
		vo.oid = pt.get<uint32>("oid");
		vo.status = pt.get<int8>("owner_stat");
		vo.target_stat = 0;

		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint32, OwnerVO>& mo_owner = pMOm->m_ownerMO.getItemsA();
		map<uint32, OwnerVO>::iterator iter = mo_owner.find(vo.oid);
		if (iter != mo_owner.end()) {
			OwnerVO& owner = iter->second;
			owner.status = vo.status;
			owner.target_stat = vo.target_stat;
		}
		//变更备份
		map<uint32, OwnerVO>& mo_owner2 = pMOm->m_ownerMO.getItemsB();
		map<uint32, OwnerVO>::iterator iter2 = mo_owner2.find(vo.oid);
		if (iter2 != mo_owner2.end()) {
			OwnerVO& owner = iter2->second;
			owner.status = vo.status;
			owner.target_stat = vo.target_stat;
			//保证非空
			strcpy(vo.domain, owner.domain);
			vo.vip = owner.vip;
			vo.special_industry = owner.special_industry;
			vo.target_industry = owner.target_industry;
			vo.balance = owner.balance;
			vo.today_cost = owner.today_cost;

			//变更数据库
			DBHandle* handle = DBHandle::getInstance();
			handle->setVO(vo.oid, vo, OWNER_DB, owner_function());
			handle->syncTable(OWNER_DB);
		}
	}
	return 0;
}

int PlanDealler::doMsg(string& op, const ptree& pt) {
	if (op == "PLAN_ACTIVE") {
		m_logger->debug("PLAN_ACTIVE domsg start");

		PlanVO vo;
		vo.plan_id = pt.get<uint32>("plan_id");
		vo.oid = pt.get<uint32>("oid");
		vo.status = pt.get<int8>("plan_stat");
		vo.target_stat = (vo.status == STATUS_OK) ? 1 : 0;
		vo.type = pt.get<uint16>("plan_type");
		vo.price_mode = pt.get<uint16>("price_mode");

		string week = Tool::getWeek();
		string strSchedule = pt.get<string>("schedule");
		if (strSchedule != "0") {
			const ptree &subtree = pt.get_child("schedule");
			for (ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++) {
				if (iter->first == week) {
					const ptree &subtree2 = iter->second;
					for (ptree::const_iterator iter2 = subtree2.begin(); iter2 != subtree2.end(); iter2++) {
						vo.schedule.push_back(iter2->second.get < uint32 > (""));
					}
				}
			}
			if (vo.schedule.size() == 0) { //无当天描述，禁止投放
				vo.schedule.push_back(100);
			}
		}

		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint32, PlanVO>& mo_plan = pMOm->m_planMO.getItemsA();
		map<uint32, PlanVO>::iterator iter = mo_plan.find(vo.plan_id);
		if (iter != mo_plan.end()) {
			PlanVO& plan = iter->second;
			plan.status = vo.status;
			plan.target_stat = vo.target_stat;
			plan.type = vo.type;
			plan.price_mode = vo.price_mode;
			vo.budget = plan.budget; //消息中无预算信息，只有快照中有, 保持原值
		}
		//变更备份
		map<uint32, PlanVO>& mo_plan2 = pMOm->m_planMO.getItemsB();
		map<uint32, PlanVO>::iterator iter2 = mo_plan2.find(vo.plan_id);
		if (iter2 != mo_plan2.end()) {
			PlanVO& plan = iter2->second;
			plan.status = vo.status;
			plan.target_stat = vo.target_stat;
			plan.type = vo.type;
			plan.price_mode = vo.price_mode;
			vo.budget = plan.budget; //消息中无预算信息，只有快照中有, 保持原值
			plan.schedule = vo.schedule;
			plan.allow_list = vo.allow_list;
		} else {
			mo_plan2[vo.plan_id] = vo;
		}
		//变更数据库
		DBHandle* handle = DBHandle::getInstance();
		handle->setVO(vo.plan_id, vo, PLAN_DB, plan_function());
		handle->syncTable(PLAN_DB);
		m_logger->debug("active plan success,plan_id=%u", vo.plan_id);

		//嵌套消息处理
		parseSubTree(pt, "units", "UNIT_ACTIVE");
	} else if (op == "PLAN_DEL") {
		PlanVO vo;
		vo.plan_id = pt.get<uint32>("plan_id");
		vo.status = pt.get<int8>("plan_stat");
		vo.target_stat = 0;

		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint32, PlanVO>& mo_plan = pMOm->m_planMO.getItemsA();
		map<uint32, PlanVO>::iterator iter = mo_plan.find(vo.plan_id);
		if (iter != mo_plan.end()) {
			PlanVO& plan = iter->second;
			plan.status = vo.status;
			plan.target_stat = vo.target_stat;
		}
		//变更备份
		map<uint32, PlanVO>& mo_plan2 = pMOm->m_planMO.getItemsB();
		map<uint32, PlanVO>::iterator iter2 = mo_plan2.find(vo.plan_id);
		if (iter2 != mo_plan2.end()) {
			PlanVO& plan = iter2->second;
			plan.status = vo.status;
			plan.target_stat = vo.target_stat;
			//保证非空
			vo.oid = plan.oid;
			vo.price_mode = plan.price_mode;
			vo.budget = plan.budget;
			//vo.weight = plan.weight;
			vo.schedule = plan.schedule;
			vo.allow_list = plan.allow_list;

			//变更数据库
			DBHandle* handle = DBHandle::getInstance();
			handle->setVO(vo.plan_id, vo, PLAN_DB, plan_function());
			handle->syncTable(PLAN_DB);
		}
	}
	return 0;
}

int UnitDealler::doMsg(string& op, const ptree& pt) {
	if (op == "UNIT_ACTIVE") {
		m_logger->debug("UNIT_ACTIVE domsg start");

		UnitVO vo;
		vo.unit_id = pt.get<uint32>("unit_id");
		vo.plan_id = pt.get<uint32>("plan_id");
		vo.oid = pt.get<uint32>("oid");
		vo.type = pt.get<uint16>("unit_type");
		vo.status = pt.get<int8>("unit_stat");
		vo.target_stat = (vo.status == 1) ? 1 : 0;
		vo.price = pt.get<uint64>("price");
		vo.isp = -1;
		vo.frequency = pt.get<uint32>("frequency");

		vo.weight = 1; //待调整

		string strArea = pt.get<string>("area");
		if (strArea != "0") {
			const ptree &subtree = pt.get_child("area");
			for (ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++) {
				const ptree &subtree2 = iter->second;
				for (ptree::const_iterator iter2 = subtree2.begin(); iter2 != subtree2.end(); iter2++) {
					vo.area.push_back(iter2->second.get < uint32 > (""));
				}
			}
		}

		string strSiteIndustry = pt.get<string>("site_industry");
		if (strSiteIndustry != "0") {
			const ptree &subtree = pt.get_child("site_industry");
			for (ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++) {
				const ptree &subtree2 = iter->second;
				for (ptree::const_iterator iter2 = subtree2.begin(); iter2 != subtree2.end(); iter2++) {
					vo.site_industry.push_back(iter2->second.get < uint32 > (""));
				}
			}
		}

		string strShowSite = pt.get<string>("show_site");
		if (strShowSite != "0") {
			const ptree &subtree = pt.get_child("show_site");
			for (ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++) {
				uint32 show_site = iter->second.get < uint32 > ("");
				if (show_site > 0)
					vo.show_site.push_back(show_site);
			}
		}

		string strDevice = pt.get<string>("device");
		if (strDevice != "0") {
			const ptree &subtree = pt.get_child("device");
			for (ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++) {
				uint32 device = iter->second.get < uint32 > ("");
				if (device > 0)
					vo.device.push_back(device);
			}
		}

		string strBrowser = pt.get<string>("browser");
		if (strBrowser != "0") {
			const ptree &subtree = pt.get_child("browser");
			for (ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++) {
				uint32 browser = iter->second.get < uint32 > ("");
				if (browser > 0)
					vo.browser.push_back(browser);
			}
		}

		string strCustomize = pt.get<string>("customize");
			if(strCustomize != "0"){
			const ptree &subtree = pt.get_child("customize");
			for (ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++) {
				vo.customize[iter->first] = subtree.get<string>(iter->first);
			}
		}

		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint32, UnitVO>& mo_unit = pMOm->m_unitMO.getItemsA();
		map<uint32, UnitVO>::iterator iter = mo_unit.find(vo.unit_id);
		if (iter != mo_unit.end()) {
			UnitVO& unit = iter->second;
			unit.status = vo.status;
			unit.target_stat = vo.target_stat;
			unit.type = vo.type;
			unit.price = vo.price;
			unit.weight = vo.weight;
			unit.isp = vo.isp;
			unit.frequency = vo.frequency;
			//对area、site_industry、show_site,devcie,os,browser,customize 采用替换法处理，以防止coredump
		}
		//变更备份
		map<uint32, UnitVO>& mo_unit2 = pMOm->m_unitMO.getItemsB();
		map<uint32, UnitVO>::iterator iter2 = mo_unit2.find(vo.unit_id);
		if (iter2 != mo_unit2.end()) {
			UnitVO& unit = iter2->second;
			unit.status = vo.status;
			unit.target_stat = vo.target_stat;
			unit.type = vo.type;
			unit.price = vo.price;
			unit.weight = vo.weight;
			unit.isp = vo.isp;
			unit.frequency = vo.frequency;
			unit.area = vo.area;
			unit.site_industry = vo.site_industry;
			unit.show_site = vo.show_site;
			unit.device = vo.device;
			unit.browser = vo.browser;
			unit.customize = vo.customize;
		} else {
			mo_unit2[vo.unit_id] = vo;
		}
		//变更数据库
		DBHandle* handle = DBHandle::getInstance();
		handle->setVO(vo.unit_id, vo, UNIT_DB, unit_function());
		handle->syncTable(UNIT_DB);
		m_logger->debug("active unit success,unit_id=%u", vo.unit_id);

		//嵌套消息处理
		parseSubTree(pt, "ideas", "IDEA_ACTIVE");
	} else if (op == "UNIT_DEL") {
		UnitVO vo;
		vo.unit_id = pt.get<uint32>("unit_id");
		vo.status = pt.get<int8>("unit_stat");
		vo.target_stat = 0;

		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint32, UnitVO>& mo_unit = pMOm->m_unitMO.getItemsA();
		map<uint32, UnitVO>::iterator iter = mo_unit.find(vo.unit_id);
		if (iter != mo_unit.end()) {
			UnitVO& unit = iter->second;
			unit.status = vo.status;
			unit.target_stat = vo.target_stat;
		}
		//变更备份
		map<uint32, UnitVO>& mo_unit2 = pMOm->m_unitMO.getItemsB();
		map<uint32, UnitVO>::iterator iter2 = mo_unit2.find(vo.unit_id);
		if (iter2 != mo_unit2.end()) {
			UnitVO& unit = iter2->second;
			unit.status = vo.status;
			unit.target_stat = vo.target_stat;

			//保证非空
			vo.plan_id = unit.plan_id;
			vo.oid = unit.oid;
			vo.price = unit.price;
			vo.weight = unit.weight;
			vo.area = unit.area;
			vo.site_industry = unit.site_industry;
			vo.show_site = unit.show_site;

			//变更数据库
			DBHandle* handle = DBHandle::getInstance();
			handle->setVO(vo.unit_id, vo, UNIT_DB, unit_function());
			handle->syncTable(UNIT_DB);
		}
	}
	return 0;
}

int IdeaDealler::doMsg(string& op, const ptree& pt) {
	if (op == "IDEA_ACTIVE") {
		m_logger->debug("IDEA_ACTIVE domsg start");

		IdeaVO vo;
		vo.idea_id = pt.get<uint64>("idea_id");
		vo.unit_id = pt.get<uint32>("unit_id");
		vo.plan_id = pt.get<uint32>("plan_id");
		vo.oid = pt.get<uint32>("oid");
		vo.status = pt.get<int8>("idea_stat");
		vo.type = pt.get<uint16>("idea_type");
		vo.width = pt.get<uint16>("width");
		vo.height = pt.get<uint16>("height");
		vo.holycrap[0] = '\0';

		const ptree &subtree = pt.get_child("template_param");
		for (ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++) {
			vo.template_param[iter->first] = subtree.get<string>(iter->first);
		}

		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint64, IdeaVO>& mo_idea = pMOm->m_ideaMO.getItemsA();
		map<uint64, IdeaVO>::iterator iter = mo_idea.find(vo.idea_id);
		if (iter != mo_idea.end()) {
			IdeaVO& idea = iter->second;
			idea.status = vo.status;
			idea.type = vo.type;
			idea.width = vo.width;
			idea.height = vo.height;
			strcpy(vo.holycrap, idea.holycrap);
		}
		//变更备份
		map<uint64, IdeaVO>& mo_idea2 = pMOm->m_ideaMO.getItemsB();
		map<uint64, IdeaVO>::iterator iter2 = mo_idea2.find(vo.idea_id);
		if (iter2 != mo_idea2.end()) {
			IdeaVO& idea = iter2->second;
			idea.status = vo.status;
			idea.type = vo.type;
			idea.width = vo.width;
			idea.height = vo.height;
			strcpy(vo.holycrap, idea.holycrap);
			idea.template_param = vo.template_param;
		} else {
			mo_idea2[vo.idea_id] = vo;
		}
		//变更数据库
		DBHandle* handle = DBHandle::getInstance();
		handle->setVO(vo.idea_id, vo, IDEA_DB, idea_function());
		handle->syncTable(IDEA_DB);
		m_logger->debug("active idea success,idea_id=%u", vo.idea_id);
	} else if (op == "IDEA_DEL") {
		IdeaVO vo;
		vo.idea_id = pt.get<uint64>("idea_id");
		vo.status = pt.get<int8>("idea_stat");

		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint64, IdeaVO>& mo_idea = pMOm->m_ideaMO.getItemsA();
		map<uint64, IdeaVO>::iterator iter = mo_idea.find(vo.idea_id);
		if (iter != mo_idea.end()) {
			IdeaVO& idea = iter->second;
			idea.status = vo.status;
		}
		//变更备份
		map<uint64, IdeaVO>& mo_idea2 = pMOm->m_ideaMO.getItemsB();
		map<uint64, IdeaVO>::iterator iter2 = mo_idea2.find(vo.idea_id);
		if (iter2 != mo_idea2.end()) {
			IdeaVO& idea = iter2->second;
			idea.status = vo.status;

			//保证非空
			vo.unit_id = idea.unit_id;
			vo.plan_id = idea.plan_id;
			vo.oid = idea.oid;
			vo.type = idea.type;
			vo.width = idea.width;
			vo.height = idea.height;
			strcpy(vo.holycrap, idea.holycrap);
			vo.template_param = idea.template_param;

			//变更数据库
			DBHandle* handle = DBHandle::getInstance();
			handle->setVO(vo.idea_id, vo, IDEA_DB, idea_function());
			handle->syncTable(IDEA_DB);
		}
	}
	return 0;
}

int UnionDealler::doMsg(string& op, const ptree& pt) {
	if (op == "UNION_ACTIVE") {
		m_logger->debug("UNION_ACTIVE domsg start");

		UnionVO vo;
		vo.uid = pt.get<uint32>("uid");
		vo.status = pt.get<int8>("union_stat");

		//oid_filter

		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint32, UnionVO>& mo_union = pMOm->m_unionMO.getItemsA();
		map<uint32, UnionVO>::iterator iter = mo_union.find(vo.uid);
		if (iter != mo_union.end()) {
			UnionVO& un = iter->second;
			un.status = vo.status;
		}
		//变更备份
		map<uint32, UnionVO>& mo_union2 = pMOm->m_unionMO.getItemsB();
		map<uint32, UnionVO>::iterator iter2 = mo_union2.find(vo.uid);
		if (iter2 != mo_union2.end()) {
			UnionVO& un = iter2->second;
			un.status = vo.status;
		} else {
			mo_union2[vo.uid] = vo;
		}
		//变更数据库
		DBHandle* handle = DBHandle::getInstance();
		handle->setVO(vo.uid, vo, UNION_DB, union_function());
		handle->syncTable(UNION_DB);

		//----------- oid_filter ---------------------
		UnionFilterVO filter_vo;
		filter_vo.uid = vo.uid;

		string stOidFilter = pt.get<string>("oid_filter");
		if (stOidFilter != "0") {
			const ptree &subtree = pt.get_child("oid_filter");
			for (ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++) {
				uint32 oid = iter->second.get < uint32 > ("");
				if (oid > 0)
					filter_vo.oid_filter.insert(oid);
			}
		}
		//不变更当前,只变更备份
		map<uint32, UnionFilterVO>& mo_union_filter2 = pMOm->m_union_filterMO.getItemsB();
		map<uint32, UnionFilterVO>::iterator iter_filter2 = mo_union_filter2.find(filter_vo.uid);
		if (iter_filter2 != mo_union_filter2.end()) {
			UnionFilterVO& un = iter_filter2->second;
			un.oid_filter = filter_vo.oid_filter;
		} else {
			mo_union_filter2[filter_vo.uid] = filter_vo;
		}
		//变更数据库
		handle->setVO(filter_vo.uid, filter_vo, UNION_FILTER_DB, union_filter_function());
		handle->syncTable(UNION_FILTER_DB);
		m_logger->debug("active unit success,uid=%u", vo.uid);

		//嵌套消息处理
		parseSubTree(pt, "sites", "SITE_ACTIVE");
		parseSubTree(pt, "adbars", "ADBAR_ACTIVE");
	} else if (op == "UNION_DEL") {
		UnionVO vo;
		vo.uid = pt.get<uint32>("uid");
		vo.status = pt.get<int8>("union_stat");

		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint32, UnionVO>& mo_union = pMOm->m_unionMO.getItemsA();
		map<uint32, UnionVO>::iterator iter = mo_union.find(vo.uid);
		if (iter != mo_union.end()) {
			UnionVO& un = iter->second;
			un.status = vo.status;
		}
		//变更备份
		map<uint32, UnionVO>& mo_union2 = pMOm->m_unionMO.getItemsB();
		map<uint32, UnionVO>::iterator iter2 = mo_union2.find(vo.uid);
		if (iter2 != mo_union2.end()) {
			UnionVO& un = iter2->second;
			un.status = vo.status;

			//变更数据库
			DBHandle* handle = DBHandle::getInstance();
			handle->setVO(vo.uid, vo, UNION_DB, union_function());
			handle->syncTable(UNION_DB);
		}
	}
	return 0;
}

int SiteDealler::doMsg(string& op, const ptree& pt) {
	if (op == "SITE_ACTIVE") {
		m_logger->debug("SITE_ACTIVE domsg start");

		SiteVO vo;
		vo.site_id = pt.get<uint32>("site_id");
		vo.uid = pt.get<uint32>("uid");
		vo.type = pt.get<uint16>("type");
		vo.status = pt.get<int8>("site_stat");
		vo.site_industry = pt.get<uint32>("site_industry");
		vo.site_type = pt.get<uint32>("site_type");

		string domain = pt.get<string>("domain");
		strncpy(vo.domain, domain.c_str(), DOMAIN_MAXLEN);
		vo.domain[DOMAIN_MAXLEN] = '\0';

		vo.special_industry = 0;
		string strSpecialIndustry = pt.get<string>("special_industry");
		if (strSpecialIndustry != "0") {
			const ptree &subtree = pt.get_child("special_industry");
			for (ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++) {
				uint32 sp_id = iter->second.get < uint32 > ("");
				if (sp_id > 0) {
					vo.special_industry |= (1 << (sp_id - 1));
				}
			}
		}

		string stTargetIndustry = pt.get<string>("target_industry");
		if (stTargetIndustry != "0") {
			const ptree &subtree = pt.get_child("target_industry");
			for (ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++) {
				uint32 code = iter->second.get < uint32 > ("");
				if (code > 0)
					vo.target_industry.insert(code);
			}
		}

		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint32, SiteVO>& mo_site = pMOm->m_siteMO.getItemsA();
		map<uint32, SiteVO>::iterator iter = mo_site.find(vo.site_id);
		if (iter != mo_site.end()) {
			SiteVO& site = iter->second;
			site.type = vo.type;
			site.status = vo.status;
			site.site_industry = vo.site_industry;
			site.site_type = vo.site_type;
			site.special_industry = vo.special_industry;
		}
		//变更备份
		map<uint32, SiteVO>& mo_site2 = pMOm->m_siteMO.getItemsB();
		map<uint32, SiteVO>::iterator iter2 = mo_site2.find(vo.site_id);
		if (iter2 != mo_site2.end()) {
			SiteVO& site = iter2->second;
			site.type = vo.type;
			site.status = vo.status;
			site.site_industry = vo.site_industry;
			site.site_type = vo.site_type;
			strcpy(site.domain, vo.domain);
			site.special_industry = vo.special_industry;
			site.target_industry = vo.target_industry;
		} else {
			mo_site2[vo.site_id] = vo;
		}
		//变更数据库
		DBHandle* handle = DBHandle::getInstance();
		handle->setVO(vo.site_id, vo, SITE_DB, site_function());
		handle->syncTable(SITE_DB);
		m_logger->debug("active site success,site_id=%u", vo.site_id);
	} else if (op == "SITE_DEL") {
		SiteVO vo;
		vo.site_id = pt.get<uint32>("site_id");
		vo.status = pt.get<int8>("site_stat");

		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint32, SiteVO>& mo_site = pMOm->m_siteMO.getItemsA();
		map<uint32, SiteVO>::iterator iter = mo_site.find(vo.site_id);
		if (iter != mo_site.end()) {
			SiteVO& site = iter->second;
			site.status = vo.status;
		}
		//变更备份
		map<uint32, SiteVO>& mo_site2 = pMOm->m_siteMO.getItemsB();
		map<uint32, SiteVO>::iterator iter2 = mo_site2.find(vo.site_id);
		if (iter2 != mo_site2.end()) {
			SiteVO& site = iter2->second;
			site.status = vo.status;

			//保证非空
			vo.uid = site.uid;
			vo.site_industry = site.site_industry;
			vo.site_type = site.site_type;
			strcpy(vo.domain, site.domain);
			vo.special_industry = site.special_industry;
			vo.target_industry = site.target_industry;

			//变更数据库
			DBHandle* handle = DBHandle::getInstance();
			handle->setVO(vo.site_id, vo, SITE_DB, site_function());
			handle->syncTable(SITE_DB);
		}
	}
	return 0;
}

int AdbarDealler::doMsg(string& op, const ptree& pt) {
	if (op == "ADBAR_ACTIVE") {
		m_logger->debug("ADBAR_ACTIVE domsg start");

		AdbarVO vo;
		vo.adbar_id = pt.get<uint64>("adbar_id");
		vo.uid = pt.get<uint32>("uid");
		vo.type = pt.get<uint16>("adbar_type");
		vo.product = pt.get<uint16>("product");
		vo.status = pt.get<int8>("adbar_stat");

		vo.ad_form = pt.get<uint32>("ad_form");
		if (vo.ad_form == 0)
			m_logger->warning("ad_form=0 is invaild!!! adbarId:%lu", vo.adbar_id);

		vo.adstyle_id = pt.get<uint16>("adstyle_id");
		vo.width = pt.get<uint16>("width");
		vo.height = pt.get<uint16>("height");
		vo.ad_kind = pt.get<uint16>("ad_kind");
		vo.default_ad = pt.get<uint32>("default_ad");

		string border_color = pt.get<string>("border_color");
		strncpy(vo.border_color, border_color.c_str(), sizeof(vo.border_color) - 1);
		vo.border_color[sizeof(vo.border_color) - 1] = '\0';

		string back_color = pt.get<string>("back_color");
		strncpy(vo.back_color, back_color.c_str(), sizeof(vo.back_color) - 1);
		vo.back_color[sizeof(vo.back_color) - 1] = '\0';

		string title_color = pt.get<string>("title_color");
		strncpy(vo.title_color, title_color.c_str(), sizeof(vo.title_color) - 1);
		vo.title_color[sizeof(vo.title_color) - 1] = '\0';

		string desc_color = pt.get<string>("desc_color");
		strncpy(vo.desc_color, desc_color.c_str(), sizeof(vo.desc_color) - 1);
		vo.desc_color[sizeof(vo.desc_color) - 1] = '\0';

		string url_color = pt.get<string>("url_color");
		strncpy(vo.url_color, url_color.c_str(), sizeof(vo.url_color) - 1);
		vo.url_color[sizeof(vo.url_color) - 1] = '\0';

		string key_color = pt.get<string>("key_color");
		strncpy(vo.key_color, key_color.c_str(), sizeof(vo.key_color) - 1);
		vo.key_color[sizeof(vo.key_color) - 1] = '\0';

		string full_color = pt.get<string>("full_color");
		strncpy(vo.full_color, full_color.c_str(), sizeof(vo.full_color) - 1);
		vo.full_color[sizeof(vo.full_color) - 1] = '\0';

		string other_ad = pt.get<string>("other_ad");
		strncpy(vo.other_ad, other_ad.c_str(), sizeof(vo.other_ad) - 1);
		vo.other_ad[sizeof(vo.other_ad) - 1] = '\0';

		vo.min_price = pt.get<uint64>("min_price");

		string strMaxUnit = pt.get<string>("max_unit");
		if (strMaxUnit != "0") {
			const ptree &subtree = pt.get_child("max_unit");
			for (ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++) {
				vo.max_unit[(uint32) atoi(iter->first.c_str())] = iter->second.get < uint32 > ("");
			}
		}

		string strAdTemplates = pt.get<string>("adtemplates");
		if (strAdTemplates != "0") {
			const ptree &subtree = pt.get_child("adtemplates");
			for (ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++) {
				list<uint32> &v = vo.ad_templates[(uint32) atoi(iter->first.c_str())];
				const ptree &subtree2 = iter->second;
				for (ptree::const_iterator iter2 = subtree2.begin(); iter2 != subtree2.end(); iter2++) {
					v.push_back(iter2->second.get < uint32 > (""));
				}
			}
		}

		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint64, AdbarVO>& mo_adbar = pMOm->m_adbarMO.getItemsA();
		map<uint64, AdbarVO>::iterator iter = mo_adbar.find(vo.adbar_id);
		if (iter != mo_adbar.end()) {
			AdbarVO& adbar = iter->second;
			adbar.type = vo.type;
			adbar.product = vo.product;
			adbar.status = vo.status;
			adbar.ad_form = vo.ad_form;
			adbar.adstyle_id = vo.adstyle_id;
			adbar.width = vo.width;
			adbar.height = vo.height;
			adbar.ad_kind = vo.ad_kind;
			adbar.default_ad = vo.default_ad;
			adbar.min_price = vo.min_price;
		}
		//变更备份
		map<uint64, AdbarVO>& mo_adbar2 = pMOm->m_adbarMO.getItemsB();
		map<uint64, AdbarVO>::iterator iter2 = mo_adbar2.find(vo.adbar_id);
		if (iter2 != mo_adbar2.end()) {
			AdbarVO& adbar = iter2->second;
			adbar.type = vo.type;
			adbar.product = vo.product;
			adbar.status = vo.status;
			adbar.ad_form = vo.ad_form;
			adbar.adstyle_id = vo.adstyle_id;
			adbar.width = vo.width;
			adbar.height = vo.height;
			adbar.ad_kind = vo.ad_kind;
			adbar.max_unit = vo.max_unit;
			adbar.default_ad = vo.default_ad;
			adbar.min_price = vo.min_price;
			strcpy(adbar.border_color, vo.border_color);
			strcpy(adbar.back_color, vo.back_color);
			strcpy(adbar.title_color, vo.title_color);
			strcpy(adbar.desc_color, vo.desc_color);
			strcpy(adbar.url_color, vo.url_color);
			strcpy(adbar.key_color, vo.key_color);
			strcpy(adbar.full_color, vo.full_color);
			strcpy(adbar.other_ad, vo.other_ad);
			adbar.ad_templates = vo.ad_templates;
		} else {
			mo_adbar2[vo.adbar_id] = vo;
		}
		//变更数据库
		DBHandle* handle = DBHandle::getInstance();
		handle->setVO(vo.adbar_id, vo, ADBAR_DB, adbar_function());
		handle->syncTable(ADBAR_DB);
		m_logger->debug("active idea success,adbar_id=%u", vo.adbar_id);
	} else if (op == "IDEA_DEL") {
		AdbarVO vo;
		vo.adbar_id = pt.get<uint64>("adbar_id");
		vo.status = pt.get<int8>("adbar_stat");

		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint64, AdbarVO>& mo_adbar = pMOm->m_adbarMO.getItemsA();
		map<uint64, AdbarVO>::iterator iter = mo_adbar.find(vo.adbar_id);
		if (iter != mo_adbar.end()) {
			AdbarVO& adbar = iter->second;
			adbar.status = vo.status;
		}
		//变更备份
		map<uint64, AdbarVO>& mo_adbar2 = pMOm->m_adbarMO.getItemsB();
		map<uint64, AdbarVO>::iterator iter2 = mo_adbar2.find(vo.adbar_id);
		if (iter2 != mo_adbar2.end()) {
			AdbarVO& adbar = iter2->second;
			adbar.status = vo.status;

			//保证非空
			vo.uid = adbar.uid;
			vo.ad_form = adbar.ad_form;
			vo.adstyle_id = adbar.adstyle_id;
			vo.width = adbar.width;
			vo.height = adbar.height;
			vo.ad_kind = adbar.ad_kind;
			vo.max_unit = adbar.max_unit;
			vo.default_ad = adbar.default_ad;
			vo.min_price = adbar.min_price;
			strcpy(vo.border_color, adbar.border_color);
			strcpy(vo.back_color, adbar.back_color);
			strcpy(vo.title_color, adbar.title_color);
			strcpy(vo.desc_color, adbar.desc_color);
			strcpy(vo.url_color, adbar.url_color);
			strcpy(vo.key_color, adbar.key_color);
			strcpy(vo.full_color, adbar.full_color);
			strcpy(vo.other_ad, adbar.other_ad);
			vo.ad_templates = adbar.ad_templates;

			//变更数据库
			DBHandle* handle = DBHandle::getInstance();
			handle->setVO(vo.adbar_id, vo, ADBAR_DB, adbar_function());
			handle->syncTable(ADBAR_DB);
		}
	}
	return 0;
}

int TargetStopDealler::doMsg(string& op, const ptree& pt) {
	int8 status = pt.get<int8>("stat");
	if (status == STATUS_OK) { //广告主余额到
		OwnerVO vo;
		vo.oid = pt.get<uint32>("oid");

		bool isChg = false;
		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint32, OwnerVO>& mo_owner = pMOm->m_ownerMO.getItemsA();
		map<uint32, OwnerVO>::iterator iter = mo_owner.find(vo.oid);
		if (iter != mo_owner.end()) {
			OwnerVO& owner = iter->second;
			owner.target_stat = 0;
			//保证非空
			vo.status = owner.status;
			vo.target_stat = 0;
			strcpy(vo.domain, owner.domain);
			vo.vip = owner.vip;
			vo.balance = owner.balance;
			vo.today_cost = owner.today_cost;
			vo.special_industry = owner.special_industry;
			isChg = true;
		}
		//变更备份
		map<uint32, OwnerVO>& mo_owner2 = pMOm->m_ownerMO.getItemsB();
		map<uint32, OwnerVO>::iterator iter2 = mo_owner2.find(vo.oid);
		if (iter2 != mo_owner2.end()) {
			OwnerVO& owner = iter2->second;
			owner.target_stat = 0;
			//保证非空
			vo.status = owner.status;
			vo.target_stat = 0;
			strcpy(vo.domain, owner.domain);
			vo.vip = owner.vip;
			vo.balance = owner.balance;
			vo.today_cost = owner.today_cost;
			vo.special_industry = owner.special_industry;
			isChg = true;
		}
		if (isChg) {
			//变更数据库
			DBHandle* handle = DBHandle::getInstance();
			handle->setVO(vo.oid, vo, OWNER_DB, owner_function());
			handle->syncTable(OWNER_DB);
		}
	} else if (status == 3) { //计划预算到
		PlanVO vo;
		vo.plan_id = pt.get<uint32>("plan_id");

		bool isChg = false;
		MOManager* pMOm = MOManager::getInstance();
		//变更当前
		map<uint32, PlanVO>& mo_plan = pMOm->m_planMO.getItemsA();
		map<uint32, PlanVO>::iterator iter = mo_plan.find(vo.plan_id);
		if (iter != mo_plan.end()) {
			PlanVO& plan = iter->second;
			plan.target_stat = 0;
			//保证非空
			vo.oid = plan.oid;
			vo.status = plan.status;
			vo.target_stat = 0;
			vo.price_mode = plan.price_mode;
			vo.budget = plan.budget;
			vo.schedule = plan.schedule;
			vo.allow_list = plan.allow_list;
			isChg = true;
		}
		//变更备份
		map<uint32, PlanVO>& mo_plan2 = pMOm->m_planMO.getItemsB();
		map<uint32, PlanVO>::iterator iter2 = mo_plan2.find(vo.plan_id);
		if (iter2 != mo_plan2.end()) {
			PlanVO& plan = iter2->second;
			plan.target_stat = 0;
			//保证非空
			vo.oid = plan.oid;
			vo.status = plan.status;
			vo.target_stat = 0;
			vo.price_mode = plan.price_mode;
			vo.budget = plan.budget;
			vo.schedule = plan.schedule;
			vo.allow_list = plan.allow_list;
			isChg = true;
		}
		if (isChg) {
			//变更数据库
			DBHandle* handle = DBHandle::getInstance();
			handle->setVO(vo.plan_id, vo, PLAN_DB, plan_function());
			handle->syncTable(PLAN_DB);
		}
	}
	return 0;
}

//查询消息
int QueryDealler::doMsg(string& op, const ptree& pt) {
	string cmd;
	try {
		cmd = pt.get<string>("obj");
	} catch (exception & e) {
		throw StrException(-100, "query msg no obj node! %s[%u]", __FUNCTION__, __LINE__);
	}

	if (cmd == "owner") {
		return searchOwner(op, pt);
	} else if (cmd == "plan") {
		return searchPlan(op, pt);
	} else if (cmd == "unit") {
		return searchUnit(op, pt);
	} else if (cmd == "union") {
		return searchUnion(op, pt);
	} else if (cmd == "site") {
		return searchSite(op, pt);
	} else if (cmd == "adbar") {
		return searchAdbar(op, pt);
	} else if (cmd == "idea") {
		return searchIdea(op, pt);
	} else {
		m_logger->warning("没有该查询项,如有疑问,请与管理员联系!,cmd=%s, %s[%u]", cmd.c_str(), __FUNCTION__, __LINE__);
	}

	return 0;
}

// owner基础表查询
int QueryDealler::searchOwner(string& op, const ptree& pt) {
	uint32 oid;
	try {
		oid = pt.get<uint32>("oid");
	} catch (exception & e) {
		throw StrException(-100, "parse json error![%s] %s[%u]", e.what(), __FUNCTION__, __LINE__);
	}

	MOManager* pMOm = MOManager::getInstance();
	ostringstream oss; //创建一个流
	bool isOutput = false;
	//输出当前
	map<uint32, OwnerVO>& mo_owner = pMOm->m_ownerMO.getItemsA();
	map<uint32, OwnerVO>::iterator iter = mo_owner.find(oid);
	if (iter != mo_owner.end()) {
		OwnerVO& owner = iter->second;
		oss << "[CUR] oid=" << owner.oid << ", status=" << (uint32) owner.status << ", target_stat=" << (uint32) owner.target_stat << ", domain="
				<< string(owner.domain) << ", vip=" << (uint32) owner.vip << ", special_industry=" << owner.special_industry << ", target_industry="
				<< owner.target_industry << ", balance=" << owner.balance << ", today_cost=" << owner.today_cost << endl;
		isOutput = true;
	}
	//输出备份
	map<uint32, OwnerVO>& mo_owner2 = pMOm->m_ownerMO.getItemsB();
	map<uint32, OwnerVO>::iterator iter2 = mo_owner2.find(oid);
	if (iter2 != mo_owner2.end()) {
		OwnerVO& owner = iter2->second;
		oss << "[BAK] oid=" << owner.oid << ", status=" << (uint32) owner.status << ", target_stat=" << (uint32) owner.target_stat << ", domain="
				<< string(owner.domain) << ", vip=" << (uint32) owner.vip << ", special_industry=" << owner.special_industry << ", target_industry="
				<< owner.target_industry << ", balance=" << owner.balance << ", today_cost=" << owner.today_cost << endl;
		isOutput = true;
	}

	if (!isOutput) {
		oss << "Did not query the data you want to you want to, oid=" << oid;
	}
	respMsg = oss.str();
	return 0;
}

// plan基础表查询
int QueryDealler::searchPlan(string& op, const ptree& pt) {
	uint32 plan_id;
	try {
		plan_id = pt.get<uint32>("plan_id");
	} catch (exception & e) {
		throw StrException(-100, "parse json error![%s] %s[%u]", e.what(), __FUNCTION__, __LINE__);
	}

	MOManager* pMOm = MOManager::getInstance();
	ostringstream oss; //创建一个流
	bool isOutput = false;
	//输出当前
	map<uint32, PlanVO>& mo_plan = pMOm->m_planMO.getItemsA();
	map<uint32, PlanVO>::iterator iter = mo_plan.find(plan_id);
	if (iter != mo_plan.end()) {
		PlanVO& plan = iter->second;
		oss << "[CUR] plan_id=" << plan.plan_id << ", oid=" << plan.oid << ", status=" << (uint32) plan.status << ", target_stat="
				<< (uint32) plan.target_stat << ", price_mode=" << plan.price_mode << ", budget=" << plan.budget
				//<<", weight="<< plan.weight
				<< ", schedule=" << DBHandle::outputList(plan.schedule) << ", allow_list=" << DBHandle::outputList(plan.allow_list) << endl;
		isOutput = true;
	}
	//输出备份
	map<uint32, PlanVO>& mo_plan2 = pMOm->m_planMO.getItemsB();
	map<uint32, PlanVO>::iterator iter2 = mo_plan2.find(plan_id);
	if (iter2 != mo_plan2.end()) {
		PlanVO& plan = iter2->second;
		oss << "[BAK] plan_id=" << plan.plan_id << ", oid=" << plan.oid << ", status=" << (uint32) plan.status << ", target_stat="
				<< (uint32) plan.target_stat << ", price_mode=" << plan.price_mode << ", budget=" << plan.budget
				//<<", weight="<< plan.weight
				<< ", schedule=" << DBHandle::outputList(plan.schedule) << ", allow_list=" << DBHandle::outputList(plan.allow_list) << endl;
		isOutput = true;
	}

	if (!isOutput) {
		oss << "Did not query the data you want to you want to, plan_id=" << plan_id;
	}
	respMsg = oss.str();
	return 0;
}
// Unit基础表查询
int QueryDealler::searchUnit(string& op, const ptree& pt) {
	uint32 unit_id;
	try {
		unit_id = pt.get<uint32>("unit_id");
	} catch (exception & e) {
		throw StrException(-100, "parse json error![%s] %s[%u]", e.what(), __FUNCTION__, __LINE__);
	}

	MOManager* pMOm = MOManager::getInstance();
	ostringstream oss; //创建一个流
	bool isOutput = false;
	//输出当前
	map<uint32, UnitVO>& mo_unit = pMOm->m_unitMO.getItemsA();
	map<uint32, UnitVO>::iterator iter = mo_unit.find(unit_id);
	if (iter != mo_unit.end()) {
		UnitVO& unit = iter->second;
		oss << "[CUR] unit_id=" << unit.unit_id << ", plan_id=" << unit.plan_id << ", oid=" << unit.oid << ", status=" << (uint32) unit.status
				<< ", target_stat=" << (uint32) unit.target_stat << ", price=" << unit.price << ", weight=" << unit.weight << ", area="
				<< DBHandle::outputList(unit.area) << ", site_industry=" << DBHandle::outputList(unit.site_industry) << ", show_site="
				<< DBHandle::outputList(unit.show_site) << endl;
		isOutput = true;
	}
	//输出备份
	map<uint32, UnitVO>& mo_unit2 = pMOm->m_unitMO.getItemsB();
	map<uint32, UnitVO>::iterator iter2 = mo_unit2.find(unit_id);
	if (iter2 != mo_unit2.end()) {
		UnitVO& unit = iter2->second;
		oss << "[BAK] unit_id=" << unit.unit_id << ", plan_id=" << unit.plan_id << ", oid=" << unit.oid << ", status=" << (uint32) unit.status
				<< ", target_stat=" << (uint32) unit.target_stat << ", price=" << unit.price << ", weight=" << unit.weight << ", area="
				<< DBHandle::outputList(unit.area) << ", site_industry=" << DBHandle::outputList(unit.site_industry) << ", show_site="
				<< DBHandle::outputList(unit.show_site) << endl;
		isOutput = true;
	}

	if (!isOutput) {
		oss << "Did not query the data you want to you want to, unit_id=" << unit_id;
	}
	respMsg = oss.str();
	return 0;
}

// idea基础表查询
int QueryDealler::searchIdea(string& op, const ptree& pt) {
	uint64 idea_id;
	try {
		idea_id = pt.get<uint64>("idea_id");
	} catch (exception & e) {
		throw StrException(-100, "parse json error![%s] %s[%u]", e.what(), __FUNCTION__, __LINE__);
	}

	MOManager* pMOm = MOManager::getInstance();
	ostringstream oss; //创建一个流
	bool isOutput = false;
	//输出当前
	map<uint64, IdeaVO>& mo_idea = pMOm->m_ideaMO.getItemsA();
	map<uint64, IdeaVO>::iterator iter = mo_idea.find(idea_id);
	if (iter != mo_idea.end()) {
		IdeaVO& idea = iter->second;
		oss << "[CUR] idea_id=" << idea.idea_id << ", unit_id=" << idea.unit_id << ", plan_id=" << idea.plan_id << ", oid=" << idea.oid << ", status="
				<< (uint32) idea.status << ", type=" << idea.type << ", width=" << idea.width << ", height=" << idea.height << ", template_param="
				<< DBHandle::outputMap<string, string>(idea.template_param) << endl;
		isOutput = true;
	}
	//输出备份
	map<uint64, IdeaVO>& mo_idea2 = pMOm->m_ideaMO.getItemsB();
	map<uint64, IdeaVO>::iterator iter2 = mo_idea2.find(idea_id);
	if (iter2 != mo_idea2.end()) {
		IdeaVO& idea = iter2->second;
		oss << "[BAK] idea_id=" << idea.idea_id << ", unit_id=" << idea.unit_id << ", plan_id=" << idea.plan_id << ", oid=" << idea.oid << ", status="
				<< (uint32) idea.status << ", type=" << idea.type << ", width=" << idea.width << ", height=" << idea.height << ", template_param="
				<< DBHandle::outputMap<string, string>(idea.template_param) << endl;
		isOutput = true;
	}

	if (!isOutput) {
		oss << "Did not query the data you want to you want to, idea_id=" << idea_id;
	}
	respMsg = oss.str();
	return 0;
}

// Union基础表查询
int QueryDealler::searchUnion(string& op, const ptree& pt) {
	uint32 uid;
	try {
		uid = pt.get<uint32>("uid");
	} catch (exception & e) {
		throw StrException(-100, "parse json error![%s] %s[%u]", e.what(), __FUNCTION__, __LINE__);
	}

	MOManager* pMOm = MOManager::getInstance();
	ostringstream oss; //创建一个流
	bool isOutput = false;
	//输出当前
	map<uint32, UnionVO>& mo_union = pMOm->m_unionMO.getItemsA();
	map<uint32, UnionVO>::iterator iter = mo_union.find(uid);
	if (iter != mo_union.end()) {
		UnionVO& un = iter->second;
		oss << "[CUR] uid=" << un.uid << ", status=" << (uint32) un.status << ", site=" << DBHandle::outputMap(un.site) << endl;
		isOutput = true;
	}
	//输出备份
	map<uint32, UnionVO>& mo_union2 = pMOm->m_unionMO.getItemsB();
	map<uint32, UnionVO>::iterator iter2 = mo_union2.find(uid);
	if (iter2 != mo_union2.end()) {
		UnionVO& un = iter2->second;
		oss << "[BAK] uid=" << un.uid << ", status=" << (uint32) un.status << ", site=" << DBHandle::outputMap(un.site) << endl;
		isOutput = true;
	}

	if (!isOutput) {
		oss << "Did not query the data you want to you want to, uid=" << uid;
	}
	respMsg = oss.str();
	return 0;
}
// Site基础表查询
int QueryDealler::searchSite(string& op, const ptree& pt) {
	uint32 site_id;
	try {
		site_id = pt.get<uint32>("site_id");
	} catch (exception & e) {
		throw StrException(-100, "parse json error![%s] %s[%u]", e.what(), __FUNCTION__, __LINE__);
	}

	MOManager* pMOm = MOManager::getInstance();
	ostringstream oss; //创建一个流
	bool isOutput = false;
	//输出当前
	map<uint32, SiteVO>& mo_site = pMOm->m_siteMO.getItemsA();
	map<uint32, SiteVO>::iterator iter = mo_site.find(site_id);
	if (iter != mo_site.end()) {
		SiteVO& site = iter->second;
		oss << "[CUR] site_id=" << site.site_id << ", uid=" << site.uid << ", status=" << (uint32) site.status << ", site_industry="
				<< site.site_industry << ", site_type=" << site.site_type << ", domain=" << string(site.domain) << ", special_industry="
				<< site.special_industry << ", target_industry=" << DBHandle::outputSet(site.target_industry) << endl;
		isOutput = true;
	}
	//输出备份
	map<uint32, SiteVO>& mo_site2 = pMOm->m_siteMO.getItemsB();
	map<uint32, SiteVO>::iterator iter2 = mo_site2.find(site_id);
	if (iter2 != mo_site2.end()) {
		SiteVO& site = iter2->second;
		oss << "[BAK] site_id=" << site.site_id << ", uid=" << site.uid << ", status=" << (uint32) site.status << ", site_industry="
				<< site.site_industry << ", site_type=" << site.site_type << ", domain=" << string(site.domain) << ", special_industry="
				<< site.special_industry << ", target_industry=" << DBHandle::outputSet(site.target_industry) << endl;
		isOutput = true;
	}

	if (!isOutput) {
		oss << "Did not query the data you want to you want to, site_id=" << site_id;
	}
	respMsg = oss.str();
	return 0;
}

// Adbar基础表查询
int QueryDealler::searchAdbar(string& op, const ptree& pt) {
	uint64 adbar_id;
	try {
		adbar_id = pt.get<uint64>("adbar_id");
	} catch (exception & e) {
		throw StrException(-100, "parse json error![%s] %s[%u]", e.what(), __FUNCTION__, __LINE__);
	}

	MOManager* pMOm = MOManager::getInstance();
	ostringstream oss; //创建一个流
	bool isOutput = false;
	//输出当前
	map<uint64, AdbarVO>& mo_adbar = pMOm->m_adbarMO.getItemsA();
	map<uint64, AdbarVO>::iterator iter = mo_adbar.find(adbar_id);
	if (iter != mo_adbar.end()) {
		AdbarVO& adbar = iter->second;
		oss << "[CUR] adbar_id=" << adbar.adbar_id << ", uid=" << adbar.uid << ", product=" << (uint32) adbar.product << ", status="
				<< (uint32) adbar.status << ", ad_form=" << adbar.ad_form << ", adstyle_id=" << adbar.adstyle_id << ", width=" << adbar.width
				<< ", height=" << adbar.height << ", ad_kind=" << adbar.ad_kind << ", max_unit="
				<< DBHandle::outputMap<uint32, uint32>(adbar.max_unit) << ", default_ad=" << adbar.default_ad << ", border_color="
				<< string(adbar.border_color) << ", back_color=" << string(adbar.back_color) << ", title_color=" << string(adbar.title_color)
				<< ", desc_color=" << string(adbar.desc_color) << ", url_color=" << string(adbar.url_color) << ", key_color="
				<< string(adbar.key_color) << ", full_color=" << string(adbar.full_color) << ", other_ad=" << string(adbar.other_ad) << ", min_price="
				<< adbar.min_price << ", ad_templates=" << DBHandle::outputMapList(adbar.ad_templates) << endl;
		isOutput = true;
	}
	//输出备份
	map<uint64, AdbarVO>& mo_adbar2 = pMOm->m_adbarMO.getItemsB();
	map<uint64, AdbarVO>::iterator iter2 = mo_adbar2.find(adbar_id);
	if (iter2 != mo_adbar2.end()) {
		AdbarVO& adbar = iter2->second;
		oss << "[BAK] adbar_id=" << adbar.adbar_id << ", uid=" << adbar.uid << ", product=" << (uint32) adbar.product << ", status="
				<< (uint32) adbar.status << ", ad_form=" << adbar.ad_form << ", adstyle_id=" << adbar.adstyle_id << ", width=" << adbar.width
				<< ", height=" << adbar.height << ", ad_kind=" << adbar.ad_kind << ", max_unit="
				<< DBHandle::outputMap<uint32, uint32>(adbar.max_unit) << ", default_ad=" << adbar.default_ad << ", border_color="
				<< string(adbar.border_color) << ", back_color=" << string(adbar.back_color) << ", title_color=" << string(adbar.title_color)
				<< ", desc_color=" << string(adbar.desc_color) << ", url_color=" << string(adbar.url_color) << ", key_color="
				<< string(adbar.key_color) << ", full_color=" << string(adbar.full_color) << ", other_ad=" << string(adbar.other_ad) << ", min_price="
				<< adbar.min_price << ", ad_templates=" << DBHandle::outputMapList(adbar.ad_templates) << endl;
		isOutput = true;
	}

	if (!isOutput) {
		oss << "Did not query the data you want to you want to, adbar_id=" << adbar_id;
	}
	respMsg = oss.str();
	return 0;
}
