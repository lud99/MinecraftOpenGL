#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <algorithm>

namespace Utils
{
	int to1D(glm::vec3& position, int width);
	int to1D(glm::ivec2 position, int width);
	glm::vec3 to3D(int index, int width, int height, int depth);
	glm::vec2 to2D(int index, int width);

	int ChunkPositionToIndex(glm::ivec2 position);
	glm::vec3 ChunkIndexToPosition(int index);

	glm::ivec2 WorldPositionToChunkPosition(glm::vec3 position);

	namespace Math
	{
		inline float Clamp(float value, float min, float max) { return std::max(min, std::min(value, max)); }
 	}
}