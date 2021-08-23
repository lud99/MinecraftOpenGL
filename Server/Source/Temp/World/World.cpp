#include "World.h"

#include <iostream>

#include <glm/vec3.hpp>

#include "WorldRenderer.h"
#include "Player/Player.h"
#include "Chunk/Chunk.h"
#include "Chunk/ChunkIO.h"
#include "../Utils/Utils.h"
#include "../Blocks.h"
#include "Collider.h"
#include <mutex>
#include <GLFW/glfw3.h>

#include <Common/NetworkThread.h>

#ifndef SERVER_BUILD
	#include <irrklang/irrKlang.h>
#endif

void World::Init(GLFWwindow* window)
{
	int chunkCount = (Settings::RenderDistance * 2) * (Settings::RenderDistance * 2);
	std::cout << "Using render distance " << Settings::RenderDistance << ". Total chunks: " << chunkCount << "\n";

	m_Window = window;

#ifndef SERVER_BUILD
	SoundEngine = irrklang::createIrrKlangDevice();
#endif

	m_TextureAtlas.Load();
	BlockTypes::CreateBlocks();

	m_Player.Init();
	m_Player.SetWindow(window);

	m_Renderer = new WorldRenderer();

	m_LookingAtCollider.Init();
}

void World::Update()
{
	HandleCreatingNewChunks();
	UnloadChunksOutsideRenderDistance();

	m_Player.OnUpdate();

	m_ChunkBuilder.ProcessQueue();
}

void World::FixedUpdate()
{
	m_Player.OnFixedUpdate();
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
			Chunk* chunk = GetChunkAt(chunkPos);

			//ChunkAction* nextAction = new ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, chunk, ChunkAction::Priority::Low);

			// Generate chunk and send it to the player

			// Create the chunk here if a chunk at this position doesn't exist
			if (!chunk)
			{
				NetworkThread& net = net.Instance();
				//ENetPeer* peer = m_ConnectedClients[m_Player.m_ClientId];

				chunk = GenerateNewChunkThreaded(chunkPos);
			}		
	}

	// Generate the terrain
	for (int x = position.x - RenderDistance * Chunk::Width; x < position.x + RenderDistance * Chunk::Width; x += Chunk::Width)
	{
		for (int z = position.z - RenderDistance * Chunk::Depth; z < position.z + RenderDistance * Chunk::Depth; z += Chunk::Depth)
		{
			glm::ivec2 chunkPos = Utils::WorldPositionToChunkPosition(glm::vec3(x, 0, z) + 8.0f);

			Chunk* chunk = GetChunkAt(chunkPos);
			if (!chunk)
				continue;

			AdjacentChunks adjacentChunks = chunk->GetAdjacentChunks();

			// Rebuild the chunk if changes has occured and it's not already rebuilding. For example if a block has been broken
			if (chunk->m_IsInitialized && chunk->IsDirty() && !chunk->m_IsRebuilding)
			{
				chunk->RebuildMeshThreaded(ChunkAction::Priority::VeryHigh);
				//m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, chunk, ChunkAction::Priority::Low));

				//if (adjacentChunks.West)// && (!westChunkEast || !westChunkEast->m_HasGenerated))
				//	World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, adjacentChunks.West, ChunkAction::Priority::Low));
				//if (adjacentChunks.East)// && (!eastChunkWest || !eastChunkWest->m_HasGenerated))
				//	World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, adjacentChunks.East, ChunkAction::Priority::Low));
				//if (adjacentChunks.North)// && (!northChunkSouth || !northChunkSouth->m_HasGenerated))
				//	World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, adjacentChunks.North, ChunkAction::Priority::Low));
				//if (adjacentChunks.South)//&& (!southChunkNorth || !southChunkNorth->m_HasGenerated))
				//	World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, adjacentChunks.South, ChunkAction::Priority::Low));

				/*if (adjacentChunks.South && !adjacentChunks.South->m_AdjacentChunksWhenLastRebuilt.North)
					adjacentChunks.South->SetDirty(true);
				if (adjacentChunks.North && !adjacentChunks.North->m_AdjacentChunksWhenLastRebuilt.South)
					adjacentChunks.North->SetDirty(true);
				if (adjacentChunks.West && !adjacentChunks.West->m_AdjacentChunksWhenLastRebuilt.East)
					adjacentChunks.West->SetDirty(true);
				if (adjacentChunks.East && !adjacentChunks.East->m_AdjacentChunksWhenLastRebuilt.West)
					adjacentChunks.East->SetDirty(true);*/
			}

			/*if (!chunk->m_IsInitialized) // Only generate if the chunk exists and has been initalized properly
				continue;

			if (chunk->m_HasGenerated || chunk->m_IsGenerating) // Don't generate the terrain twice
				continue;

			AdjacentChunks adjacentChunks = chunk->GetAdjacentChunks();
			if (adjacentChunks.West && !adjacentChunks.West->m_IsInitialized)
				continue;
			if (adjacentChunks.North && !adjacentChunks.North->m_IsInitialized)
				continue;
			if (adjacentChunks.East && !adjacentChunks.East->m_IsInitialized)
				continue;
			if (adjacentChunks.South && !adjacentChunks.South->m_IsInitialized)
				continue;

			std::cout << "Generate: " << chunkPos.x << "; " << chunkPos.y << "\n";*/

			// Add a action to be run after the generation is completed, which is to rebuild the adjacent chunks
			//ChunkAction* nextAction = new ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, chunk);
			//nextAction->type = ChunkAction::ActionType::Rebuild;// RebuildAdjacentChunks;
			//nextAction->chunk = chunk;
			//nextAction->SetTimestamp();
			//m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, chunk, ChunkAction::Priority::Low));

			//chunk->GenerateTerrainThreaded(nextAction);*/
		}
	}
}

