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

	// Create the blocks on the x axis
	for (int x = 0; x < Chunk::Width; x++)
	{
		m_Blocks[x] = new ChunkBlock*[Chunk::Height]; // Blocks on x reference an array of y blocks 

		for (int y = 0; y < Chunk::Height; y++)
		{
			m_Blocks[x][y] = new ChunkBlock[Chunk::Depth]; // Blocks on xy reference array of z blocks

			for (int z = 0; z < Chunk::Depth; z++) // All blocks are already created. Iterate through each block to set some variables
			{
				m_Blocks[x][y][z].m_ChunkIndex = m_Index;
				m_Blocks[x][y][z].SetLocalPosition(glm::vec3(x, y, z));
			}
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
				glm::vec3 position(x, y, z); 

				ChunkBlock* block = GetBlockAt(position);

				// Add all the faces on the cube
				block->AddBlockFaces(colors);
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

glm::vec3 Chunk::GetWorldPosition()
{
	return m_Position * glm::vec3(Chunk::Width, 1, Chunk::Depth);
}

void Chunk::SetPosition(glm::vec3 position) { m_Position = position; }

int Chunk::GetIndex() { return m_Index; }

Chunk::~Chunk()
{
}
