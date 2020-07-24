#include "ChunkBlock.h"

#include <iostream>
#include <math.h>

#include "Chunk.h"
#include "../World.h"
#include "../Mesh.h"
#include "../Utils.h"

AdjacentChunks::AdjacentChunks() { }

ChunkBlock::ChunkBlock()
{

}

bool ChunkBlock::ShouldAddBlockFace(ChunkBorderEdges direction, Chunk* adjacentChunk, glm::vec3 offset)
{
	if (!m_Enabled) return false;

	glm::vec3 blockInAdjacentChunkOffset(0, 0, 0);

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
		case Left: blockInAdjacentChunkOffset = glm::vec3(Chunk::Width - 1, 0, 0); break;
		case Right: blockInAdjacentChunkOffset = glm::vec3(-Chunk::Width + 1 , 0, 0); break;
		case Back: blockInAdjacentChunkOffset = glm::vec3(0, 0, Chunk::Depth - 1); break;
		case Front: blockInAdjacentChunkOffset = glm::vec3(0, 0, -Chunk::Depth + 1); break;
	}

	if (isAtChunkBorder)
	{
		//std::cout << m_Enabled;
		return true;// if (!adjacentChunk && m_Enabled) return true;
		return false;

		glm::vec3 p = m_LocalPosition + blockInAdjacentChunkOffset;

		// Check if the block adjacent to this one (in the adjacent chunk) is enabled
		bool chunkHasBlock = adjacentChunk->BlockExistsAt(m_LocalPosition + blockInAdjacentChunkOffset);

		if (!chunkHasBlock)
			return true;
	}
	else if(m_LocalPosition.y + offset.y >= 0 && m_LocalPosition.y + offset.y <= Chunk::Height - 1) {
		// Check if a block exists at offset
		if (!BlockExistsAtRelativePosition(offset))
			return true;
	}
	else return false;

	return false;
}

void ChunkBlock::AddBlockFace(const float* face, const glm::vec4* colors)
{
	for (int i = 0; i < 18; i += 3)
	{
		// Convert the vertex's position to a vec3
		glm::vec3 vertexPosition(face[i + 0], face[i + 1], face[i + 2]);
		
		GetChunk()->m_Vertices.push_back(Vertex(vertexPosition + GetWorldPosition(), colors[i / 3]));
	}
}

void ChunkBlock::AddAllBlockFaces(const glm::vec4* colors)
{
	if (!m_Enabled) return;

	AddBlockFace(CubeFaces::Bottom, &colors[0 * 0]);
	AddBlockFace(CubeFaces::Top, &colors[6 * 1]);
	AddBlockFace(CubeFaces::Left, &colors[6 * 2]);
	AddBlockFace(CubeFaces::Right, &colors[6 * 3]);
	AddBlockFace(CubeFaces::Front, &colors[6 * 4]);
	AddBlockFace(CubeFaces::Back, &colors[6 * 5]);
}

void ChunkBlock::AddBlockFaces(const glm::vec4* colors)
{
	if (!GetEnabled()) return;

	AdjacentChunks adjacentChunks = GetAdjacentChunks();

	// Check for chunks on the x axis
	if (ShouldAddBlockFace(Left, adjacentChunks.Left, glm::vec3(-1, 0, 0)))
		AddBlockFace(CubeFaces::Left, &colors[6 * 2]);
	if (ShouldAddBlockFace(Right, adjacentChunks.Right, glm::vec3(1, 0, 0)))
		AddBlockFace(CubeFaces::Right, &colors[6 * 3]);

	// Check for chunks on the y axis
	if (ShouldAddBlockFace(Top, adjacentChunks.Top, glm::vec3(0, 1, 0)))
		AddBlockFace(CubeFaces::Top, &colors[6 * 1]);
	if (ShouldAddBlockFace(Bottom, adjacentChunks.Bottom, glm::vec3(0, -1, 0)))
		AddBlockFace(CubeFaces::Bottom, &colors[6 * 0]);

	// Check for chunks on the z axis
	if (ShouldAddBlockFace(Back, adjacentChunks.Back, glm::vec3(0, 0, -1)))
		AddBlockFace(CubeFaces::Back, &colors[6 * 4]);
	if (ShouldAddBlockFace(Front, adjacentChunks.Front, glm::vec3(0, 0, 1)))
		AddBlockFace(CubeFaces::Front, &colors[6 * 5]);
}

Chunk* ChunkBlock::GetChunk()
{
	return World::GetChunkFromIndex(m_ChunkIndex);
}

const glm::vec3 ChunkBlock::GetLocalPosition() 
{ 
	return m_LocalPosition; 
}

const glm::vec3 ChunkBlock::GetWorldPosition() 
{ 
	return m_LocalPosition + GetChunk()->GetWorldPosition(); 
}

void ChunkBlock::SetLocalPosition(glm::vec3 position) 
{ 
	m_LocalPosition = position; 

}
void ChunkBlock::SetWorldPosition(glm::vec3 position) 
{ 
	m_LocalPosition = position; 
}

void ChunkBlock::SetEnabled(bool flag) { m_Enabled = flag; }
bool ChunkBlock::GetEnabled() { return m_Enabled; }

Chunk* ChunkBlock::GetChunkAtRelativePosition(glm::vec3 offset)
{
	return World::GetChunkAtPosition(Utils::WorldPositionToChunkPosition(GetWorldPosition() + offset));
}

ChunkBlock* ChunkBlock::GetBlockAtRelativePosition(glm::vec3 offset)
{
	return GetChunk()->GetBlockAt(m_LocalPosition + offset);
}

bool ChunkBlock::ChunkExistsAtRelativePosition(glm::vec3 offset)
{
	return World::ChunkExistsAtPosition(Utils::WorldPositionToChunkPosition(GetWorldPosition() + offset));
}

bool ChunkBlock::BlockExistsAtRelativePosition(glm::vec3 offset)
{
	return GetChunk()->BlockExistsAt(m_LocalPosition + offset);
}

AdjacentChunks ChunkBlock::GetAdjacentChunks()
{
	AdjacentChunks chunks;

	chunks.Left = GetChunkAtRelativePosition(glm::vec3(-1, 0, 0));
	chunks.Right = GetChunkAtRelativePosition(glm::vec3(1, 0, 0));
	chunks.Back = GetChunkAtRelativePosition(glm::vec3(0, 0, -1));
	chunks.Front = GetChunkAtRelativePosition(glm::vec3(0, 0, 1));

	return chunks;
}

ChunkBlock::~ChunkBlock()
{
}
