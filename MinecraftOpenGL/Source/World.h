#pragma once

#include <map>
#include <glm/vec3.hpp>

class Chunk;

namespace World
{
	Chunk* CreateChunk(glm::vec3 position);

	std::map<int, Chunk*>& GetChunks();

	Chunk* GetChunkFromIndex(int index);

	void Render();

	extern unsigned int m_ChunkCount;

	namespace {
		std::map<int, Chunk*> m_Chunks;
	}
};

