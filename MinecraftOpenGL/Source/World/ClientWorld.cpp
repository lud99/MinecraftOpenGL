#include "ClientWorld.h"

#include <iostream>

#include <glm/vec3.hpp>

#include "WorldRenderer.h"
#include "Player/ClientPlayer.h"
#include <Common/Chunk/Chunk.h>
//#include "Chunk/ChunkIO.h"
#include <Common/Utils/Utils.h>
#include <Common/Blocks/Blocks.h>
#include <Common/Player/IPlayer.h>
#include "Collider.h"
#include "../Window.h"

#include "../World/WorldRenderer.h"
#include "../Network/ClientNetworkThread.h"
#include "../World/Chunk/ChunkMesh.h"
#include "../World/Player/ClientPlayer.h"

#include <mutex>
#include <GLFW/glfw3.h>
#include <irrklang/irrKlang.h>

void ClientWorld::OnInit()
{
	int chunkCount = (Settings::RenderDistance * 2) * (Settings::RenderDistance * 2);
	std::cout << "Using render distance " << Settings::RenderDistance << ". Total chunks: " << chunkCount << "\n";

	SoundEngine = irrklang::createIrrKlangDevice();

	TextureAtlas::Get().Load();
	BlockTypes::CreateBlocks();

	//m_Player.Init();
	//m_Player.SetWindow(window);

	WorldRenderer::Get().Init();

	m_LookingAtCollider.Init();
}

void ClientWorld::OnUpdate()
{

}

void ClientWorld::OnFixedUpdate()
{

}

void ClientWorld::OnTickUpdate()
{

}

void ClientWorld::Render() { m_Renderer->Render(); }


void ClientWorld::HandleCreatingNewChunks()
{
	using namespace Settings;

	IPlayer* player = m_Players[0];

	glm::ivec3 position = glm::floor(player->m_Position);

	// Check all the chunks in a box around the player
	//for (int x = position.x - RenderDistance * Chunk::Width; x < position.x + RenderDistance * Chunk::Width; x += Chunk::Width)
	//{
	//	for (int z = position.z - RenderDistance * Chunk::Depth; z < position.z + RenderDistance * Chunk::Depth; z += Chunk::Depth)
	//	{
	//		glm::ivec2 chunkPos = Utils::WorldPositionToChunkPosition(glm::vec3(x, 0, z) + 8.0f);
	//		Chunk* chunk = GetChunkAt(chunkPos);

	//		//ChunkAction* nextAction = new ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, chunk, ChunkAction::Priority::Low);

	//		// Create the chunk here if a chunk at this position doesn't exist
	//		if (!chunk) {
	//			//chunk = GenerateNewChunkThreaded(chunkPos);
	//			//std::cout << "newchunk\n";
	//		}
	//	}
	//}

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
				chunk->m_ChunkMesh->RebuildMeshThreaded(ChunkAction::Priority::VeryHigh);
				std::cout << "rebuild\n";
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

//Chunk* ClientWorld::GetNewChunkNetThreaded(glm::ivec2 position, ChunkAction* nextAction)
//{
//	std::unique_lock<std::recursive_mutex> lk(m_ChunkMutex);
//
//	Chunk* chunk = new Chunk(position);
//	m_Chunks[position] = chunk;
//	chunk->Init();
//
//	lk.unlock();
//
//	chunk->GetDataNet();
//
//	return chunk;
//}
//
//Chunk* ClientWorld::LoadChunk(glm::ivec2 position)
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