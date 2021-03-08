#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <cstdint>

#include <algorithm>
#include <iostream>

/*struct ChunkPosition
{
	ChunkPosition(const ChunkPosition&) = default;
	ChunkPosition& operator=(const ChunkPosition&) = default;

	ChunkPosition();
	ChunkPosition(uint32_t x, uint32_t z);
	ChunkPosition(glm::ivec2 vec);

	int32_t x;
	int32_t z;

	bool operator==(const ChunkPosition& other) const;

	~ChunkPosition();
};

namespace std
{
	template<>
	struct hash<ChunkPosition>
	{
		std::size_t operator()(ChunkPosition const& k) const noexcept;
	};
}*/

namespace Utils
{
	int to1D(glm::vec3& position, int width, int height);
	int to1D(glm::ivec2 position, int width);
	glm::vec3 to3D(int index, int width, int height, int depth);
	glm::vec2 to2D(int index, int width);

	int ChunkPositionToIndex(glm::ivec2 position);
	glm::vec3 ChunkIndexToPosition(int index);

	uint16_t BlockPositionToIndex(glm::u8vec3 position);
	glm::u8vec3 BlockIndexToPosition(uint16_t index);

	glm::ivec2 WorldPositionToChunkPosition(glm::vec3 position);
	glm::u8vec3 WorldBlockPositionToLocalBlockPosition(glm::ivec3 blockPosition, glm::ivec2 chunkPosition);

	inline void PrintVector(glm::vec3 vector) { std::cout << "x: " << vector.x << ", y: " << vector.y << ", z: " << vector.z << "\n"; }

	namespace Math
	{
		inline float Clamp(float value, float min, float max) { return std::max(min, std::min(value, max)); }
		inline int Sign(float x) { return (x > 0) - (x < 0); }
		inline float Lerp(float a, float b, float f) { return a + f * (b - a); };
 	}
}