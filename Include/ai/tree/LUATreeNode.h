/**
 * @file
 * @ingroup LUA
 */
#pragma once

#include "tree/TreeNode.h"
#include "LUAFunctions.h"

namespace ai {

/**
 * @see @ai{LUAAIRegistry}
 */
class LUATreeNode : public TreeNode {
protected:
	lua_State* _s;

	TreeNodeStatus runLUA(const AIPtr& entity, int64_t deltaMillis) {
		// get userdata of the behaviour tree node
		const std::string name = "__meta_node_" + _type;
		lua_getfield(_s, LUA_REGISTRYINDEX, name.c_str());
#if AI_LUA_SANTITY > 0
		if (lua_isnil(_s, -1)) {
			ai_log_error("LUA node: could not find lua userdata for %s", name.c_str());
			return TreeNodeStatus::EXCEPTION;
		}
#endif
		// get metatable
		lua_getmetatable(_s, -1);
#if AI_LUA_SANTITY > 0
		if (!lua_istable(_s, -1)) {
			ai_log_error("LUA node: userdata for %s doesn't have a metatable assigned", name.c_str());
			return TreeNodeStatus::EXCEPTION;
		}
#endif
		// get execute() method
		lua_getfield(_s, -1, "execute");
		if (!lua_isfunction(_s, -1)) {
			ai_log_error("LUA node: metatable for %s doesn't have the execute() function assigned", name.c_str());
			return TreeNodeStatus::EXCEPTION;
		}

		// push self onto the stack
		lua_getfield(_s, LUA_REGISTRYINDEX, name.c_str());

		// first parameter is ai
		if (luaAI_pushai(_s, entity) == 0) {
			return TreeNodeStatus::EXCEPTION;
		}

		// second parameter is dt
		lua_pushinteger(_s, deltaMillis);

#if AI_LUA_SANTITY > 0
		if (!lua_isfunction(_s, -4)) {
			ai_log_error("LUA node: expected to find a function on stack -4");
			return TreeNodeStatus::EXCEPTION;
		}
		if (!lua_isuserdata(_s, -3)) {
			ai_log_error("LUA node: expected to find the userdata on -3");
			return TreeNodeStatus::EXCEPTION;
		}
		if (!lua_isuserdata(_s, -2)) {
			ai_log_error("LUA node: second parameter should be the ai");
			return TreeNodeStatus::EXCEPTION;
		}
		if (!lua_isinteger(_s, -1)) {
			ai_log_error("LUA node: first parameter should be the delta millis");
			return TreeNodeStatus::EXCEPTION;
		}
#endif
		const int error = lua_pcall(_s, 3, 1, 0);
		if (error) {
			ai_log_error("LUA node script: %s", lua_isstring(_s, -1) ? lua_tostring(_s, -1) : "Unknown Error");
			// reset stack
			lua_pop(_s, lua_gettop(_s));
			return TreeNodeStatus::EXCEPTION;
		}
		const lua_Integer execstate = luaL_checkinteger(_s, -1);
		if (execstate < 0 || execstate >= (lua_Integer)TreeNodeStatus::MAX_TREENODESTATUS) {
			ai_log_error("LUA node: illegal tree node status returned: " LUA_INTEGER_FMT, execstate);
		}

		// reset stack
		lua_pop(_s, lua_gettop(_s));
		return (TreeNodeStatus)execstate;
	}

public:
	class LUATreeNodeFactory : public ITreeNodeFactory {
	private:
		lua_State* _s;
		std::string _type;
	public:
		LUATreeNodeFactory(lua_State* s, const std::string& typeStr) :
				_s(s), _type(typeStr) {
		}

		inline const std::string& type() const {
			return _type;
		}

		TreeNodePtr create(const TreeNodeFactoryContext* ctx) const override {
			return std::make_shared<LUATreeNode>(ctx->name, ctx->parameters, ctx->condition, _s, _type);
		}
	};

	LUATreeNode(const std::string& name, const std::string& parameters, const ConditionPtr& condition, lua_State* s, const std::string& type) :
			TreeNode(name, parameters, condition), _s(s) {
		_type = type;
	}

	~LUATreeNode() {
	}

	TreeNodeStatus execute(const AIPtr& entity, int64_t deltaMillis) override {
		if (TreeNode::execute(entity, deltaMillis) == CANNOTEXECUTE) {
			return CANNOTEXECUTE;
		}

#if AI_EXCEPTIONS
		try {
#endif
			return state(entity, runLUA(entity, deltaMillis));
#if AI_EXCEPTIONS
		} catch (...) {
			ai_log_error("Exception while running lua tree node");
		}
		return state(entity, EXCEPTION);
#endif
	}
};

}
