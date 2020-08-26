#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../../Graphics/Mesh.h"
#include "ChunkRebuilder.h"

class ChunkBlock;
class ChunkMesh;

class Chunk
{
public:
	Chunk(glm::ivec2 position);

	void RebuildMesh();

	void Render();

	void Fill(const glm::vec4* colors);
	void CreateSphere(const glm::vec4* colors);
	void GenerateTerrain();

	void SetBlockAt(glm::ivec3 position, ChunkBlock* newBlock);
	void SetBlockAt(glm::vec3 position, ChunkBlock* newBlock);

	ChunkBlock*** GetAllBlocks();
	ChunkBlock* GetBlockAt(glm::ivec3 position);
	ChunkBlock* GetBlockAt(glm::vec3 position);

	bool BlockExistsAt(glm::vec3 localPosition);

	glm::ivec2& GetPosition();
	glm::ivec2 GetWorldPosition();
	void SetPosition(glm::ivec2 position);

	int GetIndex();

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

	static ChunkRebuilder m_Rebuilder;

private:
	glm::ivec2 m_Position = glm::ivec2(0, 0);

	int m_Index = 0;
};