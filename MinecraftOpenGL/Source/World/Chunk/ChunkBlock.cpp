#include "ChunkBlock.h"

#include <iostream>
#include <math.h>

#include "Chunk.h"
#include "../World.h"
#include "../../Graphics/Mesh.hpp"
#include "../../Graphics/Textures/TextureAtlas.h"

ChunkBlock::ChunkBlock()
{
}

bool ChunkBlock::ShouldAddBlockFace(Directions direction, Chunk* adjacentChunk)
{
	glm::ivec3 blockInAdjacentChunkOffset(0, 0, 0);
	glm::ivec3 offset(0, 0, 0);

	bool isAtChunkBorder = false;

	// Check if at a chunk border
	switch (direction)
	{
	case Directions::West:
		if (m_LocalPosition.x - 1 < 0)
			isAtChunkBorder = true;

		offset = glm::ivec3(-1, 0, 0);
		blockInAdjacentChunkOffset = glm::ivec3(Chunk::Width - 1, 0, 0);

		break;
	case Directions::East:
		if (m_LocalPosition.x + 1 >= Chunk::Width)
			isAtChunkBorder = true;

		offset = glm::ivec3(1, 0, 0);
		blockInAdjacentChunkOffset = glm::ivec3(-Chunk::Width + 1, 0, 0);

		break;
	case Directions::Bottom:
		if (m_LocalPosition.y - 1 < 0)
			isAtChunkBorder = true;

		offset = glm::ivec3(0, -1, 0);

		break;
	case Directions::Top:
		if (m_LocalPosition.y + 1 >= Chunk::Height)
			isAtChunkBorder = true;

		offset = glm::ivec3(0, 1, 0);

		break;
	case Directions::South:
		if (m_LocalPosition.z - 1 < 0)
			isAtChunkBorder = true;

		offset = glm::ivec3(0, 0, -1);
		blockInAdjacentChunkOffset = glm::ivec3(0, 0, Chunk::Depth - 1);

		break;
	case Directions::North:
		if (m_LocalPosition.z + 1 >= Chunk::Depth)
			isAtChunkBorder = true;

		offset = glm::ivec3(0, 0, 1);
		blockInAdjacentChunkOffset = glm::ivec3(0, 0, -Chunk::Depth + 1);

		break;
	}
	
	if (isAtChunkBorder)
	{
		// Because chunks don't stack vertically, nothing can occlude the top or bottom blocks
		if (direction == Bottom || direction == Top) return true;

		if (!adjacentChunk)
			return true;

		// Check if the block adjacent to this one (in the adjacent chunk) is enabled

		ChunkBlock* adjacentBlock = adjacentChunk->GetBlockAt(((glm::ivec3) m_LocalPosition) + blockInAdjacentChunkOffset);

		if (!adjacentBlock) return true;

		Block* adjacentBlockType = adjacentBlock->GetBlockType();

		// Return to prevent errors if the block doesn't have a valid block type
		if (!adjacentBlockType)
			return true;

		// Don't render air blocks
		if (adjacentBlock->m_BlockId == BlockIds::Air)			
			return true;

		// If the adjacent block is transparent and this block is opaque
		// This is to prevent seeing a side of for example water inside other water blocks, which you wouldn't expect
		if (adjacentBlockType->isTransparent && !GetBlockType()->isTransparent)
			return true;

		return false;
	}
	else if (m_LocalPosition.y + offset.y >= 0 && m_LocalPosition.y + offset.y <= Chunk::Height - 1) {
		
		ChunkBlock* adjacentBlock = GetBlockAtRelativePosition(offset);
		Block* adjacentBlockType = adjacentBlock->GetBlockType();

		// Return to prevent errors if the block doesn't have a valid block type
		if (!adjacentBlockType)
			return true;

		// Don't render air blocks
		if (adjacentBlock->m_BlockId == BlockIds::Air)
			return true;

		// If the adjacent block is transparent and this block is opaque
		// This is to prevent seeing a side of for example water inside other water blocks, which you wouldn't expect
		if (adjacentBlockType->isTransparent && !GetBlockType()->isTransparent)
			return true;
	}

	return false;
}
 
