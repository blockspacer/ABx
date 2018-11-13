/**
 * @file
 */
#pragma once

#include "common/Types.h"
#include "common/MemoryAllocator.h"
#include "conditions/ICondition.h"
#include "conditions/True.h"
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

namespace ai {

class TreeNode;
typedef std::shared_ptr<TreeNode> TreeNodePtr;
typedef std::vector<TreeNodePtr> TreeNodes;

/**
 * @brief Execution states of a TreeNode::execute() call
 */
enum TreeNodeStatus {
	UNKNOWN,
	/**
	 * Not every condition is met in order to run this node
	 * In general this means that the attached condition has to evaluate to @c true
	 */
	CANNOTEXECUTE,
	/**
	 * This behavior is still running and thus can block others
	 */
	RUNNING,
	/**
	 * The behavior ran and terminated without any problems.
	 */
	FINISHED,
	/**
	 * Controlled failure
	 */
	FAILED,
	/**
	 * Unexpected failure while executing the the node's action
	 */
	EXCEPTION,

	MAX_TREENODESTATUS
};

/**
 * @brief A node factory macro to ease and unify the registration at AIRegistry.
 */
#define NODE_FACTORY(NodeName) \
	class Factory: public ::ai::ITreeNodeFactory { \
	public: \
		::ai::TreeNodePtr create (const ::ai::TreeNodeFactoryContext *ctx) const override { \
			return std::make_shared<NodeName>(ctx->name, ctx->parameters, ctx->condition); \
		} \
	}; \
	static const Factory& getFactory() { \
		static Factory FACTORY; \
		return FACTORY; \
	}

/**
 * @brief A node class macro that also defines a factory.
 */
#define NODE_CLASS(NodeName) \
	NodeName(const std::string& name, const std::string& parameters, const ::ai::ConditionPtr& condition) : \
		::ai::TreeNode(name, parameters, condition) { \
		_type = AI_STRINGIFY(NodeName); \
	} \
	virtual ~NodeName() { \
	} \
	\
	NODE_FACTORY(NodeName)

/**
 * @brief The base class for all behaviour tree actions.
 *
 * @c TreeNode::execute is triggered with each @c AI::update.
 * Also the attached @c ICondition is evaluated here. States are stored on the
 * connected @c AI instance. Don't store states on tree nodes, because they can
 * be reused for multiple @c AI instances. Always use the @c AI or @c ICharacter
 * to store your state!
 */
class TreeNode : public MemObject {
protected:
	static int getNextId() {
		static int _nextId;
		const int nextId = _nextId++;
		return nextId;
	}
	/**
	 * @brief Every node has an id to identify it. It's unique per type.
	 */
	int _id;
	TreeNodes _children;
	std::string _name;
	std::string _type;
	std::string _parameters;
	ConditionPtr _condition;

	TreeNodeStatus state(const AIPtr& entity, TreeNodeStatus treeNodeState);
	int getSelectorState(const AIPtr& entity) const;
	void setSelectorState(const AIPtr& entity, int selected);
	int getLimitState(const AIPtr& entity) const;
	void setLimitState(const AIPtr& entity, int amount);
	void setLastExecMillis(const AIPtr& entity);

	TreeNodePtr getParent_r(const TreeNodePtr& parent, int id) const;

public:
	/**
	 * @param name The internal name of the node
	 * @param parameters Each node can be configured with several parameters that are hand in as a string. It's
	 * the responsibility of the node to parse the values in its constructor
	 * @param condition The connected ICondition for this node
	 */
	TreeNode(const std::string& name, const std::string& parameters, const ConditionPtr& condition) :
			_id(getNextId()), _name(name), _parameters(parameters), _condition(condition) {
	}

	virtual ~TreeNode() {}
	/**
	 * @brief Return the unique id for this node.
	 * @return unique id
	 */
	int getId() const;

	/**
	 * @brief Each node can have a user defines name that can be retrieved with this method.
	 */
	const std::string& getName() const;

	/**
	 * @brief Return the raw parameters for this node
	 */
	const std::string& getParameters() const;

	/**
	 * @brief Updates the custom name of this @c TreeNode
	 *
	 * @param[in] name The name to set - empty strings are ignored here
	 */
	void setName(const std::string& name);
	/**
	 * @brief The node type - this usually matches the class name of the @c TreeNode
	 */
	const std::string& getType() const;
	const ConditionPtr& getCondition() const;
	void setCondition(const ConditionPtr& condition);
	const TreeNodes& getChildren() const;
	TreeNodes& getChildren();

	/**
	 * @brief Get the state of all child nodes for the given entity
	 * @param[in] entity The entity to get the child node states for
	 */
	virtual void getRunningChildren(const AIPtr& entity, std::vector<bool>& active) const;
	/**
	 * @brief Returns the time in milliseconds when this node was last run. This is only updated if @c #execute() was called
	 */
	int64_t getLastExecMillis(const AIPtr& ai) const;
	TreeNodeStatus getLastStatus(const AIPtr& ai) const;

	/**
	 * @param entity The entity to execute the TreeNode for
	 * @param deltaMillis The delta since the last execution
	 * @return TreeNodeStatus
	 */
	virtual TreeNodeStatus execute(const AIPtr& entity, int64_t deltaMillis);

	/**
	 * @brief Reset the states in the node and also in the entity
	 */
	virtual void resetState(const AIPtr& entity);

	virtual bool addChild(const TreeNodePtr& child);
	TreeNodePtr getChild(int id) const;
	/**
	 * @brief Replace the given child node with a new one (or removes it)
	 *
	 * @param[in] id The child node id
	 * @param[in] newNode If this is an empty TreeNodePtr the child will be removed
	 * @return @c true if the removal/replace was successful, @c false otherwise
	 */
	bool replaceChild(int id, const TreeNodePtr& newNode);
	/**
	 * @brief Get the parent node for a given TreeNode id - This should only be called on the root node of the behaviour
	 *
	 * @param[in] self The pointer to the root node that is returned if one of the direct children need their parent
	 * @param[in] id The child node id
	 *
	 * @return An empty TreeNodePtr if not found, or the parent is the root node of the behaviour tree
	 */
	TreeNodePtr getParent(const TreeNodePtr& self, int id) const;
};

}
