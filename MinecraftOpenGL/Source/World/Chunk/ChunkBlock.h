#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdint>

#include "../../Graphics/Textures/Texture2D.h"
#include "../../Graphics/Textures/TextureAtlas.h"
#include "../World.h"
#include "../../Blocks/BlockTypes.h"

#include "../Collider.h"

class Chunk;

struct BlockFaceData
{
	std::vector<Vertex> opaqueVertices;
	std::vector<Vertex> waterVertices;
	std::vector<unsigned int> opaqueIndices;
	std::vector<unsigned int> waterIndices;
};

class ChunkBlock
{
public:
	ChunkBlock();

	void AddBlockFace(BlockFace& face);

	void AddBlockFaces();
	void AddAllBlockFaces();

	Chunk* GetChunk();

	const glm::u8vec3 GetLocalPosition();
	const glm::ivec3 GetWorldPosition();

	void SetLocalPosition(glm::u8vec3 position);

	Block* GetBlockType();

	~ChunkBlock();

private:
	Chunk* GetChunkAtRelativePosition(glm::i8vec2 offset);
	ChunkBlock* GetBlockAtRelativePosition(glm::u8vec3 offset);

	bool ChunkExistsAtRelativePosition(glm::i8vec3 offset);
	bool BlockExistsAtRelativePosition(glm::u8vec3 offset);

	bool ShouldAddBlockFace(Directions direction, Chunk* adjacentChunk);

public:
	static const uint8_t Size = 1;

	glm::ivec2 m_ChunkPosition; // 8 bytes

	uint8_t m_BlockId = BlockIds::Air; // 1 byte

private:
	uint16_t m_LocalPositionPacked = 0; // pack the 3 component position into 2 bytes instead of 3 bytes to save space

	//bool m_Enabled = true; // 1 byte
	//bool m_Highlighted = false; // 1 byte
};