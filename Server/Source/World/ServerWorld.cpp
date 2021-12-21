#include "ServerWorld.h"

#include <iostream>

#include <glm/vec3.hpp>

#include <Common/Chunk/Chunk.h>

#include <Common/Utils/Utils.h>
#include <Common/Blocks/Blocks.h>

#include "../Net/ServerNetworkThread.h"
#include <mutex>
#include <GLFW/glfw3.h>
#include <irrklang/irrKlang.h>

void ServerWorld::OnInit()
{
	if (m_IsInitialized) return;

	int chunkCount = (Settings::RenderDistance * 2) * (Settings::RenderDistance * 2);
	std::cout << "Using render distance " << Settings::RenderDistance << ". Total chunks: " << chunkCount << "\n";


	//	TextureAtlas::Get().Load();
	BlockTypes::CreateBlocks();

	//m_Player.Init();
	//m_Player.SetWindow(window);

	m_IsInitialized = true;
}

void ServerWorld::OnUpdate()
{

}

void ServerWorld::OnFixedUpdate()
{

}

void ServerWorld::OnTickUpdate()
{

}

void ServerWorld::HandleCreatingNewChunks()
{
	// TODO: m_ViewedByPlayers, remember to remove when player unloads it
	using namespace Settings;

	for (auto& entry : m_Players)
	{
		ServerPlayer* player = (ServerPlayer*)entry.second;

		if (!player) continue;
		if (!player->m_NetClient->m_HasJoinedSession) continue;
		if (!player->m_ConfirmedJoiningWorld) continue;

		glm::ivec3 position = glm::floor(player->m_Position);

		// Check all the chunks in a box around the player. Create new ones when necessary
		for (int x = position.x - RenderDistance * Chunk::Width; x < position.x + RenderDistance * Chunk::Width; x += Chunk::Width)
		{
			for (int z = position.z - RenderDistance * Chunk::Depth; z < position.z + RenderDistance * Chunk::Depth; z += Chunk::Depth)
			{
				int clientId = player->m_NetClient->m_Id;
				glm::ivec2 chunkPos = Utils::WorldPositionToChunkPosition(glm::vec3(x, 0, z) + 8.0f);
				Chunk* chunk = GetChunkAt(chunkPos);

				// Create the chunk here if a chunk at this position doesn't exist
				if (!chunk) 
				{
					chunk = CreateChunk(chunkPos, (IWorld*)this);

					ChunkAction* nextAction = new ChunkAction(ChunkAction::ActionType::SendChunk, chunk, ChunkAction::Priority::High);
					nextAction->extraData = player;

					chunk->CreateAndGenerateTerrain(ChunkAction::Priority::High, nextAction);
				}
				else
				{
					// If the chunk has not been viewed by the player
					if (chunk->m_ViewedByPlayers.count(clientId) == 0)
					{
						ChunkAction action(ChunkAction::ActionType::SendChunk, chunk, ChunkAction::Priority::High);
						action.extraData = player;

						ChunkBuilder::Get().AddToQueue(action);
					}
				}

				chunk->m_ViewedByPlayers.insert(clientId);
			}
		}
	}
}