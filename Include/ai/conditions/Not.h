/**
 * @file
 * @ingroup Condition
 */
#pragma once

#include "conditions/ICondition.h"

namespace ai {

/**
 * @brief This condition will just swap the result of the contained condition
 */
class Not: public ICondition {
protected:
	ConditionPtr _condition;

	void getConditionNameWithValue(std::stringstream& s, const AIPtr& entity) override {
		s << "(" << _condition->getNameWithConditions(entity) << ")";
	}

public:
	CONDITION_FACTORY_NO_IMPL(Not)

	explicit Not(const ConditionPtr& condition) :
			ICondition("Not", ""), _condition(condition) {
	}
	virtual ~Not() {
	}

	bool evaluate(const AIPtr& entity) override {
		return !_condition->evaluate(entity);
	}
};

inline ConditionPtr Not::Factory::create(const ConditionFactoryContext *ctx) const {
	if (ctx->conditions.size() != 1) {
		return ConditionPtr();
	}
	return std::make_shared<Not>(ctx->conditions.front());
}

}
