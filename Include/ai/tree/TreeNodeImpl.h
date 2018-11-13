/**
 * @file
 */
#pragma once

#include "TreeNode.h"
#include "AI.h"

namespace ai {

inline int TreeNode::getId() const {
	return _id;
}

inline void TreeNode::setName(const std::string& name) {
	if (name.empty()) {
		return;
	}
	_name = name;
}

inline const std::string& TreeNode::getType() const {
	return _type;
}

inline const std::string& TreeNode::getName() const {
	return _name;
}

inline const ConditionPtr& TreeNode::getCondition() const {
	return _condition;
}

inline void TreeNode::setCondition(const ConditionPtr& condition) {
	_condition = condition;
}

inline const std::string& TreeNode::getParameters() const {
	return _parameters;
}

inline const TreeNodes& TreeNode::getChildren() const {
	return _children;
}

inline TreeNodes& TreeNode::getChildren() {
	return _children;
}

inline bool TreeNode::addChild(const TreeNodePtr& child) {
	_children.push_back(child);
	return true;
}

inline void TreeNode::resetState(const AIPtr& entity) {
	for (auto& c : _children) {
		c->resetState(entity);
	}
}

inline void TreeNode::getRunningChildren(const AIPtr& /*entity*/, std::vector<bool>& active) const {
	const int size = (int)_children.size();
	for (int i = 0; i < size; ++i) {
		active.push_back(false);
	}
}

inline void TreeNode::setLastExecMillis(const AIPtr& entity) {
	if (!entity->_debuggingActive) {
		return;
	}
	entity->_lastExecMillis[getId()] = entity->_time;
}

inline int TreeNode::getSelectorState(const AIPtr& entity) const {
	AI::SelectorStates::const_iterator i = entity->_selectorStates.find(getId());
	if (i == entity->_selectorStates.end()) {
		return AI_NOTHING_SELECTED;
	}
	return i->second;
}

inline void TreeNode::setSelectorState(const AIPtr& entity, int selected) {
	entity->_selectorStates[getId()] = selected;
}

inline int TreeNode::getLimitState(const AIPtr& entity) const {
	AI::LimitStates::const_iterator i = entity->_limitStates.find(getId());
	if (i == entity->_limitStates.end()) {
		return 0;
	}
	return i->second;
}

inline void TreeNode::setLimitState(const AIPtr& entity, int amount) {
	entity->_limitStates[getId()] = amount;
}

inline TreeNodeStatus TreeNode::state(const AIPtr& entity, TreeNodeStatus treeNodeState) {
	if (!entity->_debuggingActive) {
		return treeNodeState;
	}
	entity->_lastStatus[getId()] = treeNodeState;
	return treeNodeState;
}

inline int64_t TreeNode::getLastExecMillis(const AIPtr& entity) const {
	if (!entity->_debuggingActive) {
		return -1L;
	}
	AI::LastExecMap::const_iterator i = entity->_lastExecMillis.find(getId());
	if (i == entity->_lastExecMillis.end()) {
		return -1L;
	}
	return i->second;
}

inline TreeNodeStatus TreeNode::getLastStatus(const AIPtr& entity) const {
	if (!entity->_debuggingActive) {
		return UNKNOWN;
	}
	AI::NodeStates::const_iterator i = entity->_lastStatus.find(getId());
	if (i == entity->_lastStatus.end()) {
		return UNKNOWN;
	}
	return i->second;
}

inline TreeNodePtr TreeNode::getChild(int id) const {
	for (auto& child : _children) {
		if (child->getId() == id) {
			return child;
		}
		const TreeNodePtr& node = child->getChild(id);
		if (node) {
			return node;
		}
	}
	return TreeNodePtr();
}

inline bool TreeNode::replaceChild(int id, const TreeNodePtr& newNode) {
	auto i = std::find_if(_children.begin(), _children.end(), [id] (const TreeNodePtr& other) { return other->getId() == id; });
	if (i == _children.end()) {
		return false;
	}

	if (newNode) {
		*i = newNode;
		return true;
	}

	_children.erase(i);
	return true;
}

inline TreeNodePtr TreeNode::getParent_r(const TreeNodePtr& parent, int id) const {
	for (auto& child : _children) {
		if (child->getId() == id) {
			return parent;
		}
		const TreeNodePtr& parentPtr = child->getParent_r(child, id);
		if (parentPtr) {
			return parentPtr;
		}
	}
	return TreeNodePtr();
}

inline TreeNodePtr TreeNode::getParent(const TreeNodePtr& self, int id) const {
	ai_assert(getId() != id, "Root nodes don't have a parent");
	for (auto& child : _children) {
		if (child->getId() == id) {
			return self;
		}
		const TreeNodePtr& parent = child->getParent_r(child, id);
		if (parent) {
			return parent;
		}
	}
	return TreeNodePtr();
}

inline TreeNodeStatus TreeNode::execute(const AIPtr& entity, int64_t /*deltaMillis*/) {
	if (!_condition->evaluate(entity)) {
		return state(entity, CANNOTEXECUTE);
	}

	setLastExecMillis(entity);
	return state(entity, FINISHED);
}

}
