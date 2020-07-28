#include "Utils.h"

#include <math.h>

#include "Chunk/Chunk.h";

int Utils::to1D(glm::vec3& position, int width, int height, int depth) {
	return position.x * width*width + position.y * width + position.z;
}

int Utils::to1D(glm::vec2 position, int width) {
	return position.x + width * position.y;
}

glm::vec3 Utils::to3D(int index, int width, int height, int depth) {
	int x = index / (width * width);
	int y = (index / width) % width;
	int z = index % width;

	return glm::vec3(x, y, z);
}

glm::vec2 Utils::to2D(int index, int width) {
	int x = index % width;
	int y = index / width;

	return glm::vec2(x, y);
}

int Utils::ChunkPositionToIndex(glm::vec3 position)
{
	return to1D(position, Chunk::Width, Chunk::Height, Chunk::Depth);
}

glm::vec3 Utils::ChunkIndexToPosition(int index)
{
	return to3D(index, Chunk::Width, Chunk::Height, Chunk::Depth);
}

glm::vec3 Utils::WorldPositionToChunkPosition(glm::vec3 position)
{
	glm::vec3 chunkPosition(
		floor(position.x / Chunk::Width),
		floor(position.y / Chunk::Height),
		floor(position.z / Chunk::Depth)
	);

	return chunkPosition;
}