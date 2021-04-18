#include "ThreadPool.h"

#include <iostream>

#include "../World/Chunk/Chunk.h"
#include "../World/Chunk/ChunkBlock.h"
#include "../World/Chunk/ChunkBuilder.h"

ThreadPool::ThreadPool()
{
	// This returns the number of threads supported by the system. If the
	// function can't figure out this information, it returns 0. 0 is not good,
	// so we create at least 1
	unsigned int numberOfThreads = std::thread::hardware_concurrency() - 1; // Don't use the main rendering thread
	if (numberOfThreads == 0) {
		numberOfThreads = 1;
	}

	for (unsigned int i = 0; i < numberOfThreads; i++) {
		// The threads will execute the private member `doWork`. Note that we need
		// to pass a reference to the function (namespaced with the class name) as
		// the first argument, and the current object as second argument
		m_Threads.push_back(std::thread(&ThreadPool::DoWork, this));
	}
}

// The destructor joins all the threads so the program can exit gracefully.
// This will be executed if there is any exception (e.g. creating the threads)

ThreadPool::~ThreadPool()
{
	// So threads know it's time to shut down
	m_Done = true;

	// Wake up all the threads, so they can finish and be joined
	m_WorkQueueConditionVariable.notify_all();
	for (auto& thread : m_Threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}
}

// This function will be called by the server every time there is a request
// that needs to be processed by the thread pool

void ThreadPool::QueueWork(ChunkAction action)
{
	// Grab the mutex
	std::lock_guard<std::mutex> g(m_WorkQueueMutex); // Auto releases when going out of scope

	// Add the action to the correct priority queue
	m_WorkQueue[(int)action.priority].push_back(action);

	//std::cout << "Entry priority: " << (int)action.priority << "\n";
	
	// Iterate through all the priority entries

	// Very low
	//if (m_WorkQueue.count(ChunkAction::Priority::VeryLow) > 0)
	//{
	//	std::cout << "Adding"
	//}

	//for (unsigned int i = 0; i < m_WorkQueue.size(); i++)
	//{
	//	// Check if multiple queued chunk actions are trying to do the same thing to the same chunk
	//	if (m_WorkQueue[i].type == action.type && m_WorkQueue[i].chunk == action.chunk)
	//	{
	//		// If so, then remove the older one from the queue
	//		if (action.timestamp > m_WorkQueue[i].timestamp) // The entry is older than the action specified
	//		{
	//			//std::cout << "Removing queue entry " << m_WorkQueue[i].timestamp << "\n";

	//			m_WorkQueue.erase(m_WorkQueue.begin() + i);
	//			
	//			break;
	//		}
	//	}
	//}

	// Push the request to the queue
	//m_WorkQueue.push_back(action);

	// Notify one thread that there are requests to process
	m_WorkQueueConditionVariable.notify_one();
}

// Function used by the threads to grab work from the queue

ChunkAction ThreadPool::GetActionToDo()
{
	typedef ChunkAction::Priority Priority;

	if (m_WorkQueue.empty())
		return ChunkAction(NULL);

	// Prioritise work with a high priority
	for (int priority = (int)Priority::VeryHigh; priority >= (int)Priority::VeryLow; priority--)
	{
		// No actions with that priority exists
		if (m_WorkQueue.count(priority) == 0)
			continue;

		if (!m_WorkQueue[priority].empty())
		{
			ChunkAction action = m_WorkQueue[priority].front();
			m_WorkQueue[priority].pop_front();

			// Remove the vector for the priority if this was the last action
			if (m_WorkQueue[priority].empty())
				m_WorkQueue.erase(priority);

			return action;
		}
	}

	// Clear the queue if nothing was found. This is to make sure it is empty to avoid infinite loops
	m_WorkQueue.clear();

	return ChunkAction(NULL);
}

