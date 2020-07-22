#include "Chunk.h"

#include <GL/glew.h>

#include <iostream>

#include "ChunkBlock.h"
#include "../Utils.h"

Chunk::Chunk(glm::vec3 position)
{
	// Generate vertex array object
	CreateVao();

	SetPosition(position);

	m_Index = Utils::ChunkPositionToIndex(position);

	// Fill the array with blocks
	m_Blocks = new ChunkBlock**[Chunk::Width];

	for (int i = 0; i < Chunk::Width; i++)
	{
		m_Blocks[i] = new ChunkBlock*[Chunk::Height];

		for (int j = 0; j < Chunk::Height; j++)
		{
			m_Blocks[i][j] = new ChunkBlock[Chunk::Depth];
			for (int k = 0; k < Chunk::Depth; k++)
				m_Blocks[i][j][k].m_ChunkIndex = m_Index;
		}
	}
}

void Chunk::Fill(const glm::vec4* colors)
{
	for (int x = 0; x < Width; x++)
	{
		for (int y = 0; y < Height; y++)
		{
			for (int z = 0; z < Width; z++)
			{
				//std::cout << x << "; " << y << "; " << z << "\n";
				glm::vec3 position(x, y, z); 

				ChunkBlock* block = GetBlockAt(position);

				// Add all the faces on the cube
				block->AddBlockFaces(position, colors);
			}
		}
	}
}

void Chunk::UpdateMesh()
{
	UpdateVertices(m_Vertices);
}

void Chunk::SetBlockAt(glm::ivec3 position, ChunkBlock* newBlock)
{
	m_Blocks[position.x][position.y][position.z] = *newBlock;
}

void Chunk::SetBlockAt(glm::vec3 position, ChunkBlock* newBlock)
{
	SetBlockAt((glm::ivec3)position, newBlock);
}

ChunkBlock*** Chunk::GetAllBlocks()
{
	return m_Blocks;
}

ChunkBlock* Chunk::GetBlockAt(glm::ivec3 position)
{
	return &m_Blocks[position.x][position.y][position.z];
}

ChunkBlock* Chunk::GetBlockAt(glm::vec3 position)
{
	return GetBlockAt((glm::ivec3)position);
}

glm::vec3 Chunk::GetPosition() { return m_Position; }
void Chunk::SetPosition(glm::vec3 position) { m_Position = position; }

int Chunk::GetIndex() { return m_Index; }

Chunk::~Chunk()
{
}
