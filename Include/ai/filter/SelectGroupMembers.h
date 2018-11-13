/**
 * @file
 * @ingroup Filter
 */
#pragma once

#include "filter/IFilter.h"
#include "zone/Zone.h"

namespace ai {

/**
 * @brief This filter will pick the entities from the groups the given @c AI instance is in
 */
class SelectGroupMembers: public IFilter {
protected:
	GroupId _groupId;
public:
	FILTER_FACTORY(SelectGroupMembers)

	explicit SelectGroupMembers(const std::string& parameters = "") :
		IFilter("SelectGroupMembers", parameters) {
		if (_parameters.empty()) {
			_groupId = -1;
		} else {
			_groupId = std::stoi(_parameters);
		}
	}

	void filter (const AIPtr& entity) override {
		FilteredEntities& entities = getFilteredEntities(entity);
		auto func = [&] (const AIPtr& ai) {
			entities.push_back(ai->getId());
			return true;
		};
		Zone* zone = entity->getZone();
		GroupMgr& groupMgr = zone->getGroupMgr();
		groupMgr.visit(_groupId, func);
	}
};

}
