#include "World.h"

#include "Chunk/Chunk.h"
#include "Utils.h"
#include <iostream>
#include <glm/vec3.hpp>

Chunk* World::CreateChunk(glm::vec3 position)
{
	int chunkIndex = Utils::ChunkPositionToIndex(position);

	m_Chunks[chunkIndex] = new Chunk(position);

	return m_Chunks[chunkIndex];
}

std::map<int, Chunk*>& World::GetChunks()
{
	return m_Chunks;
}

Chunk* World::GetChunkFromIndex(int index)
{
	return m_Chunks[index];
}

void World::Render()
{
	for (auto const& entry : m_Chunks)
		entry.second->Render();
}

unsigned int World::m_ChunkCount = 0;