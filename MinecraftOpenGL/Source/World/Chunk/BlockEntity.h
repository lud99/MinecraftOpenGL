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

struct BlockFaceData;

class BlockEntity
{
public:
	BlockEntity();

	void AddBlockFace(BlockFace& face);

	void AddBlockFaces();
	void AddAllBlockFaces();

	Chunk* GetChunk();

	const glm::u8vec3 GetLocalPosition();
	const glm::ivec3 GetWorldPosition();

	void SetLocalPosition(glm::u8vec3 position);

	BlockType* GetBlockType();

	~BlockEntity();

private:
	//Chunk* GetChunkAtRelativePosition(glm::i8vec2 offset);
	ChunkBlock* GetBlockAtRelativePosition(glm::u8vec3 offset);

	/*bool ChunkExistsAtRelativePosition(glm::i8vec3 offset);
	bool BlockExistsAtRelativePosition(glm::u8vec3 offset);*/

	bool ShouldAddBlockFace(Directions direction, Chunk* adjacentChunk);

public:
	static const uint8_t Size = 1;

#pragma pack(push, 1) // To disable byte alignment. Could reduce performance

	glm::ivec2 m_ChunkPosition; // 8 bytes
	uint16_t m_LocalPositionPacked = 0; // 2 bytes. pack the 3 component position into 2 bytes instead of 3 to save space

	uint8_t m_BlockId = BlockIds::Air; // 1 byte
	uint16_t m_BlockData = 0; // 2 bytes. 16 bits of block data packed into 2 bytes

#pragma pack(pop)
};