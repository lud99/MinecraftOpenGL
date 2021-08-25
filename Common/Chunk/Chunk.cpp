#include "Chunk.h"

#include <iostream>
#include <algorithm>
#include <thread>

#include <Common/Noise/FastNoise.h>
#include "ChunkBlock.h"
#include <Common/Utils/Utils.h>
//#include "../DroppedItem.h"
//#include "BlockEntity.h"
//#include "ChunkIO.h"
#include <Common/Blocks/Blocks.h>
//#include "../World.h"
#include <Common/Noise/NoiseGenerator.h>
#include <Common/ThreadPool.h>
#include <Common/json.hpp>
//#include NetworkThread.h

Chunk::Chunk(glm::ivec2 position, IWorld* world)
{
	SetPosition(position);

	m_World = world;

	//m_AdjacentChunksWhenLastRebuilt = GetAdjacentChunks();
}

void Chunk::Init()
{
	/*BlockEntity* chest = new BlockEntity();
	chest->m_BlockId = BlockIds::Chest;
	chest->SetLocalPosition(glm::u8vec3(0, 60, 0));
	chest->m_ChunkPosition = m_Position;

	m_BlockEntities[Utils::BlockPositionToIndex(glm::u8vec3(0, 60, 0))] = chest;*/

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

	//std::mutex mutex;
	//mutex.lock();

	int offset = 32 * 2;
	int waterLevel = 27 * 2;

	NoiseGenerator noise;
	noise.SetSeed(123456789);
	noise.SetNoiseType(FastNoise::Perlin);

#define oldbiome 1

	for (int x = 0; x < Chunk::Width; x++)
	{
		for (int z = 0; z < Chunk::Depth; z++)
		{			
#ifdef newbiome
			int h = 10;
			float biome = noise.Normalize(noise.GetNoise(worldPosition.x * 0.5f, worldPosition.z * 0.5f)) * 10;

			{
				float scale1 = 7.5f, scale2 = 0.25, scale3 = 10.0f, scale4 = 7.0f;
				float noise1 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale1, worldPosition.z * scale1, 4)) * 1.125f;

				float finalHeight = noise1 + offset;

				id = BlockIds::Sand;

				for (int y = 0; y < finalHeight; y++)
				{
					GetBlockAt(glm::vec3(x, y, z))->m_BlockId = (int)id;
				}
			}


			//if (finalHeight > 255) finalHeight = 255;
			//if (finalHeight < 0) finalHeight = 0;
			//GetBlockAt(glm::vec3(x, finalHeight, z))->m_BlockId = id;
			//for (int y = 0; y < 50; y++)
			//{
			//	//GetBlockAt(glm::vec3(x, y, z))->m_BlockId = (int)id;
			//}

			//m_HeightMap[x][z] = (uint8_t)finalHeight;
#endif
#ifdef oldbiome
			glm::vec3 worldPosition = GetBlockAt(glm::vec3(x, 0, z))->GetWorldPosition(this);

			float scale1 = 7.5f, scale2 = 0.25, scale3 = 10.0f, scale4 = 7.0f;
			float holesScale = 20.0f;

			float noise1 = noise.Normalize(noise.GetOctaveNoise(worldPosition.x * scale1, worldPosition.z * scale1, 4)) * 2.125f;
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
			float dirtTransition = (float)finalHeight;
			float stoneTransition = dirtTransition + dirtThickness;

			bool sandChance = (noise.Normalize(noise.GetOctaveNoise(worldPosition.x, worldPosition.z, 8)) > 0.5f);
			bool gravelChance = (noise.Normalize(noise.GetOctaveNoise(worldPosition.x, worldPosition.z, 8)) > 4.f);

			for (int y = 0; y < Chunk::Height; y++)
			{
				ChunkBlock* block = GetBlockAt(glm::vec3(x, y, z));
				ChunkBlock* blockAbove = GetBlockAt(glm::vec3(x, y + 1, z));

				uint8_t blockId = BlockIds::Air;

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
				int y = finalHeight;

				ChunkBlock* block = GetBlockAt(glm::vec3(x, y, z));
				ChunkBlock* blockAbove = GetBlockAt(glm::vec3(x, y + 1, z));

				if (blockAbove->m_BlockId == BlockIds::Water && gravelChance) {
					block->m_BlockId = BlockIds::Gravel;
				}

				if (blockAbove->m_BlockId == BlockIds::Air) {
					if (y <= waterLevel && sandChance)
						block->m_BlockId = BlockIds::Sand;
					else
						block->m_BlockId = BlockIds::Ice;
				}
			}

			m_HeightMap[x][z] = (uint8_t)finalHeight;


#endif
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

#ifdef TREES
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

#endif
	m_HasGenerated = true;
	m_IsGenerating = false;

	//SetDirty(true);

	// Set flag for adjacent chunks to rebuild to remove uneq. block faces at chunk border
	/*AdjacentChunks chunks = GetAdjacentChunks();
	if (chunks.West) chunks.West->SetDirty(true);
	if (chunks.North) chunks.North->SetDirty(true);
	if (chunks.East) chunks.East->SetDirty(true);
	if (chunks.South) chunks.South->SetDirty(true);*/

	//World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, this));

	//std::cout << "Terrain generatation done. " << m_Position.x << ", " << m_Position.y << "\n";

	//mutex.unlock();
}

void Chunk::GenerateTerrainThreaded(ChunkAction::Priority priority, ChunkAction* nextAction)
{
	//if (!m_IsInitialized) return;

	m_IsGenerating = true;

	ChunkBuilder::Get().AddToQueue(ChunkAction(ChunkAction::ActionType::Generate, this, priority, nextAction));
}

void Chunk::CreateAndGenerateTerrain(ChunkAction::Priority priority, ChunkAction* nextAction)
{
	m_IsGenerating = true;
	m_IsRebuilding = true; 

	ChunkBuilder::Get().AddToQueue(ChunkAction(ChunkAction::ActionType::CreateGenerateAndBuild, this, priority, nextAction));
}

void Chunk::GetDataNet()
{
	/*NetworkThread& net = net.Instance();

	json msg;
	msg["Type"] = "GetChunk";
	msg["Data"]["Position"]["X"] = m_Position.x;
	msg["Data"]["Position"]["Z"] = m_Position.y;*/

	//net.SendJson(msg);
}

void Chunk::GetDataNetThreaded(ChunkAction::Priority priority, ChunkAction* nextAction)
{

}

json Chunk::Serialize()
{
	json message;
	message["Type"] = "ChunkData";
	message["Data"]["Position"]["X"] = m_Position.x;
	message["Data"]["Position"]["Z"] = m_Position.y;

	message["Data"]["Blocks"] = json::array();
	json& blocks = message["Data"]["Blocks"];
	for (int x = 0; x < Chunk::Width; x++)
	{
		for (int y = 0; y < Chunk::Height; y++)
		{
			for (int z = 0; z < Chunk::Depth; z++)
			{
				int index = Utils::BlockPositionToIndex(glm::u8vec3(x, y, z));
				blocks[index] = GetBlockAt(glm::vec3(x, y, z))->m_BlockId;
			}
		}
	}

	return message;
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

	chunks.West = m_World->GetChunkAt(m_Position + glm::ivec2(-1, 0));
	chunks.East = m_World->GetChunkAt(m_Position + glm::ivec2(1, 0));
	chunks.South = m_World->GetChunkAt(m_Position + glm::ivec2(0, -1));
	chunks.North = m_World->GetChunkAt(m_Position + glm::ivec2(0, 1));

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