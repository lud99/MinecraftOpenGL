#pragma once

#include <map>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#include <GLFW/glfw3.h>

#include "../Utils.h"
#include "../Graphics/Textures/TextureAtlas.h"
#include "../Graphics/Mesh.h"
#include "../Collider.h"
#include "WorldRenderer.h"
#include "Player.h"
#include "Chunk/ChunkBuilder.h"

typedef Faces Directions;

class Chunk;

namespace World
{
	void Init(GLFWwindow* window);
	void Update();
	void Render();

	Chunk* CreateChunk(glm::ivec2 position);
	std::map<int, Chunk*>& GetChunks();

	Chunk* GetChunkFromIndex(int index);
	Chunk* GetChunkAtPosition(glm::ivec2 position);

	bool ChunkExistsAtIndex(int index);
	bool ChunkExistsAtPosition(glm::ivec2 position);

	Player& GetPlayer();

	extern unsigned int m_ChunkCount;
	extern TextureAtlas m_TextureAtlas;
	extern Collider m_LookingAtCollider;
	extern WorldRenderer* m_Renderer;
	extern ChunkBuilder m_ChunkBuilder;

	namespace {
		GLFWwindow* m_Window;

		std::map<int, Chunk*> m_Chunks;

		Player m_Player;
	}
};