#include "IWorld.h"

#include <iostream>
#include <optick.h>

#include <glm/vec3.hpp>

//#include "WorldRenderer.h"
//#include "Player/Player.h"
#include <Common/Chunk/Chunk.h>
//#include <Common/Chunk/ChunkIO.h>
#include <Common/Utils/Utils.h>
#include <Common/Blocks/Blocks.h>
#include <Common/Player/IPlayer.h>
//#include "Collider.h"

#include <Common/Net/INetworkThread.h>

#include <mutex>
#include <GLFW/glfw3.h>
#include <irrklang/irrKlang.h>

//void World::Init(GLFWwindow* window)
//{
//	int chunkCount = (Settings::RenderDistance * 2) * (Settings::RenderDistance * 2);
//	std::cout << "Using render distance " << Settings::RenderDistance << ". Total chunks: " << chunkCount << "\n";
//
//	m_Window = window;
//
//	SoundEngine = irrklang::createIrrKlangDevice();
//
//	m_TextureAtlas.Load();
//	BlockTypes::CreateBlocks();
//
//	m_Player.Init();
//	m_Player.SetWindow(window);
//
//	m_Renderer = new WorldRenderer();
//
//	m_LookingAtCollider.Init();
//}

IWorld::IWorld()
{
	//CommonOnInit();
}

void IWorld::CommonOnInit()
{
	OnInit();
}

void IWorld::CommonOnUpdate()
{
	OPTICK_EVENT();
	if (!m_IsInitialized)
		OnInit();

	HandleCreatingNewChunks();
	UnloadChunksOutsideRenderDistance();

	for (auto const& entry : m_Players)
	{
		entry.second->OnUpdate();
	}

	OnUpdate();

	ChunkBuilder::Get().ProcessQueue();
}

void IWorld::CommonOnFixedUpdate()
{
	OPTICK_EVENT();
	for (auto const& entry : m_Players)
	{
		entry.second->OnFixedUpdate();
	}

	OnFixedUpdate();
}

void IWorld::CommonOnTickUpdate()
{
	OPTICK_EVENT();
	for (auto const& entry : m_Players)
	{
		entry.second->OnTickUpdate();
	}

	OnTickUpdate();
}

//void World::Render() { m_Renderer->Render(); }

void IWorld::HandleCreatingNewChunks()
{
	//OPTICK_EVENT();
	//using namespace Settings;

	//IPlayer* player = GetPlayer(0);
	//if (!player) return;

	//glm::ivec3 position = glm::floor(player->m_Position);

	//// Check all the chunks in a box around the player
	//for (int x = position.x - RenderDistance * Chunk::Width; x < position.x + RenderDistance * Chunk::Width; x += Chunk::Width)
	//{
	//	for (int z = position.z - RenderDistance * Chunk::Depth; z < position.z + RenderDistance * Chunk::Depth; z += Chunk::Depth)
	//	{
	//		glm::ivec2 chunkPos = Utils::WorldPositionToChunkPosition(glm::vec3(x, 0, z) + 8.0f);
	//		Chunk* chunk = GetChunkAt(chunkPos);

	//		//ChunkAction* nextAction = new ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, chunk, ChunkAction::Priority::Low);

	//		// Create the chunk here if a chunk at this position doesn't exist
	//		if (!chunk) {
	//			//chunk = (chunkPos);//GenerateNewChunkThreaded(chunkPos);
	//			//std::cout << "newchunk\n";
	//		}
	//	}
	//}

	//// Generate the terrain
	//for (int x = position.x - RenderDistance * Chunk::Width; x < position.x + RenderDistance * Chunk::Width; x += Chunk::Width)
	//{
	//	for (int z = position.z - RenderDistance * Chunk::Depth; z < position.z + RenderDistance * Chunk::Depth; z += Chunk::Depth)
	//	{
	//		glm::ivec2 chunkPos = Utils::WorldPositionToChunkPosition(glm::vec3(x, 0, z) + 8.0f);

	//		Chunk* chunk = GetChunkAt(chunkPos);
	//		if (!chunk)
	//			continue;

	//		AdjacentChunks adjacentChunks = chunk->GetAdjacentChunks();

	//		// Rebuild the chunk if changes has occured and it's not already rebuilding. For example if a block has been broken
	//		if (chunk->m_IsInitialized && chunk->IsDirty() && !chunk->m_IsRebuilding)
	//		{
	//			//chunk->RebuildMeshThreaded(ChunkAction::Priority::VeryHigh);
	//			std::cout << "rebuild\n";
	//			//m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, chunk, ChunkAction::Priority::Low));

	//			//if (adjacentChunks.West)// && (!westChunkEast || !westChunkEast->m_HasGenerated))
	//			//	World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, adjacentChunks.West, ChunkAction::Priority::Low));
	//			//if (adjacentChunks.East)// && (!eastChunkWest || !eastChunkWest->m_HasGenerated))
	//			//	World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, adjacentChunks.East, ChunkAction::Priority::Low));
	//			//if (adjacentChunks.North)// && (!northChunkSouth || !northChunkSouth->m_HasGenerated))
	//			//	World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, adjacentChunks.North, ChunkAction::Priority::Low));
	//			//if (adjacentChunks.South)//&& (!southChunkNorth || !southChunkNorth->m_HasGenerated))
	//			//	World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, adjacentChunks.South, ChunkAction::Priority::Low));

	//			/*if (adjacentChunks.South && !adjacentChunks.South->m_AdjacentChunksWhenLastRebuilt.North)
	//				adjacentChunks.South->SetDirty(true);
	//			if (adjacentChunks.North && !adjacentChunks.North->m_AdjacentChunksWhenLastRebuilt.South)
	//				adjacentChunks.North->SetDirty(true);
	//			if (adjacentChunks.West && !adjacentChunks.West->m_AdjacentChunksWhenLastRebuilt.East)
	//				adjacentChunks.West->SetDirty(true);
	//			if (adjacentChunks.East && !adjacentChunks.East->m_AdjacentChunksWhenLastRebuilt.West)
	//				adjacentChunks.East->SetDirty(true);*/
	//		}

	//		/*if (!chunk->m_IsInitialized) // Only generate if the chunk exists and has been initalized properly
	//			continue;

	//		if (chunk->m_HasGenerated || chunk->m_IsGenerating) // Don't generate the terrain twice
	//			continue;

	//		AdjacentChunks adjacentChunks = chunk->GetAdjacentChunks();
	//		if (adjacentChunks.West && !adjacentChunks.West->m_IsInitialized)
	//			continue;
	//		if (adjacentChunks.North && !adjacentChunks.North->m_IsInitialized)
	//			continue;
	//		if (adjacentChunks.East && !adjacentChunks.East->m_IsInitialized)
	//			continue;
	//		if (adjacentChunks.South && !adjacentChunks.South->m_IsInitialized)
	//			continue;

	//		std::cout << "Generate: " << chunkPos.x << "; " << chunkPos.y << "\n";*/

	//		// Add a action to be run after the generation is completed, which is to rebuild the adjacent chunks
	//		//ChunkAction* nextAction = new ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, chunk);
	//		//nextAction->type = ChunkAction::ActionType::Rebuild;// RebuildAdjacentChunks;
	//		//nextAction->chunk = chunk;
	//		//nextAction->SetTimestamp();
	//		//m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, chunk, ChunkAction::Priority::Low));

	//		//chunk->GenerateTerrainThreaded(nextAction);*/
	//	}
	//}
}

