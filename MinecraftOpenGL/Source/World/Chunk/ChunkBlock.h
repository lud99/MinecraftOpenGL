#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "../../Graphics/Textures/Texture2D.h"
#include "../../Graphics/Textures/TextureAtlas.h"
#include "../World.h"
#include "../../Blocks/BlockTypes.h"

#include "../../Collider.h"

class Chunk;

struct BlockFaceData
{
	std::vector<Vertex> opaqueVertices;
	std::vector<Vertex> waterVertices;
	std::vector<unsigned int> opaqueIndices;
	std::vector<unsigned int> waterIndices;
};

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

	inline const glm::i8vec3 GetLocalPosition() { return m_LocalPosition; };
	const glm::i32vec3 GetWorldPosition();

	inline void SetLocalPosition(glm::i8vec3 position) { m_LocalPosition = position; }

	inline void SetEnabled(bool flag) { m_Enabled = flag; };
	inline bool IsEnabled() { return m_Enabled; }

	inline Block* GetBlockType() { return &BlockTypes::Blocks[m_BlockId]; }

	~ChunkBlock();

private:
	Chunk* GetChunkAtRelativePosition(glm::i8vec2 offset);
	ChunkBlock* GetBlockAtRelativePosition(glm::i8vec3 offset);

	bool ChunkExistsAtRelativePosition(glm::i8vec3 offset);
	bool BlockExistsAtRelativePosition(glm::i8vec3 offset);

	bool ShouldAddBlockFace(Directions direction, Chunk* adjacentChunk);

	AdjacentChunks GetAdjacentChunks();

public:
	glm::i32vec2 m_ChunkPosition;

	int m_BlockId = BlockIds::Air;

private:
	glm::i8vec3 m_LocalPosition;

	bool m_Enabled = true;
	bool m_Highlighted = false;
};