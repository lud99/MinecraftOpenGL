#include "ChunkBlock.h"

#include <iostream>
#include <math.h>

#include "Chunk.h"
#include "../World.h"
#include "../Mesh.h"
#include "../Utils.h"

ChunkBlock::ChunkBlock()
{

}

void ChunkBlock::AddBlockFace(const float* face, const glm::vec4* colors)
{
	for (int i = 0; i < 18 /* 18 vertices in a face*/; i += 3)
	{
		// Convert the vertex's position to a vec3
		glm::vec3 vertexPosition(face[i + 0], face[i + 1], face[i + 2]);
		
		Chunk* chunk = GetChunk();
		chunk->m_Vertices.push_back(Vertex(vertexPosition + GetWorldPosition(), colors[i / 3]));
	}
}

void ChunkBlock::AddAllBlockFaces(const glm::vec4* colors)
{
	if (m_Hidden) return;

	AddBlockFace(CubeFaces::Bottom, &colors[0 * 0]);
	AddBlockFace(CubeFaces::Top, &colors[6 * 1]);
	AddBlockFace(CubeFaces::Left, &colors[6 * 2]);
	AddBlockFace(CubeFaces::Right, &colors[6 * 3]);
	AddBlockFace(CubeFaces::Front, &colors[6 * 4]);
	AddBlockFace(CubeFaces::Back, &colors[6 * 5]);
}

void ChunkBlock::AddBlockFaces(const glm::vec4* colors)
{
	if (m_Hidden) return;

	Chunk* chunk = GetChunk();

	if (m_LocalPosition.x - 1 < 0)// && m_LocalPosition.z == 0 && m_LocalPosition.y == 0 && GetWorldPosition().z == 0)
	{
		//std::cout << "p; " << GetWorldPosition().x << "\n";
		// Check if a chunk exists to the left on the x axis
		if (!ChunkExistsAtRelativePosition(glm::vec3(-1, 0, 0))) {
			AddBlockFace(CubeFaces::Left, &colors[6 * 2]);

			//std::cout << GetWorldPosition().x << "\n";
		}
	}

	if (m_LocalPosition.x + 1 >= Chunk::Width)// && m_LocalPosition.z == 0 && m_LocalPosition.y == 0)
	{
		// Check if a chunk exists to the right on the x axis
		if (!ChunkExistsAtRelativePosition(glm::vec3(1, 0, 0)))
			AddBlockFace(CubeFaces::Right, &colors[6 * 3]);			
	}

	if (m_LocalPosition.y - 1 < 0)
	{
		// Check if a chunk exists below
		//if (!ChunkExistsAtRelativePosition(glm::vec3(0, -1, 0)))
			AddBlockFace(CubeFaces::Bottom, &colors[0 * 0]);
	}

	if (m_LocalPosition.y + 1 >= Chunk::Height)
	{
		// Check if a chunk exists above
		//if (!ChunkExistsAtRelativePosition(glm::vec3(0, 1, 0)))
			AddBlockFace(CubeFaces::Top, &colors[6 * 1]);
	}

	if (m_LocalPosition.z + 1 >= Chunk::Depth)// && m_LocalPosition.x == 0 && m_LocalPosition.y == 0)
	{
		// Check if a chunk exists to the right on the z axis
		if (!ChunkExistsAtRelativePosition(glm::vec3(0, 0, 1)))
			AddBlockFace(CubeFaces::Front, &colors[6 * 4]);
	}

	if (m_LocalPosition.z - 1 < 0)// && m_LocalPosition.x == 0 && m_LocalPosition.y == 0)
	{
		// Check if a chunk exists to the left on the z axis
		if (!ChunkExistsAtRelativePosition(glm::vec3(0, 0, -1)))
			AddBlockFace(CubeFaces::Back, &colors[6 * 5]);
	}
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

bool ChunkBlock::ChunkExistsAtRelativePosition(glm::vec3 offset)
{
	glm::vec3& lp = m_LocalPosition;
	const glm::vec3& wp = GetWorldPosition();

	int s = World::GetChunks().size();

	bool b = World::ChunkExistsAtPosition(Utils::WorldPositionToChunkPosition(GetWorldPosition() + offset));

	return b;
}

ChunkBlock::~ChunkBlock()
{
}
