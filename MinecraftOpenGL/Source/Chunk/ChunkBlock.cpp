#include "ChunkBlock.h"

#include <iostream>
#include <math.h>

#include "Chunk.h"
#include "../World.h"
#include "../Mesh.h"
#include "../Utils.h"
#include "../Textures/TextureList.h"

AdjacentChunks::AdjacentChunks() { }

ChunkBlock::ChunkBlock()
{
}

bool ChunkBlock::ShouldAddBlockFace(ChunkBorderEdges direction, Chunk* adjacentChunk, glm::ivec3 offset)
{
	if (!m_Enabled) return false;

	glm::ivec3 blockInAdjacentChunkOffset(0, 0, 0);

	bool isAtChunkBorder = false;

	// Check if at a chunk border
	switch (direction)
	{
	case Left:
		if (m_LocalPosition.x - 1 < 0)
			isAtChunkBorder = true;

		break;
	case Right:
		if (m_LocalPosition.x + 1 >= Chunk::Width)
			isAtChunkBorder = true;

		break;
	case Bottom:
		if (m_LocalPosition.y - 1 < 0)
			isAtChunkBorder = true;

		break;
	case Top:
		if (m_LocalPosition.y + 1 >= Chunk::Height)
			isAtChunkBorder = true;

		break;
	case Back:
		if (m_LocalPosition.z - 1 < 0)
			isAtChunkBorder = true;

		break;
	case Front:
		if (m_LocalPosition.z + 1 >= Chunk::Depth)
			isAtChunkBorder = true;

		break;
	}
	
	switch (direction) {
		case Left: blockInAdjacentChunkOffset = glm::ivec3(Chunk::Width - 1, 0, 0); break;
		case Right: blockInAdjacentChunkOffset = glm::ivec3(-Chunk::Width + 1 , 0, 0); break;
		case Back: blockInAdjacentChunkOffset = glm::ivec3(0, 0, Chunk::Depth - 1); break;
		case Front: blockInAdjacentChunkOffset = glm::ivec3(0, 0, -Chunk::Depth + 1); break;
	}

	if (isAtChunkBorder)
	{
		// Because chunks don't stack vertically, nothing can occlude the top or bottom blocks
		if (direction == Bottom || direction == Top) return true;

		if (!adjacentChunk && m_Enabled) return true;

		return true;
		

		glm::vec3 p = m_LocalPosition + blockInAdjacentChunkOffset;

		// Check if the block adjacent to this one (in the adjacent chunk) is enabled
		bool chunkHasBlock = adjacentChunk->BlockExistsAt(m_LocalPosition + blockInAdjacentChunkOffset);

		if (!chunkHasBlock)
			return true;
	}
	else if(m_LocalPosition.y + offset.y >= 0 && m_LocalPosition.y + offset.y <= Chunk::Height - 1) {
		//ChunkBlock* b = GetBlockAtRelativePosition(offset);

		//if (m_BlockType->isTransparent && b->m_BlockType->isTransparent);
			//return false;

		// Check if a block exists at offset
		if (!BlockExistsAtRelativePosition(offset))
			return true;
	}

	return false;
}

void ChunkBlock::AddBlockFace(BlockFace& face)
{
	Chunk* chunk = GetChunk();

	for (int i = 0; i < 4; i ++)
	{
		BlockTexture& texture = World::Textures[face.textureId];

		glm::vec3 position = face.positions[i] + (glm::vec3)GetWorldPosition();

		//std::cout << position.x << ", ";

		chunk->m_Vertices.push_back(Vertex(position, texture.textureCoordinates[i]));
	}

	for (int i = 0; i < 6; i++)
		chunk->m_Indices.push_back(CubeFaces::Indices[i] + chunk->m_Vertices.size() - 4);
}

