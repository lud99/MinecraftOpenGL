#include "Chunk.h"

#include <GL/glew.h>

#include <iostream>

#include "../FastNoise/FastNoise.h"

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
				m_Blocks[x][y][z].SetEnabled(false);
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
				ChunkBlock* block = GetBlockAt(glm::vec3(x, y, z));
				
				block->SetEnabled(true);
			}
		}
	}
}

void Chunk::CreateSphere(const glm::vec4* colors)
{
	for (int z = 0; z < Chunk::Width; z++)
	{
		for (int y = 0; y < Chunk::Height; y++)
		{
			for (int x = 0; x < Chunk::Depth; x++)
			{
				if (sqrt((float)(x - Chunk::Width / 2)*(x - Chunk::Width / 2) + (y - Chunk::Height / 2)*(y - Chunk::Height / 2) + (z - Chunk::Depth / 2)*(z - Chunk::Depth / 2)) <= Chunk::Width / 2)
				{
					ChunkBlock* block = GetBlockAt(glm::vec3(x, y, z));

					block->SetEnabled(true);
				}
			}
		}
	}
}

void Chunk::GenerateTerrain()
{
	FastNoise myNoise; // Create a FastNoise object
	myNoise.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type

	for (int x = 0; x < Width; x++)
	{
		for (int y = 0; y < Height; y++) 
		{
			for (int z = 0; z < Depth; z++)
			{
				ChunkBlock* block = GetBlockAt(glm::vec3(x, y, z));


				glm::vec3 worldPosition = block->GetWorldPosition();
				float height = (int)((myNoise.GetNoise(worldPosition.x, worldPosition.z) + 1) / 2 * 16);

				if (y == height) 
				{
					block->SetEnabled(true);
				} 
				else if (y < height || y > height) 
				{
					block->SetEnabled(false);
				}



				//std::cout << height << "\n";

				//m_Blocks[x][y][z] = myNoise.GetNoise(x, y);
			}
		}
	}
}

void Chunk::UpdateMesh()
{
	for (int z = 0; z < Chunk::Width; z++)
	{
		for (int y = 0; y < Chunk::Height; y++)
		{
			for (int x = 0; x < Chunk::Depth; x++)
			{
				ChunkBlock* block = GetBlockAt(glm::vec3(x, y, z));

				// Add all the faces on the cube
				block->AddBlockFaces(colors);
			}
		}
	}

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

bool Chunk::BlockExistsAt(glm::vec3 localPosition)
{
	ChunkBlock* b= GetBlockAt(localPosition);
	return GetBlockAt(localPosition)->GetEnabled();
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
	// Delete the blocks
	for (int i = 0; i < Width; ++i)
	{
		for (int j = 0; j < Width; ++j)
		{
			delete[] m_Blocks[i][j];
		}

		delete[] m_Blocks[i];
	}
	delete[] m_Blocks;
}