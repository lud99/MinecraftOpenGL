#include "Chunk.h"

#include <GL/glew.h>

#include <iostream>
#include <algorithm>
#include <thread>

#include "../../Noise/FastNoise.h"
#include "ChunkBlock.h"
#include "../../Utils.h"
#include "../../Blocks/BlockTypes.h"
#include "../World.h"
#include "../../Graphics/Mesh.h"
#include "../../Noise/NoiseGenerator.h"

Chunk::Chunk(glm::ivec2 position)
{
	// Generate vertex array object
	m_OpaqueMesh.CreateVao();
	m_WaterMesh.CreateVao();

	//m_OpaqueMesh.m_Vertices.reserve(11676);
	
	SetPosition(position);

	m_OpaqueMesh.m_Texture = World::m_TextureAtlas.Texture;
	m_WaterMesh.m_Texture = World::m_TextureAtlas.Texture;

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
				m_Blocks[x][y][z].m_BlockId = BlockIds::Air;
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

void Chunk::GenerateTerrainThreaded()
{
	World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::Generate, this));
}

void Chunk::GenerateTerrain()
{
	std::mutex mutex;
	mutex.lock();

	int waterLevel = Chunk::Height / 2;

	NoiseGenerator noise;
	noise.SetSeed(112);
	noise.SetNoiseType(FastNoise::Simplex);

	for (int x = 0; x < Chunk::Width; x++)
	{
		for (int z = 0; z < Chunk::Depth; z++)
		{
			glm::vec3 worldPosition = GetBlockAt(glm::vec3(x, 0, z))->GetWorldPosition();

			float scale1 = 5.0f;
			float scale2 = 12.0f;
			float scale3 = 20.0f;

			float noise1 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale1, worldPosition.z * scale1, 5)) / 1.75f - 2.f;
			float noise2 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale2, worldPosition.z * scale2, 2)) / 2.f + 0.25f;
			float noise3 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale3, worldPosition.z * scale3, 6)); // Holes

			float height = noise1 + noise2;

			bool setWater = false;

			if (noise3 > 36) {
				float s = 5.0f;

				height = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * s, worldPosition.z * s, 3)) / 2.f;
			}

			if (height < 1.0f) {
				setWater = true;

				height = 0;
			}

			int finalHeight = (int)floor(height) + waterLevel;

			for (int y = 0; y < Chunk::Height; y++)
			{
				ChunkBlock* block = GetBlockAt(glm::vec3(x, y, z));

				if (y < finalHeight)
				{
					block->SetEnabled(true);

					if (y < finalHeight - 2)
						block->m_BlockId = BlockIds::Stone;
					else
						block->m_BlockId = BlockIds::Dirt;
				}
				else if (y == finalHeight)
				{
					block->SetEnabled(true);

					if (!setWater)
						block->m_BlockId = BlockIds::Grass;
					else
						block->m_BlockId = BlockIds::Water;
				}
				else if (y > finalHeight)
				{
					block->SetEnabled(false);
					block->m_BlockId = BlockIds::Air;
				}
			}
		}
	}

	m_HasGenerated = true;

	mutex.unlock();
}

void Chunk::RebuildMesh()
{
	m_MeshMutex.lock();

	m_TempOpaqueMesh.Clear();
	m_TempWaterMesh.Clear();

	for (int x = 0; x < Chunk::Width; x++)
	{
		for (int y = 0; y < Chunk::Height; y++)
		{
			for (int z = 0; z < Chunk::Depth; z++)
			{
				ChunkBlock* block = GetBlockAt(glm::ivec3(x, y, z));

				// Add all the faces on the cube
				block->AddBlockFaces();
			}
		}
	}

	m_OpaqueMesh.SetVertices(m_TempOpaqueMesh.GetVertices());
	m_OpaqueMesh.SetIndices(m_TempOpaqueMesh.GetIndices());
	m_WaterMesh.SetVertices(m_TempWaterMesh.GetVertices());
	m_WaterMesh.SetIndices(m_TempWaterMesh.GetIndices());

	m_TempOpaqueMesh.Clear();
	m_TempWaterMesh.Clear();

	//std::cout << "Chunk mesh rebuilt\n";

	m_MeshMutex.unlock();
}

void Chunk::RebuildMeshThreaded()
{
	World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::Rebuild, this));
}

void Chunk::Render()
{
	m_OpaqueMesh.Render();
	m_WaterMesh.Render();
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
	ChunkBlock* block = GetBlockAt(localPosition);

	if (!block) return false;

	if (block->m_BlockId == BlockIds::Air) return false;

	if (!block->IsEnabled()) return false;

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