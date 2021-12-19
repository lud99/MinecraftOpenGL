#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <Graphics/Mesh.hpp>

#include <Common/Blocks/BlockIds.h>

#include <mutex>
#include <unordered_map>
#include <vector>

#include <Common/Blocks/Blocks.h>
#include <Common/Chunk/ChunkBuilder.h>

class Chunk;
class ChunkBlock;

typedef std::unordered_map<uint8_t, Mesh<TextureVertex>> ModelsMeshMap;

class ChunkMesh
{
public:
	ChunkMesh();

	void RebuildMesh();
	void RebuildMeshThreaded(ChunkAction::Priority priority, ChunkAction* nextAction = nullptr);

	void AddBlockFace(ChunkBlock* block, BlockFace& face);
	void AddModel(ChunkBlock* block);

	void Render();

	~ChunkMesh();

public:
	Chunk* m_Chunk = nullptr;
	
	Mesh<PackedVertex> m_OpaqueMesh;
	Mesh<PackedVertex> m_WaterMesh;

	// Every block model is stored by their block ids. All the models use different textures, and therefore need to be rendered seperately
	ModelsMeshMap m_ModelsMesh;
	ModelsMeshMap m_TempModelsMesh;

	static ModelsMeshMap m_BlockModels;
	static bool m_HaveLoadedModels;

	Mesh<PackedVertex> m_TempOpaqueMesh;
	Mesh<PackedVertex> m_TempWaterMesh;

	std::mutex m_MeshMutex;
};