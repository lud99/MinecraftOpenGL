#include "World.h"

#include <iostream>

#include <glm/vec3.hpp>

#include "WorldRenderer.h"
#include "Player/Player.h"
#include "Chunk/Chunk.h"
#include "Chunk/ChunkIO.h"
#include "../Utils/Utils.h"
#include "../Blocks/BlockTypes.h"
#include "Collider.h"

#include <GLFW/glfw3.h>

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

void World::Update(float deltaTime)
{
	HandleCreatingNewChunks();
	UnloadChunksOutsideRenderDistance();

	m_Player.Update(deltaTime);

	m_ChunkBuilder.ProcessQueue();
}

void World::Render() { m_Renderer->Render(); }

void World::HandleCreatingNewChunks()
{
	using namespace Settings;

	glm::ivec3 position = glm::floor(m_Player.m_Position);

	// Check all the chunks in a box around the player
	for (int x = position.x - RenderDistance * Chunk::Width; x < position.x + RenderDistance * Chunk::Width; x += Chunk::Width)
	{
		for (int z = position.z - RenderDistance * Chunk::Depth; z < position.z + RenderDistance * Chunk::Depth; z += Chunk::Depth)
		{
			glm::ivec2 chunkPos = Utils::WorldPositionToChunkPosition(glm::vec3(x, 0, z) + 8.0f);

			// Create the chunk here if a chunk at this position doesn't exist
			if (!ChunkExistsAt(chunkPos))
				CreateChunk(chunkPos);
		}
	}

	// Generate the terrain
	for (int x = position.x - RenderDistance * Chunk::Width; x < position.x + RenderDistance * Chunk::Width; x += Chunk::Width)
	{
		for (int z = position.z - RenderDistance * Chunk::Depth; z < position.z + RenderDistance * Chunk::Depth; z += Chunk::Depth)
		{
			glm::ivec2 chunkPos = Utils::WorldPositionToChunkPosition(glm::vec3(x, 0, z) + 8.0f);

			Chunk* chunk = GetChunkAt(chunkPos);

			if (chunk->m_HasGenerated || chunk->m_IsGenerating) // Don't generate the terrain twice
				continue;

			// Add a action to be run after the generation is completed, which is to rebuild the adjacent chunks
			ChunkAction* nextAction = new ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, chunk);
			nextAction->type = ChunkAction::ActionType::RebuildAdjacentChunks;
			nextAction->chunk = chunk;
			nextAction->SetTimestamp();

			chunk->GenerateTerrainThreaded(nextAction);
		}
	}
}

void World::UnloadChunksOutsideRenderDistance()
{
	using namespace Settings;

	// Iterate through all chunks
	for (auto entry : m_Chunks)
	{
		Chunk* chunk = entry.second;

		glm::vec2 pos = Utils::WorldPositionToChunkPosition(m_Player.m_Position + 8.0f);

		float left = pos.x - RenderDistance - 1;
		float right = pos.x + RenderDistance;
		float top = pos.y - RenderDistance - 1;
		float bottom = pos.y + RenderDistance;

		glm::vec2 chunkPosition = chunk->GetPosition();

		if (chunkPosition.x > left && chunkPosition.x < right && chunkPosition.y > top && chunkPosition.y < bottom)
		{

		}
		else
		{
			//chunk->m_ShouldBeRemoved = true;
			//std::cout << "removing chunk " << chunkPosition.x << ", " << chunkPosition.y << "\n";
			//World::RemoveChunk(chunk);
		}

		// Check all the chunks in a box around the player
		/*for (int x = m_Position.x - renderDistance * Chunk::Width; x < m_Position.x + renderDistance * Chunk::Width; x += Chunk::Width)
		{
			for (int z = m_Position.z - renderDistance * Chunk::Depth; z < m_Position.z + renderDistance * Chunk::Depth; z += Chunk::Depth)
			{
				glm::ivec2 chunkPosition = Utils::WorldPositionToChunkPosition(glm::vec3(x, 0, z) + 8.0f);
			}
		}

		glm::ivec2 chunkPosition = chunk->GetWorldPosition() + Chunk::Width / 2;

		glm::ivec2 delta = chunkPosition - glm::ivec2(m_Position.x, m_Position.z);
		int distanceToChunk = std::abs(delta.x) + std::abs(delta.y);

		if (distanceToChunk > renderDistance * 2 * Chunk::Width)
		{
			World::RemoveChunk(chunk);
		}*/
	}
}

