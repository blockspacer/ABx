/**
 * @file
 */
#pragma once

#include "common/Thread.h"
#include "tree/TreeNode.h"
#include <unordered_set>
#include "Network.h"
#include "zone/Zone.h"
#include "AIRegistry.h"
#include "AIStateMessage.h"
#include "AINamesMessage.h"
#include "AIStubTypes.h"
#include "AICharacterDetailsMessage.h"
#include "AICharacterStaticMessage.h"
#include "ProtocolHandlerRegistry.h"
#include "conditions/ConditionParser.h"
#include "tree/TreeNodeParser.h"
#include "tree/TreeNodeImpl.h"

namespace ai {

class AIStateNode;
class AIStateNodeStatic;

class SelectHandler;
class PauseHandler;
class ResetHandler;
class StepHandler;
class ChangeHandler;
class AddNodeHandler;
class DeleteNodeHandler;
class UpdateNodeHandler;
class NopHandler;

/**
 * @brief The server can serialize the state of the @ai{AI} and broadcast it to all connected clients.
 *
 * If you start a server, you can add the @ai{AI} instances to it by calling @ai{addZone()}. If you do so, make
 * sure to remove it when you remove that particular @ai{Zone} instance from your world. You should not do that
 * from different threads. The server should only be managed from one thread.
 *
 * The server will broadcast the world state - that is: It will send out an @ai{AIStateMessage} to all connected
 * clients. If someone selected a particular @ai{AI} instance by sending @ai{AISelectMessage} to the server, it
 * will also broadcast an @ai{AICharacterDetailsMessage} to all connected clients.
 *
 * You can only debug one @ai{Zone} at the same time. The debugging session is shared between all connected clients.
 */
class Server: public INetworkListener {
protected:
	typedef std::unordered_set<Zone*> Zones;
	typedef Zones::const_iterator ZoneConstIter;
	typedef Zones::iterator ZoneIter;
	Zones _zones;
	AIRegistry& _aiRegistry;
	Network _network;
	CharacterId _selectedCharacterId;
	int64_t _time;
	SelectHandler *_selectHandler;
	PauseHandler *_pauseHandler;
	ResetHandler *_resetHandler;
	StepHandler *_stepHandler;
	ChangeHandler *_changeHandler;
	AddNodeHandler *_addNodeHandler;
	DeleteNodeHandler *_deleteNodeHandler;
	UpdateNodeHandler *_updateNodeHandler;
	NopHandler _nopHandler;
	std::atomic_bool _pause;
	// the current active debugging zone
	std::atomic<Zone*> _zone;
	ReadWriteLock _lock = {"server"};
	std::vector<std::string> _names;
	uint32_t _broadcastMask = 0u;

	enum EventType {
		EV_SELECTION,
		EV_STEP,
		EV_UPDATESTATICCHRDETAILS,
		EV_NEWCONNECTION,
		EV_ZONEADD,
		EV_ZONEREMOVE,
		EV_PAUSE,
		EV_RESET,
		EV_SETDEBUG,

		EV_MAX
	};

	struct Event {
		union {
			CharacterId characterId;
			int64_t stepMillis;
			Zone* zone;
			Client* newClient;
			bool pauseState;
		} data;
		std::string strData = "";
		EventType type;
	};
	std::vector<Event> _events;

	void resetSelection();

	void addChildren(const TreeNodePtr& node, std::vector<AIStateNodeStatic>& out) const;
	void addChildren(const TreeNodePtr& node, AIStateNode& parent, const AIPtr& ai) const;

	// only call these from the Server::update method
	void broadcastState(const Zone* zone);
	void broadcastCharacterDetails(const Zone* zone);
	void broadcastStaticCharacterDetails(const Zone* zone);

	void onConnect(Client* client) override;
	void onDisconnect(Client* client) override;

	void handleEvents(Zone* zone, bool pauseState);
	void enqueueEvent(const Event& event);
public:
	Server(AIRegistry& aiRegistry, short port = 10001, const std::string& hostname = "0.0.0.0");
	virtual ~Server();

	/**
	 * @brief Start to listen on the specified port
	 */
	bool start();

	/**
	 * @brief Update the specified node with the given values for the specified @ai{ICharacter} and all the
	 * other characters that are using the same behaviour tree instance
	 *
	 * @param[in] characterId The id of the character where we want to update the specified node
	 * @param[in] nodeId The id of the @ai{TreeNode} to update with the new values
	 * @param[in] name The new name for the node
	 * @param[in] type The new node type (including parameters)
	 * @param[in] condition The new condition (including parameters)
	 *
	 * @see @c TreeNodeParser
	 * @see @c ConditionParser
	 */
	bool updateNode(const CharacterId& characterId, int32_t nodeId, const std::string& name, const std::string& type, const std::string& condition);

	/**
	 * @brief Add a new node with the given values to the specified @ai{ICharacter} and all the
	 * other characters that are using the same behaviour tree instance
	 *
	 * @param[in] characterId The id of the @ai{ICharacter} where we want to add the specified node
	 * @param[in] parentNodeId The id of the @ai{TreeNode} to attach the new @ai{TreeNode} as children
	 * @param[in] name The new name for the node
	 * @param[in] type The new node type (including parameters)
	 * @param[in] condition The new condition (including parameters)
	 *
	 * @see @ai{TreeNodeParser}
	 * @see @ai{ConditionParser}
	 */
	bool addNode(const CharacterId& characterId, int32_t parentNodeId, const std::string& name, const std::string& type, const std::string& condition);

	/**
	 * @brief Delete the specified node from the @ai{ICharacter}'s behaviour tree and all the
	 * other characters that are using the same behaviour tree instance
	 *
	 * @param[in] characterId The id of the @ai{ICharacter} where we want to delete the specified node
	 * @param[in] nodeId The id of the @ai{TreeNode} to delete
	 */
	bool deleteNode(const CharacterId& characterId, int32_t nodeId);

	/**
	 * @brief Adds a new zone to this server instance that can be debugged. The server does not own this pointer
	 * so it also doesn't free it. Every @ai{Zone} that is added here, will be part of the @ai{AINamesMessage}.
	 *
	 * @param zone The @ai{Zone} that should be made available for debugging.
	 *
	 * @note This locks the server instance
	 */
	void addZone(Zone* zone);

	/**
	 * @brief Removes a @ai{Zone} from the server. After this call the given zone is no longer available for debugging
	 * purposes.
	 *
	 * @note This locks the server instance
	 */
	void removeZone(Zone* zone);

	/**
	 * @brief Activate the debugging for this particular zone. And disables the debugging for every other zone
	 *
	 * @note This locks the server instance
	 */
	void setDebug(const std::string& zoneName);

	/**
	 * @brief Resets the @ai{AI} states
	 */
	void reset();

	/**
	 * @brief Select a particular character (resp. @ai{AI} instance) and send detail
	 * information to all the connected clients for this entity.
	 */
	void select(const ClientId& clientId, const CharacterId& id);

	/**
	 * @brief Will pause/unpause the execution of the behaviour trees for all watched @ai{AI} instances.
	 */
	void pause(const ClientId& clientId, bool pause);

	/**
	 * @brief Performs one step of the @ai{AI} in pause mode
	 */
	void step(int64_t stepMillis = 1L);

	/**
	 * @brief call this to update the server - should get called somewhere from your game tick
	 */
	void update(int64_t deltaTime);
};

}
