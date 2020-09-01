#pragma once

#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>

#include "Noise/FastNoise.h"

class Chunk;
class ChunkBuilder;

struct ChunkAction
{
	enum ActionType {
		Rebuild,
		Generate,
		Save
	};

	ChunkAction() {};
	ChunkAction(ActionType type, Chunk* chunk) :
		type(type), chunk(chunk) {};

	ActionType type = Rebuild;
	unsigned long long timestamp;

	Chunk* chunk;
	ChunkBuilder* chunkBuilder;
};

class ThreadPool {
public:
	ThreadPool();

	void QueueWork(ChunkAction action);

	~ThreadPool();

private:
	// This condition variable is used for the threads to wait until there is work to do
	std::condition_variable_any m_WorkQueueConditionVariable;

	// We store the threads in a vector, so we can later stop them gracefully
	std::vector<std::thread> m_Threads;

	// Mutex to protect workQueue
	std::mutex m_WorkQueueMutex;

	// Queue of requests waiting to be processed
	std::deque<ChunkAction> m_WorkQueue;

	// This will be set to true when the thread pool is shutting down. This tells
	// the threads to stop looping and finish
	bool m_Done = false;

	// Function used by the threads to grab work from the queue
	void DoWork();
};