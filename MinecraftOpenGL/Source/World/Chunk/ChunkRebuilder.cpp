#include "ChunkRebuilder.h"

#include "Chunk.h"
#include "ChunkBlock.h"

ChunkRebuilder::ChunkRebuilder() 
{ 
	//int coreCount = 4;

	//m_Threads = new std::thread[coreCount - 1];
}

void ChunkRebuilder::Rebuild(Chunk* chunk)
{
	auto RebuildMesh = [&](Chunk* realChunk, Chunk* chunkCopy, std::queue<int>& rebuiltChunks) {
		realChunk->m_OpaqueMesh.Clear();
		realChunk->m_WaterMesh.Clear();
		
		Mesh opaqueMesh;
		Mesh waterMesh;

		for (int x = 0; x < Chunk::Width; x++)
		{
			for (int y = 0; y < Chunk::Height; y++)
			{
				for (int z = 0; z < Chunk::Depth; z++)
				{
					ChunkBlock* block = realChunk->GetBlockAt(glm::ivec3(x, y, z));

					// Add all the faces on the cube
					block->AddBlockFaces();// &opaqueMesh, &waterMesh);
				}
			}
		}

		/*realChunk->m_OpaqueMesh.m_Vertices = opaqueMesh.m_Vertices;
		realChunk->m_OpaqueMesh.m_Indices = opaqueMesh.m_Indices;
		realChunk->m_WaterMesh.m_Vertices = waterMesh.m_Vertices;
		realChunk->m_WaterMesh.m_Indices = waterMesh.m_Indices;*/

		std::cout << "Chunk vertices updated. Thread exiting\n";

		rebuiltChunks.push(realChunk->GetIndex());
	};

	std::thread thread(RebuildMesh, chunk, new Chunk(*chunk), std::ref(m_RebuiltChunks));
	thread.detach();
}

void ChunkRebuilder::Rebuild(int chunkIndex)
{

}

void ChunkRebuilder::RebuildAll()
{

}

void ChunkRebuilder::CheckForRebuiltChunks()
{
	for (int i = 0; i < m_RebuiltChunks.size(); i++)
	{
		Chunk* chunk = World::GetChunkFromIndex(m_RebuiltChunks.front());

		std::cout << "r\n";

		chunk->m_OpaqueMesh.Update();
		chunk->m_WaterMesh.Update();

		m_RebuiltChunks.pop();
	}
}

ChunkRebuilder::~ChunkRebuilder() { }