#pragma once

#include <Common/ThreadPool.h>

class Chunk;

struct ChunkAction;

class ChunkBuilder
{
public:
	static ChunkBuilder& Get();

	void ProcessQueue();

	void AddToQueue(ChunkAction action);
	void AddToProcessedChunks(ChunkAction action);

	~ChunkBuilder();

private:
	ChunkBuilder();

private:
	std::vector<ChunkAction> m_ProcessedChunks;
	std::vector<ChunkAction> m_UpdateQueue;

	std::mutex m_UpdateQueueMutex;

	ThreadPool m_ThreadPool;
};