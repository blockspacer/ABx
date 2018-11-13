/**
 * @file
 * @ingroup LUA
 */
#pragma once

#include "Steering.h"
#include "LUAFunctions.h"

namespace ai {
namespace movement {

/**
 * @see @ai{LUAAIRegistry}
 */
class LUASteering : public ISteering {
protected:
	mutable lua_State* _s;
	std::string _type;

	MoveVector executeLUA(const AIPtr& entity, float speed) const {
		// get userdata of the behaviour tree steering
		const std::string name = "__meta_steering_" + _type;
		lua_getfield(_s, LUA_REGISTRYINDEX, name.c_str());
#if AI_LUA_SANTITY > 0
		if (lua_isnil(_s, -1)) {
			ai_log_error("LUA steering: could not find lua userdata for %s", name.c_str());
			return MoveVector(VEC3_INFINITE, 0.0f);
		}
#endif
		// get metatable
		lua_getmetatable(_s, -1);
#if AI_LUA_SANTITY > 0
		if (!lua_istable(_s, -1)) {
			ai_log_error("LUA steering: userdata for %s doesn't have a metatable assigned", name.c_str());
			return MoveVector(VEC3_INFINITE, 0.0f);
		}
#endif
		// get execute() method
		lua_getfield(_s, -1, "execute");
		if (!lua_isfunction(_s, -1)) {
			ai_log_error("LUA steering: metatable for %s doesn't have the execute() function assigned", name.c_str());
			return MoveVector(VEC3_INFINITE, 0.0f);
		}

		// push self onto the stack
		lua_getfield(_s, LUA_REGISTRYINDEX, name.c_str());

		// first parameter is ai
		if (luaAI_pushai(_s, entity) == 0) {
			return MoveVector(VEC3_INFINITE, 0.0f);
		}

		// second parameter is speed
		lua_pushnumber(_s, speed);

#if AI_LUA_SANTITY > 0
		if (!lua_isfunction(_s, -4)) {
			ai_log_error("LUA steering: expected to find a function on stack -4");
			return MoveVector(VEC3_INFINITE, 0.0f);
		}
		if (!lua_isuserdata(_s, -3)) {
			ai_log_error("LUA steering: expected to find the userdata on -3");
			return MoveVector(VEC3_INFINITE, 0.0f);
		}
		if (!lua_isuserdata(_s, -2)) {
			ai_log_error("LUA steering: second parameter should be the ai");
			return MoveVector(VEC3_INFINITE, 0.0f);
		}
		if (!lua_isnumber(_s, -1)) {
			ai_log_error("LUA steering: first parameter should be the speed");
			return MoveVector(VEC3_INFINITE, 0.0f);
		}
#endif
		const int error = lua_pcall(_s, 3, 4, 0);
		if (error) {
			ai_log_error("LUA steering script: %s", lua_isstring(_s, -1) ? lua_tostring(_s, -1) : "Unknown Error");
			// reset stack
			lua_pop(_s, lua_gettop(_s));
			return MoveVector(VEC3_INFINITE, 0.0f);
		}
		// we get four values back, the direction vector and the
		const lua_Number x = luaL_checknumber(_s, -1);
		const lua_Number y = luaL_checknumber(_s, -2);
		const lua_Number z = luaL_checknumber(_s, -3);
		const lua_Number rotation = luaL_checknumber(_s, -4);

		// reset stack
		lua_pop(_s, lua_gettop(_s));
		return MoveVector(glm::vec3((float)x, (float)y, (float)z), (float)rotation);
	}

public:
	class LUASteeringFactory : public ISteeringFactory {
	private:
		lua_State* _s;
		std::string _type;
	public:
		LUASteeringFactory(lua_State* s, const std::string& typeStr) :
				_s(s), _type(typeStr) {
		}

		inline const std::string& type() const {
			return _type;
		}

		SteeringPtr create(const SteeringFactoryContext* ctx) const override {
			return std::make_shared<LUASteering>(_s, _type);
		}
	};

	LUASteering(lua_State* s, const std::string& type) :
			ISteering(), _s(s) {
		_type = type;
	}

	~LUASteering() {
	}

	MoveVector execute(const AIPtr& entity, float speed) const override {
#if AI_EXCEPTIONS
		try {
#endif
			return executeLUA(entity, speed);
#if AI_EXCEPTIONS
		} catch (...) {
			ai_log_error("Exception while running lua steering");
		}
		return MoveVector(VEC3_INFINITE, 0.0f);
#endif
	}
};

}
}
