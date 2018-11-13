/**
 * @file
 */
#pragma once

#include "tree/TreeNode.h"

namespace ai {

/**
 * @brief A decorator node with only one child attached. The result of the attached child is only
 * taken into account if it returned @c TreeNodeStatus::RUNNING - in every other case this decorator
 * will return @c TreeNodeStatus::FAILED.
 */
class Fail: public TreeNode {
public:
	NODE_CLASS(Fail)

	TreeNodeStatus execute(const AIPtr& entity, int64_t deltaMillis) override {
		if (_children.size() != 1) {
			ai_assert(false, "Fail must have exactly one child");
		}

		if (TreeNode::execute(entity, deltaMillis) == CANNOTEXECUTE) {
			return CANNOTEXECUTE;
		}

		const TreeNodePtr& treeNode = *_children.begin();
		const TreeNodeStatus status = treeNode->execute(entity, deltaMillis);
		if (status == RUNNING) {
			return state(entity, RUNNING);
		}
		return state(entity, FAILED);
	}
};

}
