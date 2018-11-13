/**
 * @file
 */
#pragma once

#include <unordered_map>
#include <memory>

#include "group/GroupId.h"
#include "aggro/AggroMgr.h"
#include "ICharacter.h"
#include "tree/TreeNode.h"
#include "tree/loaders/ITreeLoader.h"
#include "common/Thread.h"
#include "common/Types.h"
#include "common/NonCopyable.h"
#include "common/Math.h"

namespace ai {

class ICharacter;
typedef std::shared_ptr<ICharacter> ICharacterPtr;
class Zone;

typedef std::vector<CharacterId> FilteredEntities;

#ifndef AI_NOTHING_SELECTED
#define AI_NOTHING_SELECTED (-1)
#endif

/**
 * @brief This is the type the library works with. It interacts with it's real world entity by
 * the @ai{ICharacter} interface.
 *
 * Each ai entity has a @ai[AggroMgr} assigned that is updated with each tick (update()).
 *
 * A behaviour can be replaced at runtime with setBehaviour()
 *
 * You can set single @c AI instances to no longer update their state by calling setPause()
 */
class AI : public NonCopyable, public std::enable_shared_from_this<AI> {
	friend class TreeNode;
	friend class LUAAIRegistry;
	friend class IFilter;
	friend class Filter;
	friend class Server;
protected:
	/**
	 * This map is only filled if we are in debugging mode for this entity
	 */
	typedef std::unordered_map<int, TreeNodeStatus> NodeStates;
	NodeStates _lastStatus;
	/**
	 * This map is only filled if we are in debugging mode for this entity
	 */
	typedef std::unordered_map<int, uint64_t> LastExecMap;
	LastExecMap _lastExecMillis;

	/**
	 * @note The filtered entities are kept even over several ticks. The caller should decide
	 * whether he still needs an old/previous filtered selection
	 * @sa @ai{IFilter}
	 */
	mutable FilteredEntities _filteredEntities;

	void setFilteredEntities(const FilteredEntities& filteredEntities);

	void addFilteredEntity(CharacterId id);

	/**
	 * Often @ai{Selector} states must be stored to continue in the next step at a particular
	 * position in the behaviour tree. This map is doing exactly this.
	 */
	typedef std::unordered_map<int, int> SelectorStates;
	SelectorStates _selectorStates;

	/**
	 * This map stores the amount of execution for the @ai{Limit} node. The key is the node id
	 */
	typedef std::unordered_map<int, int> LimitStates;
	LimitStates _limitStates;

	TreeNodePtr _behaviour;
	AggroMgr _aggroMgr;

	ICharacterPtr _character;

	bool _pause;
	bool _debuggingActive;

	int64_t _time;

	Zone* _zone;

	std::atomic_bool _reset;
public:
	/**
	 * @param behaviour The behaviour tree node that is applied to this ai entity
	 */
	explicit AI(const TreeNodePtr& behaviour) :
			_behaviour(behaviour), _pause(false), _debuggingActive(false), _time(0L), _zone(nullptr), _reset(false) {
	}
	virtual ~AI() {
	}

	/**
	 * @brief Update the behaviour and the aggro values if the entity is not on hold.
	 * @param[in] dt The current milliseconds to update the aggro entries and
	 * time based tasks or conditions.
	 */
	virtual void update(int64_t dt, bool debuggingActive);

	/**
	 * @brief Set the new @ai{Zone} this entity is in
	 */
	void setZone(Zone* zone);
	/**
	 * Returns the @ai{Zone} this entity is in.
	 */
	Zone* getZone() const;
	/**
	 * @brief Returns @c true if the entity is already in a @ai{Zone}. This must not be managed manually,
	 * the @c Zone is doing that already.
	 */
	bool hasZone() const;

	/**
	 * @brief don't update the entity as long as it is paused
	 * @sa isPause()
	 */
	void setPause(bool pause);

	/**
	 * @brief don't update the entity as long as it is paused
	 * @sa setPause()
	 */
	bool isPause() const;

	/**
	 * @return @c true if the owning entity is currently under debugging, @c false otherwise
	 */
	bool isDebuggingActive() const;

