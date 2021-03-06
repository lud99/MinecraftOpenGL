#pragma once

#include <string>
#include <cstdint>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <Graphics/Textures/Texture2D.h>

#include "BlockIds.h"

class Chunk;
class ChunkBlock;
class DroppedItem;

struct BlockFace
{
	const glm::vec3* positions;

	int direction;

	int textureId;
};

struct BlockType
{
	const std::string name;
	int id = BlockIds::Air;
	bool isTranslucent = false; // For example water. Has opacity but not no parts with 0 opacity
	bool isTransparent = false; // For example leaves. Has some parts which are fully see-through
	bool isOpaque = true; // Solid blocks. Neither transparent or translucent
	bool isModel = false; // For blocks that need custom vertices

	float friction = 0.25f;

	BlockFace faces[6];

	static constexpr int facesCount = 6;

	void SetTexture(int textureId);
};

namespace BlockTypes {
	extern BlockType Blocks[BlockIds::Count];

	void CreateBlocks();
}

// glm::u8vec3 position, uint8_t blockId, uint16_t blockData
class Block
{
public:
	Block(Chunk* chunk, ChunkBlock* chunkBlock);

	virtual bool OnBlockClick();
	virtual bool OnBlockLeftClick();
	virtual bool OnBlockRightClick();
	virtual bool OnBlockShouldBreak();
	virtual void Break();

public:
	Chunk* m_Chunk = nullptr;
	ChunkBlock* m_ChunkBlock = nullptr;

	glm::u8vec3 m_Position = glm::u8vec3(0, 0, 0);
	uint8_t m_BlockId = BlockIds::Air;
	uint16_t m_BlockData = 0;
};

namespace Blocks
{
	class StoneBlock : public Block
	{
	public:
		using Block::Block; // Inherit constructor

		bool OnBlockClick();
	};

	class DirtBlock : public Block
	{
	public:
		using Block::Block; // Inherit constructor

		bool OnBlockClick();
	};

	class NoteBlock : public Block
	{
	public:
		using Block::Block; // Inherit constructor

		bool OnBlockLeftClick();
		bool OnBlockRightClick();
		bool OnBlockShouldBreak();
	};
};