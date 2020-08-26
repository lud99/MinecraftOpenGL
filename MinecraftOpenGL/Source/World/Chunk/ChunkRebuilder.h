#pragma once

#include <thread>
#include <queue>

class Chunk;

class ChunkRebuilder
{
public:
	ChunkRebuilder();

	void Rebuild(int chunkIndex);
	void Rebuild(Chunk* chunk);
	void RebuildAll();

	void CheckForRebuiltChunks();

	~ChunkRebuilder();
private:
	std::queue<int> m_RebuiltChunks;
	//std::vector<std::thread> m_ActiveThreads;
};