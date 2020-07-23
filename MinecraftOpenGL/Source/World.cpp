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

Chunk* World::GetChunkAtPosition(glm::vec3 position)
{
	int index = Utils::ChunkPositionToIndex(position);

	return GetChunkFromIndex(index);
}

bool World::ChunkExistsAtPosition(glm::vec3 position)
{
	int index = Utils::ChunkPositionToIndex(position);

	bool b = m_Chunks.count(index) > 0;

	if (b)
	{
		Chunk* chunk = GetChunkFromIndex(index);
		glm::vec3 p = chunk->GetPosition();

		int a = 2;
	}

	return b;

	/*
	-1, 0, 0 = -1
	*/
}

void World::Render()
{
	for (auto const& entry : m_Chunks)
	{
		Chunk* c = entry.second;
		entry.second->Render();
	}
		
}

unsigned int World::m_ChunkCount = 0;