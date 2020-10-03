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
	void AddToProcessedChunks(ChunkAction action);

	~ChunkBuilder();

protected:
	std::vector<ChunkAction> m_ProcessedChunks;
	std::vector<ChunkAction> m_UpdateQueue;

	std::mutex m_UpdateQueueMutex;

	ThreadPool m_ThreadPool;
};