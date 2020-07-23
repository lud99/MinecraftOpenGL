#include "Utils.h"

#include <math.h>

#include "Chunk/Chunk.h";

int Utils::to1D(glm::vec3& position, int width, int height, int depth) {
	return position.x * width*width + position.y * width + position.z;
}

glm::vec3 Utils::to3D(int index, int width, int height, int depth) {
	int x = index / (width * width);
	int y = (index / width) % width;
	int z = index % width;

	return glm::vec3(x, y, z);
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