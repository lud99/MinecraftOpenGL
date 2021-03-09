#include "Chunk.h"

#include <iostream>
#include <algorithm>
#include <thread>

#include "../../Noise/FastNoise.h"
#include "ChunkBlock.h"
#include "../DroppedItem.h"
#include "BlockEntity.h"
#include "ChunkIO.h"
#include "../../Blocks/BlockTypes.h"
#include "../World.h"
#include "../../Noise/NoiseGenerator.h"
#include "../../Utils/ThreadPool.h"
#include "../../Blocks/BlockIds.h"
#include "../../Blocks/Blocks.h"

Chunk::Chunk(glm::ivec2 position)
{
	SetPosition(position);

	m_AdjacentChunksWhenLastRebuilt = GetAdjacentChunks();

	m_OpaqueMesh.m_Texture = World::m_TextureAtlas.Texture;
	m_WaterMesh.m_Texture = World::m_TextureAtlas.Texture;
}

void Chunk::Init()
{
	BlockEntity* chest = new BlockEntity();
	chest->m_BlockId = BlockIds::Chest;
	chest->SetLocalPosition(glm::u8vec3(0, 60, 0));
	chest->m_ChunkPosition = m_Position;

	m_BlockEntities[Utils::BlockPositionToIndex(glm::u8vec3(0, 60, 0))] = chest;

	m_Blocks = new ChunkBlock[Chunk::BlockCount];

	// Fill the array with blocks
	for (int x = 0; x < Chunk::Width; x++)
	{
		for (int y = 0; y < Chunk::Height; y++)
		{
			for (int z = 0; z < Chunk::Depth; z++)
			{
				uint16_t index = Utils::BlockPositionToIndex(glm::u8vec3(x, y, z));

				ChunkBlock& block = m_Blocks[index];

				//block.m_ChunkPosition = m_Position;
				block.SetLocalPosition(glm::u8vec3(x, y, z));
				block.m_BlockId = BlockIds::Air;
			}
		}
	}

	m_IsInitialized = true;
}

void Chunk::CreateGenerateAndBuild(ChunkAction* nextAction)
{
	World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::CreateGenerateAndBuild, this, nextAction));
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
				block->m_BlockId = BlockIds::Stone;

				//block->SetEnabled(true);
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
				if (sqrt((float)(x - Chunk::Width / 2) * (x - Chunk::Width / 2) + (y - Chunk::Height / 2) * (y - Chunk::Height / 2) + (z - Chunk::Depth / 2) * (z - Chunk::Depth / 2)) <= Chunk::Width / 2)
				{
					ChunkBlock* block = GetBlockAt(glm::vec3(x, y, z));
					block->m_BlockId = BlockIds::Stone;

					//block->SetEnabled(true);
				}
			}
		}
	}
}

