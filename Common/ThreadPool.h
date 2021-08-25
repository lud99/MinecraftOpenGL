#pragma once

#include <thread>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <deque>
#include <condition_variable>
#include <chrono>

#include <Common/Noise/FastNoise.h>

class Chunk;
class ChunkBuilder;
struct ChunkAction;

typedef struct ChunkAction ChunkAction;

typedef void(*OnChunkActionDone)(ChunkAction, void*);

struct ChunkAction
{
	enum class ActionType {
		Create,
		CreateGenerateAndBuild,
		Rebuild,
		RebuildAdjacentChunks,
		Generate,
		Serialize,
		UnserializeAndApply,
		SendChunk,
		Save
	};

	enum class Priority {
		VeryLow = 0,
		Low = 2,
		Normal = 5,
		High = 8,
		VeryHigh = 10,
	};

	ChunkAction() {};
	ChunkAction(int null) { isNull = null == NULL; };
	ChunkAction(ActionType type, Chunk* chunk, Priority priority = Priority::Normal, ChunkAction* nextAction = nullptr) :
		type(type), chunk(chunk), priority(priority), nextAction(nextAction) { SetTimestamp(); };

	ChunkAction(ActionType type, Chunk* chunk, int priority = (int)Priority::Normal, ChunkAction* nextAction = nullptr) :
		type(type), chunk(chunk), priority((Priority)priority), nextAction(nextAction) { SetTimestamp(); };

	void SetTimestamp()
	{
		using namespace std::chrono;

		timestamp = duration_cast<std::chrono::microseconds>(system_clock::now().time_since_epoch()).count();
	}

	bool isNull = false;
	ActionType type = ActionType::Rebuild;
	unsigned long long timestamp = 0;
	Priority priority = Priority::Normal;

	Chunk* chunk = nullptr;
	ChunkAction* nextAction = nullptr;

	void* extraData = nullptr;

	std::string stringData;

	OnChunkActionDone runWhenDone;
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
	std::unordered_map<int, std::deque<ChunkAction>> m_WorkQueue;

	// This will be set to true when the thread pool is shutting down. This tells
	// the threads to stop looping and finish
	bool m_Done = false;

	// Function used by the threads to grab work from the queue
	void DoWork();
	ChunkAction GetActionToDo();
};