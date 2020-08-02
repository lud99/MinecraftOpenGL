#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "../Textures/Texture2D.h"
#include "../Textures/TextureList.h"
#include "../Blocks/BlockTypes.h"

class Chunk;

struct AdjacentChunks
{
	AdjacentChunks();

	Chunk* Left = nullptr;
	Chunk* Right = nullptr;
	Chunk* Top = nullptr;
	Chunk* Bottom = nullptr;
	Chunk* Back = nullptr;
	Chunk* Front = nullptr;
};

class ChunkBlock
{
public:
	ChunkBlock();

	void AddBlockFace(BlockFace& face);
	void AddBlockFaces();
	void AddAllBlockFaces();

	Chunk* GetChunk();

	const glm::ivec3 GetLocalPosition();
	const glm::ivec3 GetWorldPosition();

	void SetLocalPosition(glm::ivec3 position);
	void SetWorldPosition(glm::ivec3 position);

	void SetEnabled(bool flag);
	bool GetEnabled();

	Block* GetBlockType();

	~ChunkBlock();

private:
	enum ChunkBorderEdges {
		None = -1,
		Left = 0,
		Right = 1,
		Bottom = 2,
		Top = 3,
		Back = 4,
		Front = 5
	};

	Chunk* GetChunkAtRelativePosition(glm::ivec2 offset);
	ChunkBlock* GetBlockAtRelativePosition(glm::ivec3 offset);

	bool ChunkExistsAtRelativePosition(glm::ivec3 offset);
	bool BlockExistsAtRelativePosition(glm::ivec3 offset, bool includeTransparentBlocks = false);

	bool ShouldAddBlockFace(ChunkBorderEdges direction, Chunk* adjacentChunk);

	AdjacentChunks GetAdjacentChunks();

public:
	int m_ChunkIndex = 0;

	int m_BlockId = BlockIds::Air;

private:
	glm::ivec3 m_LocalPosition;

	bool m_Enabled = false;
};