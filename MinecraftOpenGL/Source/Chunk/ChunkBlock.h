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
	void AddBlockFaces(const glm::vec4* colors);
	void AddAllBlockFaces(const glm::vec4* colors);

	Chunk* GetChunk();

	const glm::vec3 GetLocalPosition();
	const glm::vec3 GetWorldPosition();

	void SetLocalPosition(glm::vec3 position);
	void SetWorldPosition(glm::vec3 position);

	void SetEnabled(bool flag);
	bool GetEnabled();

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

	Chunk* GetChunkAtRelativePosition(glm::vec3 offset);
	ChunkBlock* GetBlockAtRelativePosition(glm::vec3 offset);

	bool ChunkExistsAtRelativePosition(glm::vec3 offset);
	bool BlockExistsAtRelativePosition(glm::vec3 offset);

	bool ShouldAddBlockFace(ChunkBorderEdges direction, Chunk* adjacentChunk, glm::vec3 offset);

	AdjacentChunks GetAdjacentChunks();

public:
	unsigned int m_ChunkIndex = 0;

	Block* m_BlockType;

private:
	glm::vec3 m_LocalPosition;

	bool m_Enabled = false;
};