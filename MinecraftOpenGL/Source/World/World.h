#pragma once

#include <map>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#include <GLFW/glfw3.h>

#include "../Utils.h"
#include "../Graphics/Textures/TextureAtlas.h"
#include "../Graphics/Mesh.h"
#include "../Collider.h"
#include "Player.h"
#include "WorldRenderer.h"

typedef Faces Directions;

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

	extern TextureAtlas m_TextureAtlas;

	extern Collider m_LookingAtCollider;

	extern WorldRenderer* m_Renderer;

	namespace {
		GLFWwindow* m_Window;

		std::map<int, Chunk*> m_Chunks;

		Player m_Player;
	}
};