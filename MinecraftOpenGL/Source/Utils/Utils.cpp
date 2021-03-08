#include "Utils.h"

#include <math.h>

#include "../World/Chunk/Chunk.h"
/*
ChunkPosition::ChunkPosition()
{
	x = 0;
	z = 0;
}
ChunkPosition::ChunkPosition(uint32_t _x, uint32_t _z)
{
	x = _x;
	z = _z;
}
ChunkPosition::ChunkPosition(glm::ivec2 vec)
{
	x = vec.x;
	z = vec.y;
}

bool ChunkPosition::operator==(const ChunkPosition& other) const
{
	return x == other.x && z == other.z;
}

ChunkPosition::~ChunkPosition()
{

}

std::size_t std::hash<ChunkPosition>::operator()(ChunkPosition const& k) const noexcept
{
	// TODO: x and y can not be larger than 65535 (sizeof uint_16)

			// Convert the positions to be positive
	unsigned int x = k.x + INT32_MAX;
	unsigned int z = k.z + INT32_MAX;

	unsigned int key = x * UINT16_MAX + z;

	return key;
	//return std::hash<int>()(k.x) ^ std::hash<int>()(k.y);
}

//namespace std
//{
//	template<>
//	struct hash<ChunkPosition>
//	{
//		std::size_t operator()(ChunkPosition const& k) const noexcept
//		{
//			// TODO: x and y can not be larger than 65535 (sizeof uint_16)
//
//			// Convert the positions to be positive
//			unsigned int x = k.x + INT32_MAX;
//			unsigned int z = k.z + INT32_MAX;
//
//			unsigned int key = x * UINT16_MAX + z;
//
//			return key;
//			//return std::hash<int>()(k.x) ^ std::hash<int>()(k.y);
//		}
//	};
//}
*/
int Utils::to1D(glm::vec3& position, int width, int height) {
	return position.x + position.y * width + position.z * width * height;//(int)(position.x * width * width + position.y * width + position.z);
}

int Utils::to1D(glm::ivec2 position, int width) {
	return position.x * width + position.y;
}

glm::vec3 Utils::to3D(int index, int width, int height, int depth) {
	int x = index % width;
	int y = (index / width) % height;
	int z = index / (width * height);

	return glm::vec3(x, y, z);
}

glm::vec2 Utils::to2D(int index, int width) {
	/*int x = index % width;
	int y = index / width;*/

	int y = index % width;
	int x = (index - y) / width;

	return glm::vec2(x, y);
}

int Utils::ChunkPositionToIndex(glm::ivec2 position)
{
	int a = to1D(position, Chunk::Width);

	if (a == -1)
		return a;

	return a;
}

glm::vec3 Utils::ChunkIndexToPosition(int index)
{
	return to3D(index, Chunk::Width, Chunk::Height, Chunk::Depth);
}

uint16_t Utils::BlockPositionToIndex(glm::u8vec3 position)
{
	return position.x + position.y * Chunk::Width + position.z * Chunk::Width * Chunk::Height;
}

glm::u8vec3 Utils::BlockIndexToPosition(uint16_t index)
{
	uint8_t x = index % Chunk::Width;
	uint8_t y = (index / Chunk::Width) % Chunk::Height;
	uint8_t z = index / (Chunk::Width * Chunk::Height);

	return glm::u8vec3(x, y, z);
}

glm::ivec2 Utils::WorldPositionToChunkPosition(glm::vec3 position)
{
	return glm::ivec2(
		floor(position.x / Chunk::Width),
		floor(position.z / Chunk::Depth)
	);
}

glm::u8vec3 Utils::WorldBlockPositionToLocalBlockPosition(glm::ivec3 blockPosition, glm::ivec2 chunkPosition)
{
	return glm::u8vec3(
		blockPosition.x - (chunkPosition.x * Chunk::Width),
		blockPosition.y,
		blockPosition.z - (chunkPosition.y * Chunk::Depth)
	);
}