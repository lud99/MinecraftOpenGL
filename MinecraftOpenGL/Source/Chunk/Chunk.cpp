#include "Chunk.h"

#include <GL/glew.h>

#include <iostream>
#include <algorithm>

#include "../Noise/FastNoise.h"
#include "ChunkBlock.h"
#include "../Utils.h"
#include "../BLocks/BlockTypes.h"
#include "../World.h"

Chunk::Chunk(glm::ivec2 position)
{
	// Generate vertex array object
	CreateVao();
	
	SetPosition(position);

	m_Texture = World::Textures.Atlas;

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
				m_Blocks[x][y][z].m_BlockType = &BlockTypes::Blocks[BlockIds::Air];
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

float Normalize(float num)
{
	return std::abs(num) * 2;
}

float GetOctaveNoise(float x, float z, int octaves, int f = 4)
{
	FastNoise noise; // Create a FastNoise object
	noise.SetSeed(122);
	noise.SetNoiseType(FastNoise::Simplex); // Set the desired noise type

	float height = 0;
	float u = 1.0f;

	for (int i = 0; i < octaves; i++) {
		height += noise.GetNoise(x / u, z / u) * u;
		u *= 2.0f;
	}

	return height;
}

float GetCombinedNoise(float x, float z, int octaves1, int octaves2, int f = 1)
{
	return GetOctaveNoise(x + GetOctaveNoise(x, z, octaves1, f), z, octaves2, f);
}

void Chunk::GenerateTerrain()
{
	int waterLevel = Height / 2;

	for (int x = 0; x < Width; x++)
	{
		for (int z = 0; z < Depth; z++)
		{
			glm::vec3 worldPosition = GetBlockAt(glm::vec3(x, 0, z))->GetWorldPosition();

			float scale1 = 5.0f;
			float scale2 = 12.0f;
			float scale3 = 20.0f;

			float noise1 = Normalize(GetOctaveNoise(worldPosition.x * scale1, worldPosition.z * scale1, 5)) / 1.75f - 2.f;
			float noise2 = Normalize(GetOctaveNoise(worldPosition.x * scale2, worldPosition.z * scale2, 2)) / 2.f + 0.25f;
			float noise3 = Normalize(GetOctaveNoise(worldPosition.x * scale3, worldPosition.z * scale3, 6)); // Holes

			float height = noise1 + noise2;

			bool setWater = false;

			if (noise3 > 36) {
				float s = 5.0f;

				height = Normalize(GetOctaveNoise(worldPosition.x * s, worldPosition.z * s, 3)) / 2.f;
			}

			if (height < 1.0f) {
				setWater = true;

				height = 0;
			}

			int finalHeight = (int)floor(height) + waterLevel;

			for (int y = 0; y < Height; y++)
			{
				ChunkBlock* block = GetBlockAt(glm::vec3(x, y, z));

				if (y < finalHeight)
				{
					block->SetEnabled(true);

					if (y < finalHeight - 2)
						block->m_BlockType = &BlockTypes::Blocks[BlockIds::Stone];
					else
						block->m_BlockType = &BlockTypes::Blocks[BlockIds::Dirt];
				}
				else if (y == finalHeight)
				{
					block->SetEnabled(true);

					if (!setWater)
						block->m_BlockType = &BlockTypes::Blocks[BlockIds::Grass];
					else
						block->m_BlockType = &BlockTypes::Blocks[BlockIds::Water];
				}
				else if (y > finalHeight)
				{
					block->SetEnabled(false);
					block->m_BlockType = BlockIds::Air;
				}
			}
		}
	}

	m_HasGenerated = true;
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
				block->AddBlockFaces();
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

	if (GetBlockAt(localPosition)->m_BlockType == NULL)
		return false;

	if (b->m_BlockType == &BlockTypes::Blocks[BlockIds::Air])
		return true;

	if (!b->GetEnabled())
		return false;

	return true;
}

glm::ivec2& Chunk::GetPosition() { return m_Position; }

glm::ivec2 Chunk::GetWorldPosition()
{
	return m_Position * glm::ivec2(Chunk::Width, Chunk::Depth);
}

void Chunk::SetPosition(glm::ivec2 position) { m_Position = position; }

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