void ChunkBlock::AddBlockFace(BlockFace& face)
{
	Chunk* chunk = GetChunk();
	glm::vec3 worldPosition = GetWorldPosition();

	Mesh<PackedVertex>& opaqueMesh = chunk->m_TempOpaqueMesh;
	Mesh<PackedVertex>& waterMesh = chunk->m_TempWaterMesh;

	for (int i = 0; i < 4; i++)
	{
		BlockTexture& texture = World::m_TextureAtlas[face.textureId];

		glm::u8vec3 position(face.positions[i] + m_LocalPosition);

		int lightLevel = 15;

		switch (face.direction)
		{
		case Directions::Top: lightLevel = 15; break;
		case Directions::West: lightLevel = 12; break;
		case Directions::East: lightLevel = 12; break;
		case Directions::North: lightLevel = 9; break;
		case Directions::South: lightLevel = 9; break;
		case Directions::Bottom: lightLevel = 6; break;
		}
		
		BlockVertex vertex;

		vertex.position = position;
		vertex.index = i;
		vertex.texture = face.textureId;
		vertex.lightLevel = lightLevel;

		if (m_BlockId == BlockIds::Water)
			chunk->m_TempWaterMesh.AddVertex(vertex.CreatePackedVertex());
		else
			chunk->m_TempOpaqueMesh.AddVertex(vertex.CreatePackedVertex());
	}

	for (int i = 0; i < 6; i++)
	{
		if (m_BlockId == BlockIds::Water)
			waterMesh.AddIndex(BasicVertices::Cube::Indices[i] + (uint16_t) (waterMesh.GetVertices().size() - 4));
		else
			opaqueMesh.AddIndex(BasicVertices::Cube::Indices[i] + (uint16_t) (opaqueMesh.GetVertices().size() - 4));
	}
} 

void ChunkBlock::AddAllBlockFaces()
{
	if (!m_Enabled || m_BlockId == BlockIds::Air) return;

	Block* blockType = GetBlockType();

	AddBlockFace(blockType->faces[Directions::West]);
	AddBlockFace(blockType->faces[Directions::East]);
	AddBlockFace(blockType->faces[Directions::Top]);
	AddBlockFace(blockType->faces[Directions::Bottom]);
	AddBlockFace(blockType->faces[Directions::South]);
	AddBlockFace(blockType->faces[Directions::North]);
}

void ChunkBlock::AddBlockFaces()
{
	if (!m_Enabled || m_BlockId == BlockIds::Air) return;

	AdjacentChunks adjacentChunks = GetChunk()->GetAdjacentChunks();
	Block* blockType = GetBlockType();

	// Check for chunks on the x axis
	if (ShouldAddBlockFace(Directions::West, adjacentChunks.West))
		AddBlockFace(blockType->faces[Directions::West]);
	if (ShouldAddBlockFace(Directions::East, adjacentChunks.East))
		AddBlockFace(blockType->faces[Directions::East]);

	// Check for chunks on the y axis
	if (ShouldAddBlockFace(Directions::Top, adjacentChunks.Top))
		AddBlockFace(blockType->faces[Directions::Top]);
	if (ShouldAddBlockFace(Directions::Bottom, adjacentChunks.Bottom))
		AddBlockFace(blockType->faces[Directions::Bottom]);

	// Check for chunks on the z axis
	if (ShouldAddBlockFace(Directions::South, adjacentChunks.South))
		AddBlockFace(blockType->faces[Directions::South]);
	if (ShouldAddBlockFace(Directions::North, adjacentChunks.North))
		AddBlockFace(blockType->faces[Directions::North]);
}

Chunk* ChunkBlock::GetChunk()
{
	return World::GetChunkAt(m_ChunkPosition);
}

const glm::ivec3 ChunkBlock::GetWorldPosition()
{ 
	Chunk* chunk = GetChunk();

	glm::ivec2 chunkPosition = GetChunk()->GetWorldPosition();

	return (glm::ivec3) m_LocalPosition + glm::ivec3(chunkPosition.x, 0, chunkPosition.y);
}

Chunk* ChunkBlock::GetChunkAtRelativePosition(glm::i8vec2 offset)
{
	return World::GetChunkAt(m_ChunkPosition + (glm::ivec2) offset);
}

ChunkBlock* ChunkBlock::GetBlockAtRelativePosition(glm::u8vec3 offset)
{
	return GetChunk()->GetBlockAt(m_LocalPosition + offset);
}

bool ChunkBlock::ChunkExistsAtRelativePosition(glm::i8vec3 offset)
{
	return World::ChunkExistsAt(GetWorldPosition() + (glm::ivec3) offset);
}

bool ChunkBlock::BlockExistsAtRelativePosition(glm::u8vec3 offset)
{
	return GetChunk()->BlockExistsAt(m_LocalPosition + offset);
}

ChunkBlock::~ChunkBlock()
{
}
