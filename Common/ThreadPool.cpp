#include "ThreadPool.h"

#include <iostream>
#include <optick.h>

#include <Common/json.hpp>
#include <Common/Net/INetworkThread.h>

#include <Common/Chunk/Chunk.h>
#include <Common/Chunk/ChunkBlock.h>
#include <Common/Chunk/ChunkBuilder.h>

#ifndef SERVER_BUILD
#include "../../World/Chunk/ChunkMesh.h"
#else
#include "../Net/ServerNetworkThread.h"
#endif

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

	// Iterate through all the queued work and remove any work that is older than the specified action, and is redu
	typedef ChunkAction::Priority Priority;
	for (int priority = (int)Priority::VeryLow; priority <= (int)Priority::VeryHigh; priority++)
	{
		// No actions with that priority exists
		if (m_WorkQueue.count(priority) == 0)
			continue;

		if (!m_WorkQueue[priority].empty())
		{
			for (int i = 0; i < m_WorkQueue[priority].size(); i++)
			{
				ChunkAction entry = m_WorkQueue[priority][i];

				// Check if multiple queued chunk actions are trying to do the same thing to the same chunk
				if (entry.type == action.type && entry.chunk == action.chunk)
				{
					// If so, then remove the older one from the queue
					if (action.timestamp > entry.timestamp) // The entry is older than the action specified
					{
						//std::cout << "Removing queue entry " << m_WorkQueue[i].timestamp << "\n";

						m_WorkQueue[priority].erase(m_WorkQueue[priority].begin() + i);
								
						break;
					}
				}
			}
			
		}
	}

	// Add the action to the correct priority queue
	m_WorkQueue[(int)action.priority].push_back(action);

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
	OPTICK_THREAD("ThreadPoolThread");

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

			int sum = 0;
			for (int priority = (int)ChunkAction::Priority::Low; priority < (int)ChunkAction::Priority::VeryHigh; priority++)
			{
				// No actions with that priority exists
				if (m_WorkQueue.count(priority) == 0)
					continue;

				if (!m_WorkQueue[priority].empty())
					sum += m_WorkQueue[priority].size();
			}

			std::string actionName = "";
			switch (action.type)
			{
			case ChunkAction::ActionType::Create:
				actionName = "Create";
				break;
			case ChunkAction::ActionType::CreateGenerateAndBuild:
				actionName = "CreateGenerateAndBuild";
				break;
			case ChunkAction::ActionType::Generate:
				actionName = "Generate";
				break;
			case ChunkAction::ActionType::Rebuild:
				actionName = "Rebuild";
				break;
			case ChunkAction::ActionType::RebuildAdjacentChunks:
				actionName = "RebuildAdjacentChunks";
				break;
			case ChunkAction::ActionType::Serialize:
				actionName = "Serialize";
				break;
			case ChunkAction::ActionType::UnserializeAndApply:
				actionName = "UnserializeAndApply";
				break;
			case ChunkAction::ActionType::SendChunk:
				actionName = "SendChunk";
				break;
			case ChunkAction::ActionType::Save:
				actionName = "Save";
				break;
			default:
				break;
			}

			std::cout << "Doing action: " << actionName << ", queue length: " << sum << "\n";
		};

		// Perform the requested action
		switch (action.type)
		{
		case ChunkAction::ActionType::Create:
			action.chunk->Init();

			break;
		case ChunkAction::ActionType::CreateGenerateAndBuild:
		{
			action.chunk->Init();
			action.chunk->GenerateTerrain();

#ifndef SERVER_BUILD

			QueueWork(ChunkAction(ChunkAction::ActionType::Rebuild, action.chunk, ChunkAction::Priority::Low));
			QueueWork(ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, action.chunk, ChunkAction::Priority::Low));
#else

#endif

			break;
		}
		case ChunkAction::ActionType::Rebuild:
			
#ifndef SERVER_BUILD
			action.chunk->m_ChunkMesh->RebuildMesh();
#endif
			//QueueWork(ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, action.chunk, ChunkAction::Priority::Low));

			break;

		case ChunkAction::ActionType::RebuildAdjacentChunks: {
#ifndef SERVER_BUILD
			AdjacentChunks chunks = action.chunk->GetAdjacentChunks();

			/*Chunk* westChunkEast = chunks.West ? chunks.West->m_AdjacentChunksWhenLastRebuilt.East : nullptr;
			Chunk* eastChunkWest = chunks.East ? chunks.East->m_AdjacentChunksWhenLastRebuilt.West : nullptr;
			Chunk* northChunkSouth = chunks.North ? chunks.North->m_AdjacentChunksWhenLastRebuilt.South : nullptr;
			Chunk* southChunkNorth = chunks.South ? chunks.South->m_AdjacentChunksWhenLastRebuilt.North : nullptr;*/

			// Check that the chunk exists before adding it to the work queue
			if (chunks.West && chunks.West->m_HasGenerated)// && (!westChunkEast || !westChunkEast->m_HasGenerated))
				chunks.West->m_ChunkMesh->RebuildMeshThreaded(ChunkAction::Priority::Low);
			if (chunks.East && chunks.East->m_HasGenerated)// && (!eastChunkWest || !eastChunkWest->m_HasGenerated))
				chunks.East->m_ChunkMesh->RebuildMeshThreaded(ChunkAction::Priority::Low);
			if (chunks.North && chunks.North->m_HasGenerated)// && (!northChunkSouth || !northChunkSouth->m_HasGenerated))
				chunks.North->m_ChunkMesh->RebuildMeshThreaded(ChunkAction::Priority::Low);
			if (chunks.South && chunks.South->m_HasGenerated)//&& (!southChunkNorth || !southChunkNorth->m_HasGenerated))
				chunks.South->m_ChunkMesh->RebuildMeshThreaded(ChunkAction::Priority::Low);
#endif
			break;
		}

		case ChunkAction::ActionType::Generate:
			action.chunk->GenerateTerrain();

			break;

		case ChunkAction::ActionType::Serialize:
		{
			std::string serialized = action.chunk->Serialize().dump();

			action.runWhenDone(action, (void*)serialized.c_str());

			break;
		}

		case ChunkAction::ActionType::UnserializeAndApply:
		{
			std::string serialized = action.stringData;

			// Unserialize the chunk data
			json unserializedPacket = Chunk::Unserialize(serialized);

			for (int i = 0; i < Chunk::BlockCount; i++)
			{
				glm::u8vec3 position = Utils::BlockIndexToPosition(i);
				ChunkBlock* block = action.chunk->GetBlockAt(position);
				block->m_BlockId = unserializedPacket["Data"]["Blocks"][i];
			}

			action.chunk->SetDirty(true);
			action.chunk->m_HasGenerated = true;

			break;
		}

		case ChunkAction::ActionType::SendChunk:
		{
#ifdef SERVER_BUILD
			NetworkClient* client = (NetworkClient*)action.extraData;

			json serialized = action.chunk->Serialize();

			client->SendJson(serialized);
#endif
			break;
		}

		default:
			m_WorkQueue.clear();

			break;
		}

		// Check if there's a next action specified
		if (action.nextAction)
			QueueWork(*action.nextAction);

		ChunkBuilder::Get().AddToProcessedChunks(action);
	}
};