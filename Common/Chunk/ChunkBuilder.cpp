#include "ChunkBuilder.h"

#include <shared_mutex>
#include <queue>

#include "Chunk.h"
#ifndef SERVER_BUILD
#include "../World/Chunk/ChunkMesh.h"
#endif
//#include "ChunkIO.h"
#include "ChunkBlock.h"

ChunkBuilder::ChunkBuilder() 
{ 

}

void ChunkBuilder::AddToQueue(ChunkAction action)
{
	m_ThreadPool.QueueWork(action);
}

void ChunkBuilder::AddToProcessedChunks(ChunkAction action)
{
	m_UpdateQueueMutex.lock();

	m_ProcessedChunks.push_back(action);

	m_UpdateQueueMutex.unlock();
}

ChunkBuilder& ChunkBuilder::Get()
{
	static ChunkBuilder instance;
	return instance;
}

void ChunkBuilder::ProcessQueue()
{
	// Check for chunks that have finished processing in the threadpool
	for (unsigned int i = 0; i < m_ProcessedChunks.size(); i++)
	{
		ChunkAction action = m_ProcessedChunks[i];

		switch (action.type)
		{
		case ChunkAction::ActionType::Rebuild:
#ifndef SERVER_BUILD
			action.chunk->m_ChunkMesh->m_OpaqueMesh.Update();
			action.chunk->m_ChunkMesh->m_WaterMesh.Update();
#endif

			break;
		case ChunkAction::ActionType::CreateGenerateAndBuild:
#ifndef SERVER_BUILD
			action.chunk->m_ChunkMesh->m_OpaqueMesh.Update();
			action.chunk->m_ChunkMesh->m_WaterMesh.Update();
#endif

			break;
		case ChunkAction::ActionType::RebuildAdjacentChunks:
#ifndef SERVER_BUILD
			action.chunk->m_ChunkMesh->m_OpaqueMesh.Update();
			action.chunk->m_ChunkMesh->m_WaterMesh.Update();
#endif

			break;
		case ChunkAction::ActionType::Generate:

			break;
		}
	}

	m_ProcessedChunks.clear();
}

ChunkBuilder::~ChunkBuilder() { }