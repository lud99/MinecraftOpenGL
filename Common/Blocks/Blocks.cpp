#include "Blocks.h"

#include <iostream>
#include <math.h>

//#include <Graphics/Textures/Texture2D.h>
//#include <Graphics/Mesh.hpp>
#include <Common/Graphics/BasicVertices.h>

#include <Common/World/IWorld.h>
#include <Common/Chunk/Chunk.h>
#include <Common/Chunk/ChunkBlock.h>
//#include "../World/DroppedItem.h"

#ifndef SERVER_BUILD
#include <irrklang/irrKlang.h>
#endif

void BlockType::SetTexture(int textureId)
{
	for (int i = 0; i < facesCount; i++)
		faces[i].textureId = textureId;
}

void BlockTypes::CreateBlocks()
{
	using namespace BasicVertices;
	using namespace BlockIds;

	for (int i = 0; i < Count; i++) {
		Blocks[i].id = i;

		for (int j = 0; j < BlockType::facesCount; j++) {
			Blocks[i].faces[j].positions = Cube::GetFace(j);
			Blocks[i].faces[j].direction = j;
		}
	}

	Blocks[Air].isTransparent = true;
	Blocks[Air].isOpaque = false;

	// Set all sides to the same texture as the default

	// Dirt
	Blocks[Dirt].SetTexture(TextureIds::Dirt);
	// Stone
	Blocks[Stone].SetTexture(TextureIds::Stone);
	// Water
	Blocks[Water].SetTexture(TextureIds::Water);
	Blocks[Water].isTranslucent = true;
	Blocks[Water].isOpaque = false;
	// Gravel
	Blocks[Gravel].SetTexture(TextureIds::Gravel);
	// Sand
	Blocks[Sand].SetTexture(TextureIds::Sand);
	// Chest
	Blocks[Chest].SetTexture(TextureIds::Chest);
	// Noteblock
	Blocks[Noteblock].SetTexture(TextureIds::Noteblock);

	Blocks[Ice].SetTexture(TextureIds::Ice);
	Blocks[Ice].isTranslucent = true;
	Blocks[Ice].isOpaque = false;
	Blocks[Ice].friction = 0.01f;

	// Set all the textures for the oak log
	Blocks[OakLog].faces[Cube::Faces::Left].textureId = TextureIds::OakLogSide;
	Blocks[OakLog].faces[Cube::Faces::Right].textureId = TextureIds::OakLogSide;
	Blocks[OakLog].faces[Cube::Faces::Bottom].textureId = TextureIds::OakLogTop;
	Blocks[OakLog].faces[Cube::Faces::Top].textureId = TextureIds::OakLogTop;
	Blocks[OakLog].faces[Cube::Faces::Front].textureId = TextureIds::OakLogSide;
	Blocks[OakLog].faces[Cube::Faces::Back].textureId = TextureIds::OakLogSide;

	// Oak leaves
	Blocks[OakLeaves].SetTexture(TextureIds::OakLeaves);
	Blocks[OakLeaves].isTransparent = true;
	Blocks[OakLeaves].isOpaque = false;

	// Door
	Blocks[DoorBottom].isTranslucent = true;
	Blocks[DoorBottom].isModel = true;
	Blocks[DoorBottom].modelPath = "Resources/Models/Door/door-bottom.obj";
	Blocks[DoorTop].isTranslucent = true;
	Blocks[DoorTop].isModel = true;
	Blocks[DoorTop].modelPath = "Resources/Models/Door/door-top.obj";

	// Set all the textures for the grass block
	Blocks[Grass].faces[Cube::Faces::Left].textureId = TextureIds::GrassSide;
	Blocks[Grass].faces[Cube::Faces::Right].textureId = TextureIds::GrassSide;
	Blocks[Grass].faces[Cube::Faces::Bottom].textureId = TextureIds::Dirt;
	Blocks[Grass].faces[Cube::Faces::Top].textureId = TextureIds::GrassTop;
	Blocks[Grass].faces[Cube::Faces::Front].textureId = TextureIds::GrassSide;
	Blocks[Grass].faces[Cube::Faces::Back].textureId = TextureIds::GrassSide;
}

BlockType BlockTypes::Blocks[BlockIds::Count];

Block::Block(IWorld* world, Chunk* chunk, ChunkBlock* chunkBlock)
{
	m_World = world;
	m_Chunk = chunk;
	m_ChunkBlock = chunkBlock;

	m_Position = chunkBlock->GetLocalPosition();
	m_BlockId = chunkBlock->m_BlockId;
	m_BlockData = chunkBlock->m_BlockData;
}

void Block::Break()
{
	if (m_BlockId == BlockIds::Air) return;

	if (!m_Chunk) return;

	m_ChunkBlock->m_BlockId = BlockIds::Air;

	m_Chunk->SetDirty(true);

	// Rebuild the adjacent chunk if it exists
	glm::ivec2 chunkPosition = m_Chunk->GetPosition();
	std::vector<Chunk*> adjacentChunks;

	// TODO: check for null
	// Logic to get the chunk at edge depending on the block position
	if (m_Position.x == 0)
		adjacentChunks.push_back(m_World->GetChunkAt(chunkPosition + glm::ivec2(-1, 0)));
	if (m_Position.x == Chunk::Width - 1)
		adjacentChunks.push_back(m_World->GetChunkAt(chunkPosition + glm::ivec2(1, 0)));
	if (m_Position.z == 0)
		adjacentChunks.push_back(m_World->GetChunkAt(chunkPosition + glm::ivec2(0, -1)));
	if (m_Position.z == Chunk::Depth - 1)
		adjacentChunks.push_back(m_World->GetChunkAt(chunkPosition + glm::ivec2(0, 1)));

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

	/*DroppedItem* item = new DroppedItem();
	item->m_Position = m_ChunkBlock->GetWorldPosition(m_Chunk);

	m_Chunk->m_DroppedItems.push_back(item);*/

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

#ifndef SERVER_BUILD

	float spd = powf(2, (m_ChunkBlock->m_BlockData - maxNote / 2.0f) / 12.0f);

	/*irrklang::ISound* snd = m_World->SoundEngine->play2D("Resources/Audio/harp.wav", false, false, true);
	if (snd)
	{
		snd->setPlaybackSpeed(spd);
	}

	if (snd)
	{

		snd->drop();
		snd = 0;
	}*/
#endif

	return false;
}

bool Blocks::NoteBlock::OnBlockRightClick()
{
	int maxNote = 24;

	if (m_ChunkBlock->m_BlockData < maxNote)
		m_ChunkBlock->m_BlockData++;
	else
		m_ChunkBlock->m_BlockData = 0;

#ifndef SERVER_BUILD

	float spd = powf(2, (m_ChunkBlock->m_BlockData - maxNote / 2.0f) / 12.0f);

	//irrklang::ISound* snd = m_World->SoundEngine->play2D("Resources/Audio/harp.wav", false, false, true);
	/*if (snd)
	{
		snd->setPlaybackSpeed(spd);
	}

	if (snd)
	{
		snd->drop();
		snd = 0;
	}*/
#endif

	return false;
}

bool Blocks::NoteBlock::OnBlockShouldBreak()
{
	return false;
}