#pragma once

#include <glm/vec3.hpp>

namespace Utils
{
	int to1D(glm::vec3& position, int xMax, int yMax);
	glm::vec3 to3D(int index, int xMax, int yMax);

	int ChunkPositionToIndex(glm::vec3& position);
	glm::vec3 ChunkIndexToPosition(int index);
}