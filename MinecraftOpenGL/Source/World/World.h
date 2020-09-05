#pragma once

#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/hash.hpp>

#include <GLFW/glfw3.h>

#include <mutex>

#include "../Utils.h"
#include "../Graphics/Textures/TextureAtlas.h"
#include "../Graphics/Mesh.h"
#include "../Collider.h"
#include "WorldRenderer.h"
#include "Player.h"
#include "Chunk/ChunkBuilder.h"

typedef Faces Directions;

class Chunk;

typedef std::unordered_map<glm::ivec2, Chunk*> ChunkMap;

namespace World
{
	void Init(GLFWwindow* window);
	void Update();
	void Render();

	Chunk* CreateChunk(glm::ivec2 position);
	Chunk* CreateChunk(glm::ivec3 position);

	Chunk* GetChunkAt(glm::ivec2 position);
	Chunk* GetChunkAt(glm::ivec3 position);

	ChunkMap& GetChunks();

	bool ChunkExistsAt(glm::ivec2 position);
	bool ChunkExistsAt(glm::ivec3 position);

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

		std::mutex m_ChunkMutex;
	}
};