void Chunk::GenerateTerrain()
{
	m_IsGenerating = true;

	std::mutex mutex;
	mutex.lock();

	int offset = 32 * 2;
	int waterLevel = 27 * 2;

	NoiseGenerator noise;
	noise.SetSeed(123456789);
	noise.SetNoiseType(FastNoise::Perlin);

	for (int x = 0; x < Chunk::Width; x++)
	{
		for (int z = 0; z < Chunk::Depth; z++)
		{
			glm::vec3 worldPosition = GetBlockAt(glm::vec3(x, 0, z))->GetWorldPosition(this);

			float scale1 = 7.5f, scale2 = 0.25, scale3 = 10.0f, scale4 = 7.0f;
			float holesScale = 20.0f;

			float noise1 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale1, worldPosition.z * scale1, 4)) * 4.125f;
			float noise2 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale2, worldPosition.z * scale2, 3)) * 2.5f;
			float noise3 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale2 * 3, worldPosition.z * scale2 * 3, 3)) * 0.5f;
			float noise4 = noise.Normalize(noise.GetCombinedNoise(worldPosition.x * scale3, worldPosition.z * scale3, 3, 3)) * .025f;
			float noise5 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale4, worldPosition.z * scale4, 3)) * 2.0f + 10; // small holes in ground
			float holesNoise = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * holesScale + 12345, worldPosition.z * holesScale - 12345, 6)); // Holes

			float height = noise1 + (noise2 * noise3) - (noise4) - noise5;

			/*float noise1 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale1, worldPosition.z * scale1, 4)) * 3.125f;
			float noise2 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale2, worldPosition.z * scale2, 3)) * 2.5f;
			float noise3 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale2 * 3, worldPosition.z * scale2 * 3, 3)) * 2.5f;
			float noise4 = noise.Normalize(noise.GetCombinedNoise(worldPosition.x * scale3, worldPosition.z * scale3, 3, 3)) * .025f;
			float noise5 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale4, worldPosition.z * scale4, 3)) * 2.0f + 10; // small holes in ground
			float holesNoise = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * holesScale + 12345, worldPosition.z * holesScale - 12345, 6)); // Holes

			float height = noise1 + (2*noise2 * noise3) + (noise4) - noise5;*/
			bool setWater = false;

			if (holesNoise > 26) {
				float s = 5.0f;

				float h = (noise.Normalize(noise.GetOctaveNoise(worldPosition.x * s, worldPosition.z * s, 3)) * 0.5f) - 0.25f;

				if (h < 0) h = 0;

				height *= h;
			}

			height += offset;

			if (height < waterLevel) {
				setWater = true;
			}

			//float scale1 = 5.0f;
			//float scale2 = 12.0f;
			//float scale3 = 20.0f;

			//float noise1 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale1, worldPosition.z * scale1, 5)) / 1.75f - 2.f;
			//float noise2 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale2, worldPosition.z * scale2, 2)) / 2.f + 0.25f;
			//float noise3 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale3, worldPosition.z * scale3, 6)); // Holes

			//float height = noise1 + noise2;

			//bool setWater = false;

			//if (noise3 > 36) {
			//	float s = 5.0f;

			//	height = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * s, worldPosition.z * s, 3)) / 2.f;
			//}

			//if (height < 1.0f) {
			//	setWater = true;

			//	height = 0;
			//}

			int finalHeight = (int)floor(height);

			float dirtThickness = noise.GetOctaveNoise(worldPosition.x, worldPosition.z, 8) / 24 - 2;
			float dirtTransition = finalHeight;
			float stoneTransition = dirtTransition + dirtThickness;

			bool sandChance = (noise.Normalize(noise.GetOctaveNoise(worldPosition.x, worldPosition.z, 8)) > 0.5f);
			bool gravelChance = (noise.Normalize(noise.GetOctaveNoise(worldPosition.x, worldPosition.z, 8)) > 4.f);

			for (int y = 0; y < Chunk::Height; y++)
			{
				ChunkBlock* block = GetBlockAt(glm::vec3(x, y, z));
				ChunkBlock* blockAbove = GetBlockAt(glm::vec3(x, y + 1, z));

				BlockIds blockId = BlockIds::Air;

				if (y <= stoneTransition) blockId = BlockIds::Stone;
				else if (y <= dirtTransition) blockId = BlockIds::Dirt;

				if (y >= finalHeight)
				{
					if (setWater && y <= waterLevel)
						blockId = BlockIds::Water;
				}

				block->m_BlockId = blockId;
			}

			{
				float y = finalHeight;

				ChunkBlock* block = GetBlockAt(glm::vec3(x, y, z));
				ChunkBlock* blockAbove = GetBlockAt(glm::vec3(x, y + 1, z));

				if (blockAbove->m_BlockId == BlockIds::Water && gravelChance) {
					block->m_BlockId = BlockIds::Gravel;
				}

				if (blockAbove->m_BlockId == BlockIds::Air) {
					if (y <= waterLevel && sandChance)
						block->m_BlockId = BlockIds::Sand;
					else
						block->m_BlockId = BlockIds::Grass;
				}
			}

			m_HeightMap[x][z] = (uint8_t)finalHeight;



		}

		/*if (y < finalHeight)
		{
			block->SetEnabled(true);

			if (y < finalHeight - (noise1 * 0.25f))
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
			if (setWater && y <= (waterLevel)) {
				block->m_BlockId = BlockIds::Water;
			}
			else {
				block->SetEnabled(false);
				block->m_BlockId = BlockIds::Air;
			}

		}*/

	}

	// Tree generation
	for (int x = 0; x < Chunk::Width; x++)
	{
		for (int z = 0; z < Chunk::Depth; z++)
		{
			uint8_t finalHeight = m_HeightMap[x][z];

			if (x == 8 && z == 8)
			{
				glm::vec3 treeTrunk(8, finalHeight + 1, 8);
				int treeTrunkHeight = 6;
				int treeTopHeight = 4;
				int treeTopOffset = 3;
				int treeHeight = treeTrunkHeight + (treeTopHeight - treeTopOffset);
				int treeRadius = 3;

				for (int y = 0; y < treeTrunkHeight; y++)
				{
					ChunkBlock* block = GetBlockAt(glm::vec3(x, finalHeight + y + 1, z));
					block->m_BlockId = BlockIds::OakLog;
				}

				for (int y = treeTrunk.y + treeTopOffset; y < treeTrunk.y + treeHeight; y++)
				{
					for (int rx = treeTrunk.x - treeRadius; rx <= treeTrunk.x + treeRadius; rx++)
					{
						for (int rz = treeTrunk.z - treeRadius; rz <= treeTrunk.z + treeRadius; rz++)
						{
							if ((treeTrunk.x != rx || treeTrunk.z != rz) || y >= treeTrunk.y + treeTrunkHeight)
							{
								float dist = glm::distance(glm::vec2(rx, rz), glm::vec2(treeTrunk.x, treeTrunk.z));
								if (dist < treeRadius * 0.75f)
								{
									ChunkBlock* block = GetBlockAt(glm::vec3(rx, y, rz));
									block->m_BlockId = BlockIds::OakLeaves;
								}
							}
						}
					}

					if (y == treeTrunk.y + treeHeight - 2) treeRadius -= 1;
				}
			}
		}
	}

	m_HasGenerated = true;
	m_IsGenerating = false;
	m_ShouldRebuild = true;

	// Set flag for adjacent chunks to rebuild to remove uneq. block faces at chunk border
	/*AdjacentChunks chunks = GetAdjacentChunks();
	if (chunks.West) chunks.West->SetDirty(true);
	if (chunks.North) chunks.North->SetDirty(true);
	if (chunks.East) chunks.East->SetDirty(true);
	if (chunks.South) chunks.South->SetDirty(true);*/

	World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, this));

	//std::cout << "Terrain generatation done. " << m_Position.x << ", " << m_Position.y << "\n";

	mutex.unlock();
}

