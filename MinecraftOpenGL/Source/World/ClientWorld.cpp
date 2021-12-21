#include "ClientWorld.h"

#include <iostream>
#include <optick.h>

#include <glm/vec3.hpp>

#include "WorldRenderer.h"
#include "Player/ClientPlayer.h"
#include <Common/Chunk/Chunk.h>
#include <Common/Chunk/ChunkBlock.h>
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

ClientWorld::ClientWorld()
{
	
}

void ClientWorld::OnInit()
{
	if (m_IsInitialized) return;

	int chunkCount = (Settings::RenderDistance * 2) * (Settings::RenderDistance * 2);
	std::cout << "Using render distance " << Settings::RenderDistance << ". Total chunks: " << chunkCount << "\n";

	SoundEngine = irrklang::createIrrKlangDevice();

	TextureAtlas::Get().Load();
	BlockTypes::CreateBlocks();

	//m_Player.Init();
	//m_Player.SetWindow(window);

	WorldRenderer::Get().Init();
	WorldRenderer::Get().m_World = this;

	m_LookingAtCollider.Init();

	m_IsInitialized = true;
}

void ClientWorld::OnUpdate()
{
	OPTICK_EVENT();
	// Iterate through all chunks to create the meshes, if not done. Main thread
	for (auto& entry : m_Chunks)
	{
		Chunk* chunk = entry.second;
		if (!chunk->m_ChunkMesh)
		{
			chunk->m_ChunkMesh = new ChunkMesh();
			chunk->m_ChunkMesh->m_Chunk = chunk;
		}
	}
}

void ClientWorld::OnFixedUpdate()
{

}

void ClientWorld::OnTickUpdate()
{

}

void ClientWorld::Render() { WorldRenderer::Get().Render(); }


void ClientWorld::HandleCreatingNewChunks()
{
	using namespace Settings;

	// Check for dirty chunks to rebuild
	for (const auto& entry : m_Chunks)
	{
		Chunk* chunk = entry.second;
		if (!chunk)
			continue;

		AdjacentChunks adjacentChunks = chunk->GetAdjacentChunks();

		// Rebuild the chunk if changes has occured and it's not already rebuilding. For example if a block has been broken
		if (chunk->m_IsInitialized && chunk->m_ChunkMesh && chunk->IsDirty() && !chunk->m_IsRebuilding)
		{
			chunk->m_ChunkMesh->RebuildMeshThreaded(ChunkAction::Priority::Normal);

			if (chunk->m_ChangedBlocks.empty())
				continue;

			// Otherwise send the block changes to the other clients
			json packet;
			packet["Type"] = "ChunkDataChanged";
			packet["Data"]["Position"]["X"] = chunk->GetPosition().x;
			packet["Data"]["Position"]["Z"] = chunk->GetPosition().y;

			packet["Data"]["Blocks"] = json::array();

			for (int i = 0; i < chunk->m_ChangedBlocks.size(); i++)
			{
				json block;
				block["X"] = chunk->m_ChangedBlocks[i].GetLocalPosition().x;
				block["Y"] = chunk->m_ChangedBlocks[i].GetLocalPosition().y;
				block["Z"] = chunk->m_ChangedBlocks[i].GetLocalPosition().z;
				block["BlockId"] = chunk->m_ChangedBlocks[i].m_BlockId;

				packet["Data"]["Blocks"].push_back(block);
			}


			m_LocalPlayer->m_NetClient->SendJson(packet);

			chunk->m_ChangedBlocks.clear();

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