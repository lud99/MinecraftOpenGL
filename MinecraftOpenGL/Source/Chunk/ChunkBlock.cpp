#include "ChunkBlock.h"

#include <iostream>

#include "Chunk.h"
#include "../World.h"

ChunkBlock::ChunkBlock()
{

}

void ChunkBlock::AddBlockFace(const float* face, glm::vec3 localPosition, const glm::vec4* colors)
{
	for (int i = 0; i < 18 /* 18 vertices in a face*/; i += 3)
	{
		// Convert the vertex's position to a vec3
		glm::vec3 vertexPosition(face[i + 0], face[i + 1], face[i + 2]);

		m_Vertices.push_back(Vertex(vertexPosition + localPosition + GetChunk()->GetPosition(), colors[i / 3]));
	}
}

void ChunkBlock::AddAllBlockFaces(glm::vec3 localPosition, const glm::vec4* colors)
{
	m_LocalPosition = localPosition;

	if (m_Hidden) return;

	AddBlockFace(CubeFaces::Bottom, localPosition, &colors[0 * 0]);
	AddBlockFace(CubeFaces::Top, localPosition, &colors[6 * 1]);
	AddBlockFace(CubeFaces::Left, localPosition, &colors[6 * 2]);
	AddBlockFace(CubeFaces::Right, localPosition, &colors[6 * 3]);
	AddBlockFace(CubeFaces::Front, localPosition, &colors[6 * 4]);
	AddBlockFace(CubeFaces::Back, localPosition, &colors[6 * 5]);

	std::vector<Vertex>& meshVertices = GetChunk()->m_Vertices;

	meshVertices.insert(meshVertices.end(), m_Vertices.begin(), m_Vertices.end());
}

void ChunkBlock::AddBlockFaces(glm::vec3 localPosition, const glm::vec4* colors)
{
	m_LocalPosition = localPosition;

	if (m_Hidden) return;

	Chunk* chunk = GetChunk();

	if (localPosition.x - 1 < 0)
	{
		//ChunkBlock* blockToLeft = chunk->GetBlockAt(localPosition - glm::vec3(1, 0, 0));
		AddBlockFace(CubeFaces::Left, localPosition, &colors[6 * 2]);
	}


	if (localPosition.x + 1 >= Chunk::Width)
	{
		AddBlockFace(CubeFaces::Right, localPosition, &colors[6 * 3]);
	}

	if (localPosition.y - 1 < 0)
	{
		AddBlockFace(CubeFaces::Bottom, localPosition, &colors[0 * 0]);
	}

	if (localPosition.y + 1 >= Chunk::Height)
	{
		AddBlockFace(CubeFaces::Top, localPosition, &colors[6 * 1]);
	}

	if (localPosition.z + 1 >= Chunk::Depth)
	{
		AddBlockFace(CubeFaces::Front, localPosition, &colors[6 * 4]);
	}

	if (localPosition.z - 1 < 0)
	{
		AddBlockFace(CubeFaces::Back, localPosition, &colors[6 * 5]);
	}

	//ChunkBlock* blockToLeft = chunk->GetBlockAt(localPosition - glm::vec3(1, 0, 0));
	//ChunkBlock* blockToRight = chunk->GetBlockAt(localPosition + glm::vec3(1, 0, 0));

	//AddBlockFace(CubeFaces::Bottom, localPosition, &colors[0 * 0]);
	//AddBlockFace(CubeFaces::Bottom, localPosition, &colors[0 * 0]);
	//AddBlockFace(CubeFaces::Top, localPosition, &colors[6 * 1]);
	//AddBlockFace(CubeFaces::Left, localPosition, &colors[6 * 2]);
	//AddBlockFace(CubeFaces::Right, localPosition, &colors[6 * 3]);
	//AddBlockFace(CubeFaces::Front, localPosition, &colors[6 * 4]);
	//AddBlockFace(CubeFaces::Back, localPosition, &colors[6 * 5]);

	chunk->m_Vertices.insert(chunk->m_Vertices.end(), m_Vertices.begin(), m_Vertices.end());
}

Chunk* ChunkBlock::GetChunk()
{
	return World::GetChunkFromIndex(m_ChunkIndex);
}

const glm::vec3 ChunkBlock::GetLocalPosition() { return m_LocalPosition; }
const glm::vec3 ChunkBlock::GetWorldPosition() { return m_LocalPosition * GetChunk()->GetPosition(); }

void ChunkBlock::SetLocalPosition(glm::vec3 position) { m_LocalPosition = position; }
void ChunkBlock::SetWorldPosition(glm::vec3 position) { m_LocalPosition = position; }

ChunkBlock::~ChunkBlock()
{
}
