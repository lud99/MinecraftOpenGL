#include "ChunkBlock.h"

#include <iostream>
#include <math.h>

#include "Chunk.h"
#include "../World.h"
#include "../../Blocks/BlockTypes.h"
#include <Graphics/Mesh.hpp>
#include <Graphics/Textures/TextureAtlas.h>

ChunkBlock::ChunkBlock()
{
}

bool ChunkBlock::ShouldAddBlockFace(Chunk* chunk, Directions direction, Chunk* adjacentChunk)
{
	glm::ivec3 blockInAdjacentChunkOffset(0, 0, 0);
	glm::ivec3 offset(0, 0, 0);
	glm::u8vec3 localPosition = GetLocalPosition();

	bool isAtChunkBorder = false;

	// Check if at a chunk border
	switch (direction)
	{
	case Directions::West:
		if (localPosition.x - 1 < 0)
			isAtChunkBorder = true;

		offset = glm::ivec3(-1, 0, 0);
		blockInAdjacentChunkOffset = glm::ivec3(Chunk::Width - 1, 0, 0);

		break;
	case Directions::East:
		if (localPosition.x + 1 >= Chunk::Width)
			isAtChunkBorder = true;

		offset = glm::ivec3(1, 0, 0);
		blockInAdjacentChunkOffset = glm::ivec3(-Chunk::Width + 1, 0, 0);

		break;
	case Directions::Bottom:
		if (localPosition.y - 1 < 0)
			isAtChunkBorder = true;

		offset = glm::ivec3(0, -1, 0);

		break;
	case Directions::Top:
		if (localPosition.y + 1 >= Chunk::Height)
			isAtChunkBorder = true;

		offset = glm::ivec3(0, 1, 0);

		break;
	case Directions::South:
		if (localPosition.z - 1 < 0)
			isAtChunkBorder = true;

		offset = glm::ivec3(0, 0, -1);
		blockInAdjacentChunkOffset = glm::ivec3(0, 0, Chunk::Depth - 1);

		break;
	case Directions::North:
		if (localPosition.z + 1 >= Chunk::Depth)
			isAtChunkBorder = true;

		offset = glm::ivec3(0, 0, 1);
		blockInAdjacentChunkOffset = glm::ivec3(0, 0, -Chunk::Depth + 1);

		break;
	}

	ChunkBlock* adjacentBlock = nullptr;
	BlockType* adjacentBlockType = nullptr;

	if (isAtChunkBorder)
	{
		if (!adjacentChunk || !adjacentChunk->m_IsInitialized)
			return true;

		// Check if the block adjacent to this one exists
		adjacentBlock = adjacentChunk->GetBlockAt((glm::ivec3)localPosition + blockInAdjacentChunkOffset);
		if (!adjacentBlock) return true;

		adjacentBlockType = adjacentBlock->GetBlockType();
	}
	// If inside chunk
	else if (localPosition.y + offset.y >= 0 && localPosition.y + offset.y <= Chunk::Height - 1)
	{
		adjacentBlock = chunk->GetBlockAt((glm::ivec3)GetLocalPosition() + offset);
		if (!adjacentBlock) return true;

		adjacentBlockType = adjacentBlock->GetBlockType();
	}
	else return false;

	// Return to prevent errors if the block doesn't have a valid block type
	if (!adjacentBlockType)
		return true;

	// Don't render air blocks
	if (adjacentBlock->m_BlockId == BlockIds::Air)			
		return true;

	if (adjacentBlockType->isTransparent)
		return true;

	// Because chunks don't stack vertically, nothing can occlude the top or bottom blocks
	//if (direction == Bottom || direction == Top)
		//return true;

	// Render all sides of fully transparent blocks
	if (!adjacentBlockType->isOpaque && GetBlockType()->isTransparent)
		return true;

	// If the adjacent block is translucent and this block is transparent or opaque
	// This is to prevent seeing a side of for example water inside other water blocks, which you wouldn't expect
	if (adjacentBlockType->isTranslucent && !GetBlockType()->isTranslucent)
		return true;

	// If the other block is fully transparent and this block is translucent
	// Then render this blockface aswell
	if (adjacentBlockType->isTransparent && GetBlockType()->isTranslucent)
		return true;

	// If this block is opaque but the adjacent is transparent (ex: stone and leaves)
	if (GetBlockType()->isOpaque && adjacentBlockType->isTransparent) 
		return true;
	
	return false;
}
 
