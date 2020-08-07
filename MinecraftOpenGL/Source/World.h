#pragma once

#include <map>
#include <glm/vec3.hpp>

#include <GLFW/glfw3.h>

#include "Utils.h"
#include "Textures/TextureList.h"
#include "Player.h"

class Player;
class Chunk;

namespace World
{
	void Init(GLFWwindow* window);
	void Update();
	void Render();

	Player& GetPlayer();

	Chunk* CreateChunk(glm::ivec2 position);
	std::map<int, Chunk*>& GetChunks();

	Chunk* GetChunkFromIndex(int index);
	Chunk* GetChunkAtPosition(glm::ivec2 position);

	bool ChunkExistsAtIndex(int index);
	bool ChunkExistsAtPosition(glm::ivec2 position);

	extern unsigned int m_ChunkCount;

	extern TextureList Textures;

	namespace {
		GLFWwindow* m_Window;

		std::map<int, Chunk*> m_Chunks;

		Player m_Player;
	}
};

