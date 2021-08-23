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
#include <Graphics/Textures/TextureAtlas.h>

class IPlayer;
class Chunk;
class ChunkBlock;
class WorldRenderer;
class NetworkConnection;

//namespace irrklang
//{
//	class ISoundEngine;
//};

typedef std::unordered_map<glm::ivec2, Chunk*> ChunkMap;

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

	~IWorld();

public:
	//Player& GetPlayer();

	//Collider m_LookingAtCollider;

	//extern irrklang::ISoundEngine* SoundEngine;
	static constexpr float Gravity = -0.6f;

	std::unordered_map<int, NetworkConnection*> m_ConnectedClients;
	std::unordered_map<int, IPlayer*> m_Players;

	ChunkMap m_Chunks;

	std::recursive_mutex m_ChunkMutex;

	/*namespace {
		GLFWwindow* m_Window;

		ChunkMap m_Chunks;

		Player m_Player;

		std::recursive_mutex m_ChunkMutex;
	}*/
};

namespace Settings 
{
	extern int RenderDistance;
}