void ChunkBlock::AddBlockFace(Chunk* chunk, BlockFace& face)
{
	glm::vec3 worldPosition = GetWorldPosition(chunk);

	Mesh<PackedVertex>& opaqueMesh = chunk->m_TempOpaqueMesh;
	Mesh<PackedVertex>& waterMesh = chunk->m_TempWaterMesh;

	for (int i = 0; i < 4; i++)
	{
		BlockTexture& texture = World::m_TextureAtlas[face.textureId];

		glm::u8vec3 position(face.positions[i] + GetLocalPosition());

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
	if (m_BlockId == BlockIds::Air) return;

	BlockType* blockType = GetBlockType();

	/*AddBlockFace(blockType->faces[Directions::West]);
	AddBlockFace(blockType->faces[Directions::East]);
	AddBlockFace(blockType->faces[Directions::Top]);
	AddBlockFace(blockType->faces[Directions::Bottom]);
	AddBlockFace(blockType->faces[Directions::South]);
	AddBlockFace(blockType->faces[Directions::North]);*/
}

void ChunkBlock::AddBlockFaces(Chunk* chunk)
{
	if (m_BlockId == BlockIds::Air) return;

	AdjacentChunks adjacentChunks = chunk->GetAdjacentChunks();
	BlockType* blockType = GetBlockType();

	// Check for chunks on the x axis
	if (ShouldAddBlockFace(chunk, Directions::West, adjacentChunks.West))
		AddBlockFace(chunk, blockType->faces[Directions::West]);
	if (ShouldAddBlockFace(chunk, Directions::East, adjacentChunks.East))
		AddBlockFace(chunk, blockType->faces[Directions::East]);

	// Check for chunks on the y axis
	if (ShouldAddBlockFace(chunk, Directions::Top, adjacentChunks.Top))
		AddBlockFace(chunk, blockType->faces[Directions::Top]);
	if (ShouldAddBlockFace(chunk, Directions::Bottom, adjacentChunks.Bottom))
		AddBlockFace(chunk, blockType->faces[Directions::Bottom]);

	// Check for chunks on the z axis
	if (ShouldAddBlockFace(chunk, Directions::South, adjacentChunks.South))
		AddBlockFace(chunk, blockType->faces[Directions::South]);
	if (ShouldAddBlockFace(chunk, Directions::North, adjacentChunks.North))
		AddBlockFace(chunk, blockType->faces[Directions::North]);
}

/*Chunk* ChunkBlock::GetChunk()
{
	return World::GetChunkAt(m_ChunkPosition);
}*/

const glm::u8vec3 ChunkBlock::GetLocalPosition() 
{ 
	glm::u8vec3 position;

	// Unpack the position
	position.x = (m_LocalPositionPacked >> 12) & ((1 << 4) - 1);
	position.y = (m_LocalPositionPacked >> 4) & ((1 << 8) - 1);
	position.z = (m_LocalPositionPacked >> 0) & ((1 << 4) - 1);

	return position;
}

void ChunkBlock::SetLocalPosition(glm::u8vec3 position) 
{ 
	// X will only ever be between 0 and 15 (4 bits)
	// Y will only ever be between 0 and 255 (8 bits)
	// Z will only ever be between 0 and 15 (4 bits)

	m_LocalPositionPacked = (m_LocalPositionPacked << 4) | position.x;
	m_LocalPositionPacked = (m_LocalPositionPacked << 8) | position.y;
	m_LocalPositionPacked = (m_LocalPositionPacked << 4) | position.z;
}

const glm::ivec3 ChunkBlock::GetWorldPosition(Chunk* chunk)
{ 
	glm::ivec2 chunkPosition = chunk->GetWorldPosition();

	return (glm::ivec3) GetLocalPosition() + glm::ivec3(chunkPosition.x, 0, chunkPosition.y);
}

BlockType* ChunkBlock::GetBlockType() 
{ 
	return &BlockTypes::Blocks[m_BlockId]; 
}

Block* ChunkBlock::GetBlock(Chunk* chunk)
{
	// Create a new instance with the data from this block
	Block* block = nullptr;

	switch (m_BlockId)
	{
	case Air:
		break;
	case Grass:
		break;
	case Dirt:
		block = new Blocks::DirtBlock(chunk, this);
		break;
	case Stone:
		block = new Blocks::StoneBlock(chunk, this);
		break;
	case Water:
		break;
	case Gravel:
		break;
	case Sand:
		break;
	case OakLog:
		break;
	case OakLeaves:
		break;
	case Chest:
		break;
	
	default:
		break;
	}

	if (block) return block;

	return new Block(chunk, this);
}

ChunkBlock::~ChunkBlock()
{
}