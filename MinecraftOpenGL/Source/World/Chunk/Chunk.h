#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "../../Graphics/Mesh.h"

#include <mutex>

class ChunkBlock;
class ChunkMesh;

class Chunk
{
public:
	Chunk(glm::ivec2 position);

	void RebuildMesh();
	void RebuildMeshThreaded();

	void Render();

	void Fill(const glm::vec4* colors);
	void CreateSphere(const glm::vec4* colors);
	void GenerateTerrain();
	void GenerateTerrainThreaded();

	void SetBlockAt(glm::ivec3 position, ChunkBlock* newBlock);
	void SetBlockAt(glm::vec3 position, ChunkBlock* newBlock);

	ChunkBlock*** GetAllBlocks();
	ChunkBlock* GetBlockAt(glm::u8vec3 position);
	ChunkBlock* GetBlockAt(glm::ivec3 position);
	ChunkBlock* GetBlockAt(glm::vec3 position);

	bool BlockExistsAt(glm::vec3 localPosition);

	glm::ivec2& GetPosition();
	glm::ivec2 GetWorldPosition();
	void SetPosition(glm::ivec2 position);

	~Chunk();

public:
	static const int Width = 16;
	static const int Height = 128;
	static const int Depth = 16;

	static const int BlockCount = Width * Height * Depth;

	bool m_HasGenerated = false;

	ChunkBlock*** m_Blocks;

	Mesh m_OpaqueMesh;
	Mesh m_WaterMesh;

	Mesh m_TempOpaqueMesh;
	Mesh m_TempWaterMesh;

	std::mutex m_MeshMutex;

private:
	glm::ivec2 m_Position = glm::ivec2(0, 0);
};