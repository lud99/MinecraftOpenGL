#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Chunk;

class ChunkBlock
{
public:
	ChunkBlock();

	void AddBlockFace(const float* face, const glm::vec4* colors);
	void AddBlockFaces(const glm::vec4* colors);
	void AddAllBlockFaces(const glm::vec4* colors);

	Chunk* GetChunk();

	const glm::vec3 GetLocalPosition();
	const glm::vec3 GetWorldPosition();

	void SetLocalPosition(glm::vec3 position);
	void SetWorldPosition(glm::vec3 position);

	~ChunkBlock();

public:
	bool ChunkExistsAtRelativePosition(glm::vec3 offset);

public:
	bool m_Hidden = false;

	unsigned int m_ChunkIndex = 0;

private:
	glm::vec3 m_LocalPosition;
};