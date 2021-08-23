#include "ChunkMesh.h"

#include <iostream>
#include <algorithm>
#include <thread>

#include <Common/World/IWorld.h>

#include <Common/ThreadPool.h>
#include <Common/Chunk/Chunk.h>
#include <Common/Chunk/ChunkBlock.h>
#include <Common/Noise/FastNoise.h>
#include <Graphics/BasicVertices.h>

ChunkMesh::ChunkMesh()
{
	m_OpaqueMesh.m_Texture = TextureAtlas::Get().Texture;
	m_WaterMesh.m_Texture = TextureAtlas::Get().Texture;
}

void ChunkMesh::RebuildMesh()
{
	//SetDirty(true);
	m_Chunk->m_IsRebuilding = true;

	m_MeshMutex.lock();

	m_TempOpaqueMesh.Clear();
	m_TempWaterMesh.Clear();

	for (int x = 0; x < Chunk::Width; x++)
	{
		for (int y = 0; y < Chunk::Height; y++)
		{
			for (int z = 0; z < Chunk::Depth; z++)
			{
				ChunkBlock* block = m_Chunk->GetBlockAt(glm::ivec3(x, y, z));

				// Add all the faces on the cube
				block->AddBlockFaces(m_Chunk);
			}
		}
	}

	// Iterate through all block entities. TODO enable
	for (auto& entry : m_Chunk->m_BlockEntities)
	{
		//BlockEntity* entity = entry.second;

		//// Add all the faces on the cube
		//entity->AddBlockFaces();
	}

	m_OpaqueMesh.SetVertices(m_TempOpaqueMesh.GetVertices());
	m_OpaqueMesh.SetIndices(m_TempOpaqueMesh.GetIndices());
	m_WaterMesh.SetVertices(m_TempWaterMesh.GetVertices());
	m_WaterMesh.SetIndices(m_TempWaterMesh.GetIndices());

	m_TempOpaqueMesh.Clear();
	m_TempWaterMesh.Clear();

	// Set the adjacent chunks
	m_Chunk->m_AdjacentChunksWhenLastRebuilt = m_Chunk->GetAdjacentChunks();

	//std::cout << "Chunk mesh rebuilt. " << m_Position.x << ", " << m_Position.y << "\n";

	m_Chunk->m_IsRebuilding = false;
	m_Chunk->SetDirty(false);

	m_MeshMutex.unlock();
}

void ChunkMesh::RebuildMeshThreaded(ChunkAction::Priority priority, ChunkAction* nextAction)
{
	if (!m_Chunk->m_IsInitialized) std::cout << "Not int\n";

	ChunkBuilder::Get().AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, m_Chunk, priority, nextAction));
}

void ChunkMesh::AddBlockFace(ChunkBlock* block, BlockFace& face)
{
	glm::vec3 worldPosition = block->GetWorldPosition(m_Chunk);
	BlockType* blockType = block->GetBlockType();

	Mesh<PackedVertex>& opaqueMesh = m_TempOpaqueMesh;
	Mesh<PackedVertex>& waterMesh = m_TempWaterMesh;

	for (int i = 0; i < 4; i++)
	{
		glm::u8vec3 position((glm::u8vec3)face.positions[i] + block->GetLocalPosition());

		int lightLevel = 15;

		switch (face.direction)
		{
		case Directions::Top: lightLevel = 15; break;
		case Directions::West: lightLevel = 12; break;
		case Directions::East: lightLevel = 12; break;
		case Directions::North: lightLevel = 9; break;
		case Directions::South: lightLevel = 9; break;
		case Directions::Bottom: lightLevel = 6; break;
		}

		BlockVertex vertex;

		vertex.position = position;
		vertex.index = i;
		vertex.texture = face.textureId;
		vertex.lightLevel = lightLevel;

		if (blockType->isOpaque)
			m_TempOpaqueMesh.AddVertex(vertex.CreatePackedVertex());
		else
			m_TempWaterMesh.AddVertex(vertex.CreatePackedVertex());
	}

	for (int i = 0; i < 6; i++)
	{
		if (blockType->isOpaque)
			m_TempOpaqueMesh.AddIndex(BasicVertices::Cube::Indices[i] + (uint16_t)(m_TempOpaqueMesh.GetVertices().size() - 4));
		else
			m_TempWaterMesh.AddIndex(BasicVertices::Cube::Indices[i] + (uint16_t)(m_TempWaterMesh.GetVertices().size() - 4));
	}
}

void ChunkMesh::Render()
{
	m_OpaqueMesh.Render();
	m_WaterMesh.Render();
}


ChunkMesh::~ChunkMesh()
{

}