void ThreadPool::DoWork()
{
	// Loop while the queue is not destructing
	while (!m_Done) {
		ChunkAction action;

		// Do stuff that requires locking so that it wont block the other threads for longer than necessary
		{
			std::unique_lock<std::mutex> g(m_WorkQueueMutex);
			m_WorkQueueConditionVariable.wait(g, [&] {
				// Only wake up if there are elements in the queue or the program is
				// shutting down
				return !m_WorkQueue.empty() || m_Done;
			});

			// If we are shutting down, then exit witout trying to process more work
			if (m_Done) break;

			action = GetActionToDo();

			if (action.isNull)
				continue;

			//for (int priority = (int)Priority::VeryLow; priority < (int)Priority::VeryLow; priority--)
			//{

			//std::cout << "Threadpool queue length: " << m_WorkQueue.size() << "\n";

			int sum = 0;
			for (int priority = (int)ChunkAction::Priority::Low; priority < (int)ChunkAction::Priority::VeryHigh; priority++)
			{
				// No actions with that priority exists
				if (m_WorkQueue.count(priority) == 0)
					continue;

				if (!m_WorkQueue[priority].empty())
					sum += m_WorkQueue[priority].size();
			}

			std::cout << sum << "; " << "\n";
		};

		// Perform the requested action
		switch (action.type)
		{
		case ChunkAction::ActionType::Create:
			action.chunk->Init();

			break;
		case ChunkAction::ActionType::CreateGenerateAndBuild:
			action.chunk->Init();
			action.chunk->GenerateTerrain();

			QueueWork(ChunkAction(ChunkAction::ActionType::Rebuild, action.chunk, ChunkAction::Priority::Low));
			QueueWork(ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, action.chunk, ChunkAction::Priority::Low));

			break;
		case ChunkAction::ActionType::Rebuild:
			
			action.chunk->RebuildMesh();

			//QueueWork(ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, action.chunk, ChunkAction::Priority::Low));

			break;

		case ChunkAction::ActionType::RebuildAdjacentChunks: {
			AdjacentChunks chunks = action.chunk->GetAdjacentChunks();

			/*Chunk* westChunkEast = chunks.West ? chunks.West->m_AdjacentChunksWhenLastRebuilt.East : nullptr;
			Chunk* eastChunkWest = chunks.East ? chunks.East->m_AdjacentChunksWhenLastRebuilt.West : nullptr;
			Chunk* northChunkSouth = chunks.North ? chunks.North->m_AdjacentChunksWhenLastRebuilt.South : nullptr;
			Chunk* southChunkNorth = chunks.South ? chunks.South->m_AdjacentChunksWhenLastRebuilt.North : nullptr;*/

			// Check that the chunk exists before adding it to the work queue
			if (chunks.West && chunks.West->m_HasGenerated)// && (!westChunkEast || !westChunkEast->m_HasGenerated))
				chunks.West->RebuildMeshThreaded(ChunkAction::Priority::Low);
			if (chunks.East && chunks.East->m_HasGenerated)// && (!eastChunkWest || !eastChunkWest->m_HasGenerated))
				chunks.East->RebuildMeshThreaded(ChunkAction::Priority::Low);
			if (chunks.North && chunks.North->m_HasGenerated)// && (!northChunkSouth || !northChunkSouth->m_HasGenerated))
				chunks.North->RebuildMeshThreaded(ChunkAction::Priority::Low);
			if (chunks.South && chunks.South->m_HasGenerated)//&& (!southChunkNorth || !southChunkNorth->m_HasGenerated))
				chunks.South->RebuildMeshThreaded(ChunkAction::Priority::Low);

			break;
		}

		case ChunkAction::ActionType::Generate:
			action.chunk->GenerateTerrain();

			break;

		default:
			m_WorkQueue.clear();

			break;
		}

		// Check if there's a next action specified
		if (action.nextAction)
			QueueWork(*action.nextAction);
		
		World::m_ChunkBuilder.AddToProcessedChunks(action);
	}
};