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
	if (position.x == 0 && position.y == 8)
		int a = 2;

	m_Chunks[position] = new Chunk(position);

	return m_Chunks[position];
}

Chunk* World::CreateChunk(glm::ivec3 position) { return CreateChunk(glm::ivec2(position.x, position.z)); }

ChunkMap& World::GetChunks() { return m_Chunks; }

Chunk* World::GetChunkAt(glm::ivec2 position)
{
	// Check if it exists
	if (ChunkExistsAt(position))
		return m_Chunks.at(position);

	return nullptr;
}

Chunk* World::GetChunkAt(glm::ivec3 position) { return GetChunkAt(glm::ivec2(position.x, position.z)); }

bool World::ChunkExistsAt(glm::ivec2 position) { return m_Chunks.count(position) > 0; }
bool World::ChunkExistsAt(glm::ivec3 position) { return m_Chunks.count(glm::ivec2(position.x, position.z)) > 0; }

Player& World::GetPlayer() { return m_Player; };

TextureAtlas World::m_TextureAtlas;
Collider World::m_LookingAtCollider;
WorldRenderer* World::m_Renderer;
ChunkBuilder World::m_ChunkBuilder;

unsigned int World::m_ChunkCount = 0;