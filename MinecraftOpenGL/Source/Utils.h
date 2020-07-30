#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Utils
{
	int to1D(glm::vec3& position, int width);
	int to1D(glm::ivec2 position, int width);
	glm::vec3 to3D(int index, int width, int height, int depth);
	glm::vec2 to2D(int index, int width);

	int ChunkPositionToIndex(glm::ivec2 position);
	glm::vec3 ChunkIndexToPosition(int index);

	glm::vec3 WorldPositionToChunkPosition(glm::vec3 position);
}