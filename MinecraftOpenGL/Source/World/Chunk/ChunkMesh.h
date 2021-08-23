#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <Graphics/Mesh.hpp>

#include <mutex>
#include <unordered_map>
#include <vector>

#include <Common/Blocks/Blocks.h>
#include <Common/Chunk/ChunkBuilder.h>

class Chunk;
class ChunkBlock;

class ChunkMesh
{
public:
	ChunkMesh();

	void RebuildMesh();
	void RebuildMeshThreaded(ChunkAction::Priority priority, ChunkAction* nextAction = nullptr);

	void AddBlockFace(ChunkBlock* block, BlockFace& face);

	void Render();

	~ChunkMesh();

public:
	Chunk* m_Chunk = nullptr;
	
	Mesh<PackedVertex> m_OpaqueMesh;
	Mesh<PackedVertex> m_WaterMesh;
	Mesh<TextureVertex> m_ModelsMesh;

	Mesh<PackedVertex> m_TempOpaqueMesh;
	Mesh<PackedVertex> m_TempWaterMesh;

	std::mutex m_MeshMutex;
};