void ChunkBlock::AddAllBlockFaces()
{
	if (!m_Enabled) return;

	AddBlockFace(m_BlockType->faces[Left]);
	AddBlockFace(m_BlockType->faces[Right]);
	AddBlockFace(m_BlockType->faces[Top]);
	AddBlockFace(m_BlockType->faces[Bottom]);
	AddBlockFace(m_BlockType->faces[Back]);
	AddBlockFace(m_BlockType->faces[Front]);
}

void ChunkBlock::AddBlockFaces()
{
	if (!GetEnabled()) return;

	AdjacentChunks adjacentChunks = GetAdjacentChunks();

	// Check for chunks on the x axis
	if (ShouldAddBlockFace(Left, adjacentChunks.Left, glm::ivec3(-1, 0, 0)))
		AddBlockFace(m_BlockType->faces[Left]);
	if (ShouldAddBlockFace(Right, adjacentChunks.Right, glm::ivec3(1, 0, 0)))
		AddBlockFace(m_BlockType->faces[Right]);

	// Check for chunks on the y axis
	if (ShouldAddBlockFace(Top, adjacentChunks.Top, glm::ivec3(0, 1, 0)))
		AddBlockFace(m_BlockType->faces[Top]);
	if (ShouldAddBlockFace(Bottom, adjacentChunks.Bottom, glm::ivec3(0, -1, 0)))
		AddBlockFace(m_BlockType->faces[Bottom]);

	// Check for chunks on the z axis
	if (ShouldAddBlockFace(Back, adjacentChunks.Back, glm::ivec3(0, 0, -1)))
		AddBlockFace(m_BlockType->faces[Back]);
	if (ShouldAddBlockFace(Front, adjacentChunks.Front, glm::ivec3(0, 0, 1)))
		AddBlockFace(m_BlockType->faces[Front]);
}

Chunk* ChunkBlock::GetChunk()
{
	return World::GetChunkFromIndex(m_ChunkIndex);
}

const glm::ivec3 ChunkBlock::GetLocalPosition() 
{ 
	return m_LocalPosition; 
}

const glm::ivec3 ChunkBlock::GetWorldPosition() 
{ 
	glm::ivec2 chunkPosition = GetChunk()->GetWorldPosition();

	return m_LocalPosition + glm::ivec3(chunkPosition.x, 0, chunkPosition.y);
}

void ChunkBlock::SetLocalPosition(glm::ivec3 position) 
{ 
	m_LocalPosition = position; 

}
void ChunkBlock::SetWorldPosition(glm::ivec3 position) 
{ 
	m_LocalPosition = position; 
}

void ChunkBlock::SetEnabled(bool flag) { m_Enabled = flag; }
bool ChunkBlock::GetEnabled() { return m_Enabled; }

Chunk* ChunkBlock::GetChunkAtRelativePosition(glm::ivec3 offset)
{
	return World::GetChunkAtPosition(Utils::WorldPositionToChunkPosition(GetWorldPosition() + offset));
}

ChunkBlock* ChunkBlock::GetBlockAtRelativePosition(glm::ivec3 offset)
{
	return GetChunk()->GetBlockAt(m_LocalPosition + offset);
}

bool ChunkBlock::ChunkExistsAtRelativePosition(glm::ivec3 offset)
{
	return World::ChunkExistsAtPosition(Utils::WorldPositionToChunkPosition(GetWorldPosition() + offset));
}

bool ChunkBlock::BlockExistsAtRelativePosition(glm::ivec3 offset)
{
	return GetChunk()->BlockExistsAt(m_LocalPosition + offset);
}

AdjacentChunks ChunkBlock::GetAdjacentChunks()
{
	AdjacentChunks chunks;

	chunks.Left = GetChunkAtRelativePosition(glm::ivec3(-1, 0, 0));
	chunks.Right = GetChunkAtRelativePosition(glm::ivec3(1, 0, 0));
	chunks.Back = GetChunkAtRelativePosition(glm::ivec3(0, 0, -1));
	chunks.Front = GetChunkAtRelativePosition(glm::ivec3(0, 0, 1));

	return chunks;
}

ChunkBlock::~ChunkBlock()
{
}
