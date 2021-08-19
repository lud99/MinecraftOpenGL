#include "ChunkBuilder.h"

#include <shared_mutex>
#include <queue>

#include "Chunk.h"
#include "ChunkIO.h"
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

void ChunkBuilder::ProcessQueue()
{
	// Check for chunks that have finished processing in the threadpool
	for (unsigned int i = 0; i < m_ProcessedChunks.size(); i++)
	{
		ChunkAction action = m_ProcessedChunks[i];

		switch (action.type)
		{
		case ChunkAction::ActionType::Rebuild:
			action.chunk->m_OpaqueMesh.Update();
			action.chunk->m_WaterMesh.Update();

			//ChunkIO::WriteFile("test.txt", action.chunk);

			break;
		case ChunkAction::ActionType::CreateGenerateAndBuild:
			action.chunk->m_OpaqueMesh.Update();
			action.chunk->m_WaterMesh.Update();

			break;
		case ChunkAction::ActionType::RebuildAdjacentChunks:
			action.chunk->m_OpaqueMesh.Update();
			action.chunk->m_WaterMesh.Update();

			break;
		case ChunkAction::ActionType::Generate:

			break;
		}
	}

	m_ProcessedChunks.clear();
}

ChunkBuilder::~ChunkBuilder() { }