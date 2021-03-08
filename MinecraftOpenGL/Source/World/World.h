#pragma once

#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/hash.hpp>

#include <mutex>

#include "../Utils/Utils.h"
#include <Graphics/Textures/TextureAtlas.h>
#include <Graphics/Mesh.hpp>
#include <Graphics/BasicVertices.h>
#include "Collider.h"
#include "Player/Player.h"
#include "Chunk/ChunkBuilder.h"

class Chunk;
class WorldRenderer;

/*
class HashFn
{
	std::size_t operator()(glm::ivec2 const& k) const noexcept;
	//bool operator()(const glm::ivec2& a, const glm::ivec2& b)const;
};*/

typedef std::unordered_map<glm::ivec2, Chunk*> ChunkMap;

enum Directions {
	West,
	East,
	Bottom,
	Top,
	South,
	North
};

namespace World
{
	void Init(GLFWwindow* window);
	void Update(float deltaTime);
	void Render();

	void HandleCreatingNewChunks();
	void UnloadChunksOutsideRenderDistance();

	Chunk* CreateChunk(glm::ivec2 position);
	Chunk* CreateChunk(glm::ivec3 position);

	Chunk* LoadChunk(glm::ivec2 position);

	void RemoveChunk(Chunk* chunk);
	void RemoveChunk(glm::ivec2 position);
	void RemoveChunk(glm::ivec3 position);

	Chunk* GetChunkAt(glm::ivec2 position);
	Chunk* GetChunkAt(glm::ivec3 position);

	ChunkMap& GetChunks();

	bool ChunkExistsAt(glm::ivec2 position);
	bool ChunkExistsAt(glm::ivec3 position);

	ChunkBlock* GetBlockAt(glm::ivec3 position);
	ChunkBlock* GetBlockAt(glm::vec3 position);

	Player& GetPlayer();

	extern unsigned int m_ChunkCount;
	extern TextureAtlas m_TextureAtlas;
	extern Collider m_LookingAtCollider;
	extern WorldRenderer* m_Renderer;
	extern ChunkBuilder m_ChunkBuilder;

	namespace {
		GLFWwindow* m_Window;

		ChunkMap m_Chunks;

		Player m_Player;

		std::recursive_mutex m_ChunkMutex;
	}
};

namespace Settings 
{
	extern int RenderDistance;
}