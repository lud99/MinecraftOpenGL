#pragma once

#include <map>
#include <glm/vec3.hpp>

#include "Utils.h"

class Chunk;

namespace World
{
	Chunk* CreateChunk(glm::vec3 position);

	std::map<int, Chunk*>& GetChunks();

	Chunk* GetChunkFromIndex(int index);
	Chunk* GetChunkAtPosition(glm::vec3 position);

	bool ChunkExistsAtPosition(glm::vec3 position);

	void Render();

	extern unsigned int m_ChunkCount;

	namespace {
		std::map<int, Chunk*> m_Chunks;
	}
};

