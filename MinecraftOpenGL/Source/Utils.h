#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Utils
{
	int to1D(glm::vec3& position, int width, int height, int depth);
	int to1D(glm::vec2 position, int width);
	glm::vec3 to3D(int index, int width, int height, int depth);
	glm::vec2 to2D(int index, int width);

	int ChunkPositionToIndex(glm::vec3 position);
	glm::vec3 ChunkIndexToPosition(int index);

	glm::vec3 WorldPositionToChunkPosition(glm::vec3 position);
}