void Chunk::RebuildMesh()
{
	if (!m_IsInitialized)
	{
		std::cout << "Chunk not initialized. Cant rebuild mesh\n";
		return;
	}

	SetDirty(true);
	m_IsRebuilding = true;

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
				block->AddBlockFaces(this);
			}
		}
	}

	// Iterate through all block entities
	for (auto entry : m_BlockEntities)
	{
		BlockEntity* entity = entry.second;

		// Add all the faces on the cube
		entity->AddBlockFaces();
	}

	m_OpaqueMesh.SetVertices(m_TempOpaqueMesh.GetVertices());
	m_OpaqueMesh.SetIndices(m_TempOpaqueMesh.GetIndices());
	m_WaterMesh.SetVertices(m_TempWaterMesh.GetVertices());
	m_WaterMesh.SetIndices(m_TempWaterMesh.GetIndices());

	m_TempOpaqueMesh.Clear();
	m_TempWaterMesh.Clear();

	// Set the adjacent chunks
	m_AdjacentChunksWhenLastRebuilt = GetAdjacentChunks();

	//std::cout << "Chunk mesh rebuilt. " << m_Position.x << ", " << m_Position.y << "\n";

	m_IsRebuilding = false;
	SetDirty(false);

	m_MeshMutex.unlock();
}

void Chunk::RebuildMeshThreaded(ChunkAction* nextAction)
{
	if (!m_IsInitialized) std::cout << "Not int\n";
	if (!m_IsInitialized) return;

	m_IsRebuilding = true;
	SetDirty(true);

	World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, this, nextAction));
}

void Chunk::GenerateTerrainThreaded(ChunkAction* nextAction)
{
	//if (!m_IsInitialized) return;

	m_IsGenerating = true;

	World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Generate, this, nextAction));
}

void Chunk::Render()
{
	m_OpaqueMesh.Render();
	m_WaterMesh.Render();
}

void Chunk::SetBlockAt(glm::ivec3 position, ChunkBlock* newBlock)
{
	uint16_t index = Utils::BlockPositionToIndex(position);

	m_Blocks[index] = *newBlock;
}

void Chunk::SetBlockAt(glm::vec3 position, ChunkBlock* newBlock)
{
	SetBlockAt((glm::ivec3)position, newBlock);
}

ChunkBlock* Chunk::GetAllBlocks() { return m_Blocks; }

ChunkBlock* Chunk::GetBlockAt(glm::u8vec3 position)
{
	uint16_t index = Utils::BlockPositionToIndex(position);

	return &m_Blocks[index];
}

ChunkBlock* Chunk::GetBlockAt(glm::ivec3 position)
{
	return GetBlockAt((glm::u8vec3)position);
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

	return true;
}

glm::ivec2& Chunk::GetPosition() { return m_Position; }

glm::ivec2 Chunk::GetWorldPosition()
{
	return m_Position * glm::ivec2(Chunk::Width, Chunk::Depth);
}

void Chunk::SetPosition(glm::ivec2 position) { m_Position = position; }

AdjacentChunks Chunk::GetAdjacentChunks()
{
	AdjacentChunks chunks;

	chunks.West = World::GetChunkAt(m_Position + glm::ivec2(-1, 0));
	chunks.East = World::GetChunkAt(m_Position + glm::ivec2(1, 0));
	chunks.South = World::GetChunkAt(m_Position + glm::ivec2(0, -1));
	chunks.North = World::GetChunkAt(m_Position + glm::ivec2(0, 1));

	return chunks;
}

void Chunk::SetDirty(bool flag) { m_IsDirty = flag; }
bool Chunk::IsDirty() { return m_IsDirty; }

Chunk::~Chunk()
{
	std::cout << "delete chunk\n";

	// Delete the blocks
	delete[] m_Blocks;
}