void World::UnloadChunksOutsideRenderDistance()
{
	using namespace Settings;

	// Iterate through all chunks
	for (auto& entry : m_Chunks)
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

Chunk* World::CreateEmptyChunk(glm::ivec2 position)
{
	std::lock_guard<std::recursive_mutex> lk(m_ChunkMutex);

	m_Chunks[position] = new Chunk(position);

	return GetChunkAt(position);
}

Chunk* World::CreateChunk(glm::ivec2 position)
{
	std::lock_guard<std::recursive_mutex> lk(m_ChunkMutex);

	Chunk* chunk = new Chunk(position);
	chunk->Init();

	m_Chunks[position] = chunk;

	return chunk;
}

Chunk* World::GenerateNewChunkThreaded(glm::ivec2 position, ChunkAction* nextAction)
{
	std::unique_lock<std::recursive_mutex> lk(m_ChunkMutex);

	Chunk* chunk = new Chunk(position);
	m_Chunks[position] = chunk;

	lk.unlock();

	chunk->CreateGenerateAndBuild(ChunkAction::Priority::High, nextAction);

	return chunk;
}

Chunk* World::LoadChunk(glm::ivec2 position)
{
	ChunkFile chunkData = ChunkIO::ReadFile("test.txt");

	Chunk* chunk = CreateEmptyChunk(position);

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

	ChunkAction* nextAction = nullptr;/* new ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, chunk);
	nextAction->type = ChunkAction::ActionType::RebuildAdjacentChunks;
	nextAction->chunk = chunk;
	nextAction->SetTimestamp();*/

	chunk->RebuildMeshThreaded(ChunkAction::Priority::High, nextAction);

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
	std::lock_guard<std::recursive_mutex> lk(m_ChunkMutex);

	if (m_Chunks.count(position) == 1)
		return m_Chunks.at(position);

	return nullptr;
}

Chunk* World::GetChunkAt(glm::ivec3 position) { return GetChunkAt(glm::ivec2(position.x, position.z)); }

void World::SetChunkAt(Chunk* chunk)
{
	std::lock_guard<std::recursive_mutex> lk(m_ChunkMutex);

	m_Chunks[chunk->GetPosition()] = chunk;
}

bool World::ChunkExistsAt(glm::ivec2 position) 
{ 
	return m_Chunks.count(position) == 1; 
}

bool World::ChunkExistsAt(glm::vec3 position) { return ChunkExistsAt(glm::ivec2(position.x, position.z)); }

ChunkBlock* World::GetBlockAt(glm::ivec3 position)
{
	// Check the y position bounds (0 - 255) to avoid overflow
	if (position.y < 0 || position.y > 255)
		return nullptr;

	glm::ivec2 chunkPosition = Utils::WorldPositionToChunkPosition(position);
	Chunk* chunk = GetChunkAt(chunkPosition);

	if (!chunk || !chunk->m_IsInitialized) return nullptr;

	glm::u8vec3 blockPosition = Utils::WorldBlockPositionToLocalBlockPosition(position, chunk->GetPosition());
	ChunkBlock* block = chunk->GetBlockAt(blockPosition);

	return block;
}
ChunkBlock* World::GetBlockAt(glm::vec3 position)
{
	return GetBlockAt((glm::ivec3) glm::floor(position));
}

Player& World::GetPlayer() { return m_Player; };

TextureAtlas World::m_TextureAtlas;
Collider World::m_LookingAtCollider;
WorldRenderer* World::m_Renderer;
ChunkBuilder World::m_ChunkBuilder;
irrklang::ISoundEngine* World::SoundEngine;

unsigned int World::m_ChunkCount = 0;

int Settings::RenderDistance = 2;