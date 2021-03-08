#pragma once

#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>

#include "../Noise/FastNoise.h"

class Chunk;
class ChunkBuilder;

typedef struct ChunkAction ChunkAction;

struct ChunkAction
{
	enum class ActionType {
		Create,
		CreateGenerateAndBuild,
		Rebuild,
		RebuildAdjacentChunks,
		Generate,
		Save
	};

	ChunkAction() {};
	ChunkAction(ActionType type, Chunk* chunk, ChunkAction* nextAction = nullptr) : 
		type(type), chunk(chunk), nextAction(nextAction) { SetTimestamp(); };

	void SetTimestamp()
	{
		using namespace std::chrono;

		timestamp = duration_cast<std::chrono::microseconds>(system_clock::now().time_since_epoch()).count();
	}

	ActionType type = ActionType::Rebuild;
	unsigned long long timestamp = 0;

	Chunk* chunk = nullptr;
	ChunkAction* nextAction = nullptr;
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