#pragma once

#include "../../ThreadPool.h"

class Chunk;

struct ChunkAction;

class ChunkBuilder
{
public:
	ChunkBuilder();

	void ProcessQueue();

	void AddToQueue(ChunkAction action);
	void AddToRebuiltChunks(Chunk* chunk);

	~ChunkBuilder();

protected:
	std::vector<int> m_RebuiltChunks;
	std::vector<ChunkAction> m_UpdateQueue;

	std::mutex m_UpdateQueueMutex;

	std::vector<std::thread> m_Threads;

	ThreadPool m_ThreadPool;
};