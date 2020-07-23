#pragma once

#include <glm/vec3.hpp>

namespace Utils
{
	int to1D(glm::vec3& position, int width, int height, int depth);
	glm::vec3 to3D(int index, int width, int height, int depth);

	int ChunkPositionToIndex(glm::vec3 position);
	glm::vec3 ChunkIndexToPosition(int index);

	glm::vec3 WorldPositionToChunkPosition(glm::vec3 position);
}