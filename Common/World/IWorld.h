#pragma once

#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/hash.hpp>

#include <mutex>

#include <Common/Utils/Utils.h>
//#include "Collider.h"
//#include "Player/Player.h"
#include <Common/Chunk/ChunkBuilder.h>
#include <Common/ThreadPool.h>

class IPlayer;
class Chunk;
class ChunkBlock;
class WorldRenderer;
class NetworkClient;

typedef std::unordered_map<glm::ivec2, Chunk*> ChunkMap;
typedef std::unordered_map<int, IPlayer*> PlayersMap;
//typedef	std::unordered_map<int, NetworkClient*> ClientsMap;

enum Directions {
	West,
	East,
	Bottom,
	Top,
	South,
	North
};

class IWorld
{
public:
	IWorld();

	void CommonOnInit();
	virtual void OnInit() = 0;
	void CommonOnUpdate();
	virtual void OnUpdate() = 0;
	void CommonOnFixedUpdate();
	virtual void OnFixedUpdate() = 0;
	void CommonOnTickUpdate();
	virtual void OnTickUpdate() = 0;

	virtual void HandleCreatingNewChunks();
	void UnloadChunksOutsideRenderDistance();

	Chunk* CreateEmptyChunk(glm::ivec2 position);
	Chunk* CreateChunk(glm::ivec2 position);
	Chunk* GenerateNewChunkThreaded(glm::ivec2 position, ChunkAction* nextAction = nullptr);

	Chunk* GetNewChunkNetThreaded(glm::ivec2 position, ChunkAction* nextAction = nullptr);

	Chunk* LoadChunk(glm::ivec2 position);

	void RemoveChunk(Chunk* chunk);
	void RemoveChunk(glm::ivec2 position);
	void RemoveChunk(glm::ivec3 position);

	Chunk* GetChunkAt(glm::ivec2 position);
	Chunk* GetChunkAt(glm::ivec3 position);
	void SetChunkAt(Chunk* chunk);

	ChunkMap& GetChunks();

	bool ChunkExistsAt(glm::ivec2 position);
	bool ChunkExistsAt(glm::vec3 position);

	ChunkBlock* GetBlockAt(glm::ivec3 position);
	ChunkBlock* GetBlockAt(glm::vec3 position);

	// Players
	void AddPlayer(IPlayer* player);
	IPlayer* GetPlayer(int id);
	PlayersMap& GetPlayers();
	void RemovePlayer(int id);

	~IWorld();

private:
	PlayersMap m_Players;
	std::mutex m_PlayersMutex;

public:
	//Player& GetPlayer();

	//Collider m_LookingAtCollider;

	static constexpr float Gravity = -0.6f;

	//ConnectionsMap m_ConnectedClients;

	bool m_IsInitialized = false;

	ChunkMap m_Chunks;

	std::recursive_mutex m_ChunkMutex;
};

namespace Settings 
{
	extern int RenderDistance;
}