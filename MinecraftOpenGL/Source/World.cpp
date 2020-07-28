#include "World.h"

#include <iostream>

#include <glm/vec3.hpp>

#include "Chunk/Chunk.h"
#include "Utils.h"
#include "Blocks/BlockTypes.h"

void World::Init()
{
	Textures.Load();
	BlockTypes::CreateBlocks();
}

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
	// Check if it exists
	if (ChunkExistsAtIndex(index))
		return m_Chunks.at(index);
	
	return nullptr;
}

Chunk* World::GetChunkAtPosition(glm::vec3 position)
{
	int index = Utils::ChunkPositionToIndex(position);

	return GetChunkFromIndex(index);
}

bool World::ChunkExistsAtIndex(int index)
{
	return m_Chunks.count(index) > 0;
}

bool World::ChunkExistsAtPosition(glm::vec3 position)
{
	int index = Utils::ChunkPositionToIndex(position);

	return m_Chunks.count(index) > 0;
}

void World::Render()
{
	for (auto const& entry : m_Chunks)
	{
		entry.second->Render();
	}
		
}

TextureList World::Textures;
unsigned int World::m_ChunkCount = 0;