#pragma once

#include <glm/vec3.hpp>

#include <cstdint>

struct VertexData
{
	glm::u8vec3 position;
	uint8_t index;
	uint8_t texture;
	uint8_t lightLevel;
};

struct Vertex
{
	void SetData(VertexData vertexData);
	VertexData GetData();

	unsigned int packedData;

	static const int numBits = 8;
};