void IWorld::UnloadChunksOutsideRenderDistance()
{
	//OPTICK_EVENT();
	//using namespace Settings;

	////IPlayer* player = GetPlayer(0);
	////if (!player) return;

	//// Iterate through all chunks
	//for (auto& entry : m_Chunks)
	//{
	//	Chunk* chunk = entry.second;

	//	glm::vec2 pos = Utils::WorldPositionToChunkPosition(player->m_Position + 8.0f);

	//	float left = pos.x - RenderDistance - 1;
	//	float right = pos.x + RenderDistance;
	//	float top = pos.y - RenderDistance - 1;
	//	float bottom = pos.y + RenderDistance;

	//	glm::vec2 chunkPosition = chunk->GetPosition();

	//	if (chunkPosition.x > left && chunkPosition.x < right && chunkPosition.y > top && chunkPosition.y < bottom)
	//	{

	//	}
	//	else
	//	{
	//		//chunk->m_ShouldBeRemoved = true;
	//		//std::cout << "removing chunk " << chunkPosition.x << ", " << chunkPosition.y << "\n";
	//		//World::RemoveChunk(chunk);
	//	}

	//	// Check all the chunks in a box around the player
	//	/*for (int x = m_Position.x - renderDistance * Chunk::Width; x < m_Position.x + renderDistance * Chunk::Width; x += Chunk::Width)
	//	{
	//		for (int z = m_Position.z - renderDistance * Chunk::Depth; z < m_Position.z + renderDistance * Chunk::Depth; z += Chunk::Depth)
	//		{
	//			glm::ivec2 chunkPosition = Utils::WorldPositionToChunkPosition(glm::vec3(x, 0, z) + 8.0f);
	//		}
	//	}

	//	glm::ivec2 chunkPosition = chunk->GetWorldPosition() + Chunk::Width / 2;

	//	glm::ivec2 delta = chunkPosition - glm::ivec2(m_Position.x, m_Position.z);
	//	int distanceToChunk = std::abs(delta.x) + std::abs(delta.y);

	//	if (distanceToChunk > renderDistance * 2 * Chunk::Width)
	//	{
	//		World::RemoveChunk(chunk);
	//	}*/
	//}
}

Chunk* IWorld::CreateEmptyChunk(glm::ivec2 position, IWorld* world)
{
	std::lock_guard<std::recursive_mutex> lk(m_ChunkMutex);

	m_Chunks[position] = new Chunk(position, world);

	return GetChunkAt(position);
}

