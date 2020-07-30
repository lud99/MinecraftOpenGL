#pragma once

#include <map>
#include <glm/vec3.hpp>

#include "Utils.h"
#include "Textures/TextureList.h"

class Chunk;

namespace World
{
	Chunk* CreateChunk(glm::ivec2 position);

	std::map<int, Chunk*>& GetChunks();

	Chunk* GetChunkFromIndex(int index);
	Chunk* GetChunkAtPosition(glm::ivec2 position);

	bool ChunkExistsAtIndex(int index);
	bool ChunkExistsAtPosition(glm::ivec2 position);

	void Init();

	void Render();

	extern unsigned int m_ChunkCount;

	extern TextureList Textures;

	namespace {
		std::map<int, Chunk*> m_Chunks;
	}
};

