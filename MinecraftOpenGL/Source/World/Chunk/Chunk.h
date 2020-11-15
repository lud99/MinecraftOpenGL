#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "../../Graphics/Mesh.hpp"

#include <mutex>

class ChunkBlock;
class ChunkMesh;

struct ChunkAction;
class Chunk;

struct AdjacentChunks
{
	Chunk* West = nullptr;
	Chunk* East = nullptr;
	Chunk* Top = nullptr;
	Chunk* Bottom = nullptr;
	Chunk* South = nullptr;
	Chunk* North = nullptr;
};

class Chunk
{
public:
	Chunk(glm::ivec2 position);

	void RebuildMesh();
	void RebuildMeshThreaded(ChunkAction* nextAction = nullptr);
	void GenerateTerrain();
	void GenerateTerrainThreaded(ChunkAction* nextAction = nullptr);

	void Fill(const glm::vec4* colors);
	void CreateSphere(const glm::vec4* colors);

	void Render();

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

	AdjacentChunks GetAdjacentChunks();

	~Chunk();

public:
	static const int Width = 16;
	static const int Height = 256;
	static const int Depth = 16;

	static const int BlockCount = Width * Height * Depth;

	bool m_HasGenerated = false;
	bool m_IsGenerating = false;
	bool m_ShouldRebuild = false;
	bool m_ShouldBeRemoved = false;
	bool m_IsRebuilding = false;

	AdjacentChunks m_AdjacentChunksWhenLastRebuilt;

	ChunkBlock*** m_Blocks;

	Mesh<PackedVertex> m_OpaqueMesh;
	Mesh<PackedVertex> m_WaterMesh;

	Mesh<PackedVertex> m_TempOpaqueMesh;
	Mesh<PackedVertex> m_TempWaterMesh;

	std::mutex m_MeshMutex;

private:
	glm::ivec2 m_Position = glm::ivec2(0, 0);
};