Chunk* IWorld::CreateChunk(glm::ivec2 position, IWorld* world)
{
	OPTICK_EVENT();

	std::lock_guard<std::recursive_mutex> lk(m_ChunkMutex);

	Chunk* chunk = new Chunk(position, world);
	chunk->Init();

	m_Chunks[position] = chunk;

	return chunk;
}

Chunk* IWorld::GenerateNewChunkThreaded(glm::ivec2 position, IWorld* world, ChunkAction* nextAction)
{
	std::unique_lock<std::recursive_mutex> lk(m_ChunkMutex);

	Chunk* chunk = new Chunk(position, world);
	m_Chunks[position] = chunk;

	lk.unlock();

	chunk->CreateAndGenerateTerrain(ChunkAction::Priority::High, nextAction);

	return chunk;
}

Chunk* IWorld::GetNewChunkNetThreaded(glm::ivec2 position, IWorld* world, ChunkAction* nextAction)
{
	std::unique_lock<std::recursive_mutex> lk(m_ChunkMutex);

	Chunk* chunk = new Chunk(position, world);
	m_Chunks[position] = chunk;
	chunk->Init();

	lk.unlock();

	chunk->GetDataNet();

	return chunk;
}
//
//Chunk* IWorld::LoadChunk(glm::ivec2 position)
//{
//	ChunkFile chunkData = ChunkIO::ReadFile("test.txt");
//
//	Chunk* chunk = CreateEmptyChunk(position);
//
//	// Iterate through all blocks
//	for (int x = 0; x < Chunk::Width; x++)
//	{
//		for (int y = 0; y < Chunk::Height; y++)
//		{
//			for (int z = 0; z < Chunk::Depth; z++)
//			{
//				ChunkBlock* block = chunk->GetBlockAt(glm::vec3(x, y, z));
//
//				block->m_BlockId = (int)chunkData.blocks[x][y][z].m_BlockId;
//			}
//		}
//	}
//
//	chunk->m_HasGenerated = true;
//	chunk->m_ShouldRebuild = true;
//
//	ChunkAction* nextAction = nullptr;/* new ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, chunk);
//	nextAction->type = ChunkAction::ActionType::RebuildAdjacentChunks;
//	nextAction->chunk = chunk;
//	nextAction->SetTimestamp();*/
//
//	chunk->RebuildMeshThreaded(ChunkAction::Priority::High, nextAction);
//
//	return chunk;
//}

void IWorld::RemoveChunk(Chunk* chunk)
{
	RemoveChunk(chunk->GetPosition());
}

void IWorld::RemoveChunk(glm::ivec2 position)
{
	m_Chunks[position]->~Chunk(); // Call the destructor to free up memory

	m_Chunks.erase(position);
}

void IWorld::RemoveChunk(glm::ivec3 position)
{
	RemoveChunk(glm::ivec2(position.x, position.z));
}

ChunkMap& IWorld::GetChunks() { return m_Chunks; }

Chunk* IWorld::GetChunkAt(glm::ivec2 position)
{
	std::lock_guard<std::recursive_mutex> lk(m_ChunkMutex);

	if (m_Chunks.count(position) == 1)
		return m_Chunks.at(position);

	return nullptr;
}

Chunk* IWorld::GetChunkAt(glm::ivec3 position) { return GetChunkAt(glm::ivec2(position.x, position.z)); }

void IWorld::SetChunkAt(Chunk* chunk)
{
	std::lock_guard<std::recursive_mutex> lk(m_ChunkMutex);

	m_Chunks[chunk->GetPosition()] = chunk;
}

bool IWorld::ChunkExistsAt(glm::ivec2 position) 
{ 
	return m_Chunks.count(position) == 1; 
}

bool IWorld::ChunkExistsAt(glm::vec3 position) { return ChunkExistsAt(glm::ivec2(position.x, position.z)); }

ChunkBlock* IWorld::GetBlockAt(glm::ivec3 position)
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
ChunkBlock* IWorld::GetBlockAt(glm::vec3 position)
{
	return GetBlockAt((glm::ivec3) glm::floor(position));
}

void IWorld::AddPlayer(IPlayer* player)
{
	std::lock_guard<std::mutex> lk(m_PlayersMutex);

	// Player already exists
	if (m_Players.count(player->m_NetClient->m_Id) == 1)
		std::cout << "Error: Player already exists!\n";

	m_Players[player->m_NetClient->m_Id] = player;
}

IPlayer* IWorld::GetPlayer(int id)
{
	std::lock_guard<std::mutex> lk(m_PlayersMutex);

	if (m_Players.count(id) == 1)
		return m_Players[id];

	return nullptr;
}

PlayersMap& IWorld::GetPlayers()
{
	return m_Players;
}

void IWorld::RemovePlayer(int id)
{
	std::lock_guard<std::mutex> lk(m_PlayersMutex);

	// Player doesn't exist
	if (m_Players.count(id) == 0)
		std::cout << "Error: Player doesnt exist!\n";

	m_Players.erase(id);
}

IWorld::~IWorld()
{
	std::cout << "World destroy\n";
}


int Settings::RenderDistance = 2;