Chunk* World::CreateChunk(glm::ivec3 position)
{
	return CreateChunk(glm::ivec2(position.x, position.z));
}

Chunk* World::CreateChunk(glm::ivec2 position)
{
	m_Chunks[position] = new Chunk(position);

	return m_Chunks[position];
}

Chunk* World::LoadChunk(glm::ivec2 position)
{
	ChunkFile chunkData = ChunkIO::ReadFile("test.txt");

	Chunk* chunk = CreateChunk(position);

	// Iterate through all blocks
	for (int x = 0; x < Chunk::Width; x++)
	{
		for (int y = 0; y < Chunk::Height; y++)
		{
			for (int z = 0; z < Chunk::Depth; z++)
			{
				ChunkBlock* block = chunk->GetBlockAt(glm::vec3(x, y, z));

				block->m_BlockId = (int)chunkData.blocks[x][y][z].m_BlockId;
			}
		}
	}

	chunk->m_HasGenerated = true;
	chunk->m_ShouldRebuild = true;

	ChunkAction* nextAction = new ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, chunk);
	nextAction->type = ChunkAction::ActionType::RebuildAdjacentChunks;
	nextAction->chunk = chunk;
	nextAction->SetTimestamp();

	chunk->RebuildMeshThreaded(nextAction);

	return chunk;
}

void World::RemoveChunk(Chunk* chunk)
{
	RemoveChunk(chunk->GetPosition());
}

void World::RemoveChunk(glm::ivec2 position)
{
	m_Chunks[position]->~Chunk(); // Call the destructor to free up memory

	m_Chunks.erase(position);
}

void World::RemoveChunk(glm::ivec3 position)
{
	RemoveChunk(glm::ivec2(position.x, position.z));
}

ChunkMap& World::GetChunks() { return m_Chunks; }

Chunk* World::GetChunkAt(glm::ivec2 position)
{
	// Check if it exists
	if (ChunkExistsAt(position))
		return m_Chunks.at(position);

	return nullptr;
}

Chunk* World::GetChunkAt(glm::ivec3 position) { return GetChunkAt(glm::ivec2(position.x, position.z)); }

bool World::ChunkExistsAt(glm::ivec2 position) 
{ 
	return m_Chunks.count(position) == 1; 
}
bool World::ChunkExistsAt(glm::ivec3 position) 
{ 
	return m_Chunks.size() > 0 ? m_Chunks.count(glm::ivec2(position.x, position.z)) > 0 : false; 
}

ChunkBlock* World::GetBlockAt(glm::ivec3 position)
{
	glm::ivec2 chunkPosition = Utils::WorldPositionToChunkPosition(position);
	Chunk* chunk = GetChunkAt(chunkPosition);

	glm::u8vec3 blockPosition = Utils::WorldBlockPositionToLocalBlockPosition(position, chunk->GetPosition());
	ChunkBlock* block = chunk->GetBlockAt(blockPosition);

	return block;
}
ChunkBlock* World::GetBlockAt(glm::vec3 position)
{
	return GetBlockAt((glm::ivec3) position);
}

Player& World::GetPlayer() { return m_Player; };

TextureAtlas World::m_TextureAtlas;
Collider World::m_LookingAtCollider;
WorldRenderer* World::m_Renderer;
ChunkBuilder World::m_ChunkBuilder;

unsigned int World::m_ChunkCount = 0;

int Settings::RenderDistance = 16;