#include "Utils.h"

#include "Chunk/Chunk.h";

int Utils::to1D(glm::vec3& position, int xMax, int yMax) {
	return (position.z * xMax * yMax) + (position.y * xMax) + position.x;
}

glm::vec3 Utils::to3D(int index, int xMax, int yMax) {
	int z = index / (xMax * yMax);

	index -= (z * xMax * yMax);

	int y = index / xMax;
	int x = index % xMax;

	return glm::vec3(x, y, z);
}

int Utils::ChunkPositionToIndex(glm::vec3& position)
{
	return to1D(position, Chunk::Width, Chunk::Height);
}

glm::vec3 Utils::ChunkIndexToPosition(int index)
{
	return to3D(index, Chunk::Width, Chunk::Height);
}