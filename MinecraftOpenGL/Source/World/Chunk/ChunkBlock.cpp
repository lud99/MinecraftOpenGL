#include "ChunkBlock.h"

#include <iostream>
#include <math.h>

#include "Chunk.h"
#include "../World.h"
#include "../../Graphics/Mesh.h"
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
	case Left:
		if (m_LocalPosition.x - 1 < 0)
			isAtChunkBorder = true;

		offset = glm::ivec3(-1, 0, 0);
		blockInAdjacentChunkOffset = glm::ivec3(Chunk::Width - 1, 0, 0);

		break;
	case Right:
		if (m_LocalPosition.x + 1 >= Chunk::Width)
			isAtChunkBorder = true;

		offset = glm::ivec3(1, 0, 0);
		blockInAdjacentChunkOffset = glm::ivec3(-Chunk::Width + 1, 0, 0);

		break;
	case Bottom:
		if (m_LocalPosition.y - 1 < 0)
			isAtChunkBorder = true;

		offset = glm::ivec3(0, -1, 0);

		break;
	case Top:
		if (m_LocalPosition.y + 1 >= Chunk::Height)
			isAtChunkBorder = true;

		offset = glm::ivec3(0, 1, 0);

		break;
	case Back:
		if (m_LocalPosition.z - 1 < 0)
			isAtChunkBorder = true;

		offset = glm::ivec3(0, 0, -1);
		blockInAdjacentChunkOffset = glm::ivec3(0, 0, Chunk::Depth - 1);

		break;
	case Front:
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

	Mesh& opaqueMesh = chunk->m_TempOpaqueMesh; 
	Mesh& waterMesh = chunk->m_TempWaterMesh;

	for (int i = 0; i < 4; i++)
	{
		BlockTexture& texture = World::m_TextureAtlas[face.textureId];

		glm::u8vec3 position(face.positions[i] + m_LocalPosition);

		int lightLevel = 15;

		switch (face.direction)
		{
		case Top: lightLevel = 15; break;
		case Left: lightLevel = 12; break;
		case Right: lightLevel = 12; break;
		case Front: lightLevel = 9; break;
		case Back: lightLevel = 9; break;
		case Bottom: lightLevel = 6; break;
		}

		Vertex vertex;
		VertexData vertexData;

		vertexData.position = position;
		vertexData.index = i;
		vertexData.texture = face.textureId;
		vertexData.lightLevel = lightLevel;

		vertex.SetData(vertexData);

		if (m_BlockId == BlockIds::Water)
			waterMesh.AddVertex(vertex);
		else
			opaqueMesh.AddVertex(vertex);
	}

	for (int i = 0; i < 6; i++)
	{
		if (m_BlockId == BlockIds::Water)
			waterMesh.AddIndex(CubeFaces::Indices[i] + (uint16_t) (waterMesh.GetVertices().size() - 4));
		else
			opaqueMesh.AddIndex(CubeFaces::Indices[i] + (uint16_t) (opaqueMesh.GetVertices().size() - 4));
	}
} 

void ChunkBlock::AddAllBlockFaces()
{
	if (!m_Enabled || m_BlockId == BlockIds::Air) return;

	Block* blockType = GetBlockType();

	AddBlockFace(blockType->faces[Left]);
	AddBlockFace(blockType->faces[Right]);
	AddBlockFace(blockType->faces[Top]);
	AddBlockFace(blockType->faces[Bottom]);
	AddBlockFace(blockType->faces[Back]);
	AddBlockFace(blockType->faces[Front]);
}

void ChunkBlock::AddBlockFaces()
{
	if (!m_Enabled || m_BlockId == BlockIds::Air) return;

	AdjacentChunks adjacentChunks = GetChunk()->GetAdjacentChunks();
	Block* blockType = GetBlockType();

	// Check for chunks on the x axis
	if (ShouldAddBlockFace(Left, adjacentChunks.Left))
		AddBlockFace(blockType->faces[Left]);
	if (ShouldAddBlockFace(Right, adjacentChunks.Right))
		AddBlockFace(blockType->faces[Right]);

	// Check for chunks on the y axis
	if (ShouldAddBlockFace(Top, adjacentChunks.Top))
		AddBlockFace(blockType->faces[Top]);
	if (ShouldAddBlockFace(Bottom, adjacentChunks.Bottom))
		AddBlockFace(blockType->faces[Bottom]);

	// Check for chunks on the z axis
	if (ShouldAddBlockFace(Back, adjacentChunks.Back))
		AddBlockFace(blockType->faces[Back]);
	if (ShouldAddBlockFace(Front, adjacentChunks.Front))
		AddBlockFace(blockType->faces[Front]);
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