	/**
	 * @brief Get the current behaviour for this ai
	 */
	TreeNodePtr getBehaviour() const;
	/**
	 * @brief Set a new behaviour
	 * @return the old one if there was any
	 */
	TreeNodePtr setBehaviour(const TreeNodePtr& newBehaviour);
	/**
	 * @return The real world entity reference
	 */
	ICharacterPtr getCharacter() const;
	/**
	 * You might not set a character twice to an @c AI instance.
	 */
	void setCharacter(const ICharacterPtr& character);

	template <typename CharacterType>
	inline CharacterType& getCharacterCast() const {
		return *static_cast<CharacterType*>(_character.get());
	}

	int64_t getTime() const;

	CharacterId getId() const;

	/**
	 * @return the @c AggroMgr for this @c AI instance. Each @c AI instance has its own @c AggroMgr instance.
	 */
	AggroMgr& getAggroMgr();
	/**
	 * @return the @c AggroMgr for this @c AI instance. Each @c AI instance has its own @c AggroMgr instance.
	 */
	const AggroMgr& getAggroMgr() const;

	/**
	 * @brief @c FilteredEntities is holding a list of @c CharacterIds that were selected by the @c Select condition.
	 * @sa @c IFilter interface.
	 * @sa @c Filter condition that executes assigned @c IFilter implementations.
	 * @return A reference to the internal data structure. This should only be used from within @c TreeNode implementations
	 * to access those entities that were filtered by the @c Filter condition.
	 *
	 * @note If you call this from outside of the behaviour tree tick, you will run into race conditions.
	 */
	const FilteredEntities& getFilteredEntities() const;

	/**
	 * If the object is currently maintained by a shared_ptr, you can get a shared_ptr from a raw pointer
	 * instance that shares the state with the already existing shared_ptrs around.
	 */
	inline std::shared_ptr<AI> ptr() {
		return shared_from_this();
	}
};

inline TreeNodePtr AI::getBehaviour() const {
	return _behaviour;
}

inline TreeNodePtr AI::setBehaviour(const TreeNodePtr& newBehaviour) {
	TreeNodePtr current = _behaviour;
	_behaviour = newBehaviour;
	_reset = true;
	return current;
}

inline void AI::setPause(bool pause) {
	_pause = pause;
}

inline bool AI::isPause() const {
	return _pause;
}

inline ICharacterPtr AI::getCharacter() const {
	return _character;
}

inline void AI::setCharacter(const ICharacterPtr& character) {
	ai_assert(!_character, "There is already a character set");
	_character = character;
}

inline AggroMgr& AI::getAggroMgr() {
	return _aggroMgr;
}

inline const AggroMgr& AI::getAggroMgr() const {
	return _aggroMgr;
}

inline const FilteredEntities& AI::getFilteredEntities() const {
	return _filteredEntities;
}

inline void AI::setFilteredEntities(const FilteredEntities& filteredEntities) {
	_filteredEntities = filteredEntities;
}

inline void AI::addFilteredEntity(CharacterId id) {
	_filteredEntities.push_back(id);
}

inline bool AI::isDebuggingActive() const {
	return _debuggingActive;
}

inline void AI::setZone(Zone* zone) {
	_zone = zone;
}

inline Zone* AI::getZone() const {
	return _zone;
}

inline bool AI::hasZone() const {
	return _zone != nullptr;
}

inline int64_t AI::getTime() const {
	return _time;
}

inline CharacterId AI::getId() const {
	if (!_character) {
		return AI_NOTHING_SELECTED;
	}
	return _character->getId();
}

inline void AI::update(int64_t dt, bool debuggingActive) {
	if (isPause()) {
		return;
	}

	if (_character) {
		_character->update(dt, debuggingActive);
	}

	if (_reset) {
		// safe to do it like this, because update is not called from multiple threads
		_reset = false;
		_lastStatus.clear();
		_lastExecMillis.clear();
		_filteredEntities.clear();
		_selectorStates.clear();
	}

	_debuggingActive = debuggingActive;
	_time += dt;
	_aggroMgr.update(dt);
}

typedef std::shared_ptr<AI> AIPtr;

}
