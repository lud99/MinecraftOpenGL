#include "ChunkBuilder.h"

#include <shared_mutex>
#include <queue>

#include "Chunk.h"
#include "ChunkBlock.h"

ChunkBuilder::ChunkBuilder() 
{ 

}

void ChunkBuilder::AddToQueue(ChunkAction action)
{
	using namespace std::chrono;

	m_UpdateQueueMutex.lock();

	action.chunkBuilder = this;
	action.timestamp = duration_cast<std::chrono::microseconds>(system_clock::now().time_since_epoch()).count();

	m_UpdateQueue.push_back(action);

	m_UpdateQueueMutex.unlock();
}

void ChunkBuilder::AddToRebuiltChunks(Chunk* chunk)
{
	m_UpdateQueueMutex.lock();

	m_RebuiltChunks.push_back(chunk->GetIndex());

	m_UpdateQueueMutex.unlock();
}

void ChunkBuilder::ProcessQueue()
{
	// Check for chunks that should be updated
	for (unsigned int i = 0; i < m_UpdateQueue.size(); i++)
	{
		m_ThreadPool.QueueWork(m_UpdateQueue[i]);
	}

	m_UpdateQueue.clear();

	// Check for chunks that have finished rebuilding
	for (unsigned int i = 0; i < m_RebuiltChunks.size(); i++)
	{
		Chunk* chunk = World::GetChunkFromIndex(m_RebuiltChunks[i]);

		chunk->m_OpaqueMesh.Update();
		chunk->m_WaterMesh.Update();
	}

	m_RebuiltChunks.clear();
}

ChunkBuilder::~ChunkBuilder() { }