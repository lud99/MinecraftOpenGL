#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdint>

#include <Graphics/Textures/Texture2D.h>
#include <Graphics/Textures/TextureAtlas.h>
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

	void AddBlockFace(Chunk* chunk, BlockFace& face);
	void AddBlockFaces(Chunk* chunk);
	void AddAllBlockFaces();
	bool ShouldAddBlockFace(Chunk* chunk, Directions direction, Chunk* adjacentChunk);

	const glm::u8vec3 GetLocalPosition();
	const glm::ivec3 GetWorldPosition(Chunk* chunk);

	void SetLocalPosition(glm::u8vec3 position);

	BlockType* GetBlockType();

	/**
	Gets a block instance corresponding to the correct block type.
	Remember to deallocate when done!
	**/
	Block* GetBlock(Chunk* chunk);

	~ChunkBlock();

public:
	static const uint8_t Size = 1;

#pragma pack(push, 1) // To disable byte alignment. Could reduce performance

	uint16_t m_LocalPositionPacked = 0; // 2 bytes. pack the 3 component position into 2 bytes instead of 3 to save space

	uint8_t m_BlockId = BlockIds::Air; // 1 byte
	uint16_t m_BlockData = 0; // 2 bytes. 16 bits of block data packed into 2 bytes

#pragma pack(pop)
};