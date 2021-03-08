#include "Blocks.h"

#include <iostream>

#include "../World/World.h"
#include "../World/Chunk/Chunk.h"
#include "../World/Chunk/ChunkBlock.h"
#include "../World/DroppedItem.h"

void BlockType::SetTexture(int textureId)
{
	for (int i = 0; i < facesCount; i++)
		faces[i].textureId = textureId;
}

Block::Block(Chunk* chunk, ChunkBlock* chunkBlock)
{
	m_Chunk = chunk;
	m_ChunkBlock = chunkBlock;

	m_Position = chunkBlock->GetLocalPosition();
	m_BlockId = chunkBlock->m_BlockId;
	m_BlockData = chunkBlock->m_BlockData;
}

void Block::Break()
{
	if (m_BlockId == BlockIds::Air) return;

	m_ChunkBlock->m_BlockId = BlockIds::Air;

	m_Chunk->RebuildMeshThreaded();

	// Rebuild the adjacent chunk if it exists
	glm::ivec2 chunkPosition = m_Chunk->GetPosition();
	std::vector<Chunk*> adjacentChunks;

	// Logic to get the chunk at edge depending on the block position
	if (m_Position.x == 0)
		adjacentChunks.push_back(World::GetChunkAt(chunkPosition + glm::ivec2(-1, 0)));
	if (m_Position.x == Chunk::Width - 1)
		adjacentChunks.push_back(World::GetChunkAt(chunkPosition + glm::ivec2(1, 0)));
	if (m_Position.z == 0)
		adjacentChunks.push_back(World::GetChunkAt(chunkPosition + glm::ivec2(0, -1)));
	if (m_Position.z == Chunk::Depth - 1)
		adjacentChunks.push_back(World::GetChunkAt(chunkPosition + glm::ivec2(0, 1)));

	// Add each of the adjacent chunks to the rebuild queue
	for (unsigned int i = 0; i < adjacentChunks.size(); i++)
		adjacentChunks[i]->RebuildMeshThreaded();
}

bool Blocks::StoneBlock::OnBlockClick(int button, int action, int mods)
{
	std::cout << "Stone block clicked! " << (int)m_Position.x << " " << (int)m_Position.z << "\n";

	DroppedItem* item = new DroppedItem();
	item->m_Position = m_ChunkBlock->GetWorldPosition(m_Chunk);

	m_Chunk->m_DroppedItems.push_back(item);
	//m_Chunk->m_DroppedItems[m_Chunk->m_DroppedItems.size() - 1].m_Position = m_ChunkBlock->GetWorldPosition(m_Chunk);

	return true;
}

bool Blocks::DirtBlock::OnBlockClick(int button, int action, int mods)
{
	std::cout << "Dirt block clicked!\n";

	return true;
}