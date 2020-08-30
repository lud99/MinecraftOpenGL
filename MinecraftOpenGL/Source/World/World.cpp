#include "World.h"

#include <iostream>

#include <glm/vec3.hpp>

#include "Player.h"
#include "Chunk/Chunk.h"
#include "../Utils.h"
#include "../Blocks/BlockTypes.h"
#include "../Collider.h"

void World::Init(GLFWwindow* window)
{
	m_Window = window;

	m_TextureAtlas.Load();
	BlockTypes::CreateBlocks();

	m_Player.Init();
	m_Player.SetWindow(window);

	m_Renderer = new WorldRenderer();

	m_LookingAtCollider.Init();
}

void World::Update()
{
	m_Player.Update();

	m_ChunkBuilder.ProcessQueue();
}

void World::Render() { m_Renderer->Render(); }

Chunk* World::CreateChunk(glm::ivec2 position)
{
	int chunkIndex = Utils::ChunkPositionToIndex(position);

	m_Chunks[chunkIndex] = new Chunk(position);

	return m_Chunks[chunkIndex];
}

std::map<int, Chunk*>& World::GetChunks() { return m_Chunks; }

Chunk* World::GetChunkFromIndex(int index)
{
	// Check if it exists
	if (ChunkExistsAtIndex(index))
		return m_Chunks.at(index);

	return nullptr;
}

Chunk* World::GetChunkAtPosition(glm::ivec2 position)
{
	int index = Utils::ChunkPositionToIndex(position);

	return GetChunkFromIndex(index);
}

bool World::ChunkExistsAtIndex(int index) { return m_Chunks.count(index) > 0; }

bool World::ChunkExistsAtPosition(glm::ivec2 position)
{
	int index = Utils::ChunkPositionToIndex(position);

	return m_Chunks.count(index) > 0;
}

Player& World::GetPlayer() { return m_Player; };

TextureAtlas World::m_TextureAtlas;
Collider World::m_LookingAtCollider;
WorldRenderer* World::m_Renderer;
ChunkBuilder World::m_ChunkBuilder;

unsigned int World::m_ChunkCount = 0;