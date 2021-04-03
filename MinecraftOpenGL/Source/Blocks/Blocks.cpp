#include "Blocks.h"

#include <iostream>
#include <math.h>

#include "../World/World.h"
#include "../World/Chunk/Chunk.h"
#include "../World/Chunk/ChunkBlock.h"
#include "../World/DroppedItem.h"

#include <irrklang/irrKlang.h>

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

	m_Chunk->SetDirty(true);

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
		adjacentChunks[i]->SetDirty(true);
}

// Events
bool Block::OnBlockClick() 
{
	return true;
};

bool Block::OnBlockLeftClick()
{
	return true;
};

bool Block::OnBlockRightClick()
{
	return true;
}

bool Block::OnBlockShouldBreak()
{
	return true;
}

bool Blocks::StoneBlock::OnBlockClick()
{
	std::cout << "Stone block clicked! " << (int)m_Position.x << " " << (int)m_Position.z << "\n";

	DroppedItem* item = new DroppedItem();
	item->m_Position = m_ChunkBlock->GetWorldPosition(m_Chunk);

	m_Chunk->m_DroppedItems.push_back(item);

	return true;
}

bool Blocks::DirtBlock::OnBlockClick()
{
	std::cout << "Dirt block clicked!\n";

	return true;
}

bool Blocks::NoteBlock::OnBlockLeftClick()
{
	int maxNote = 24;

	float spd = powf(2, (m_ChunkBlock->m_BlockData - maxNote / 2.0f) / 12.0f);

	irrklang::ISound* snd = World::SoundEngine->play2D("Resources/Audio/harp.wav", false, false, true);
	if (snd)
	{
		snd->setPlaybackSpeed(spd);
	}

	if (snd)
	{
		snd->drop();
		snd = 0;
	}

	return false;
}

bool Blocks::NoteBlock::OnBlockRightClick()
{
	int maxNote = 24;

	if (m_ChunkBlock->m_BlockData < maxNote)
		m_ChunkBlock->m_BlockData++;
	else
		m_ChunkBlock->m_BlockData = 0;

	float spd = powf(2, (m_ChunkBlock->m_BlockData - maxNote / 2.0f) / 12.0f);

	irrklang::ISound* snd = World::SoundEngine->play2D("Resources/Audio/harp.wav", false, false, true);
	if (snd)
	{
		snd->setPlaybackSpeed(spd);
	}

	if (snd)
	{
		snd->drop();
		snd = 0;
	}

	return false;
}

bool Blocks::NoteBlock::OnBlockShouldBreak()
{
	return false;
}