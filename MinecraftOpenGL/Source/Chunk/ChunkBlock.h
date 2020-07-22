#pragma once

#include "../Mesh.h"

class Chunk;

class ChunkBlock : public Mesh
{
public:
	ChunkBlock();

	void AddBlockFace(const float* face, glm::vec3 position, const glm::vec4* colors);
	void AddBlockFaces(glm::vec3 localPosition, const glm::vec4* colors);
	void AddAllBlockFaces(glm::vec3 position, const glm::vec4* colors);

	Chunk* GetChunk();

	const glm::vec3 GetLocalPosition();
	const glm::vec3 GetWorldPosition();

	void SetLocalPosition(glm::vec3 position);
	void SetWorldPosition(glm::vec3 position);

	~ChunkBlock();

public:
	bool m_Hidden = false;

	unsigned int m_ChunkIndex = 0;

private:
	glm::vec3 m_LocalPosition;
};