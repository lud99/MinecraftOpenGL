#pragma once

#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "../Mesh.h"

class ChunkBlock;
class ChunkMesh;

class Chunk : public Mesh
{
public:
	Chunk(glm::vec3 position);

	void UpdateMesh();

	void Fill(const glm::vec4* colors);

	void SetBlockAt(glm::ivec3 position, ChunkBlock* newBlock);
	void SetBlockAt(glm::vec3 position, ChunkBlock* newBlock);

	ChunkBlock*** GetAllBlocks();
	ChunkBlock* GetBlockAt(glm::ivec3 position);
	ChunkBlock* GetBlockAt(glm::vec3 position);

	glm::vec3 GetPosition();
	void SetPosition(glm::vec3 position);

	int GetIndex();

	~Chunk();

public:
	static const int Width = 16;
	static const int Height = 256;
	static const int Depth = 16;

	static const int BlockCount = Width * Height * Depth;

	ChunkBlock*** m_Blocks;

private:
	glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);

	unsigned int m_Index;
};