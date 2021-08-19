#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdint>

struct Vertex
{
	int data;
};

struct PackedVertex
{
	unsigned int data;
};

struct BasicVertex
{
	glm::vec3 position = glm::vec3(0);
};

struct PackedVertexData
{
	virtual unsigned int Pack() = 0;

	PackedVertex CreatePackedVertex();
};

struct TextureVertex
{
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec2 textureCoord = glm::vec2(0.0f, 0.0f);
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct BlockVertex : public PackedVertexData
{
	glm::u8vec3 position;
	uint8_t index;
	uint8_t texture;
	uint8_t lightLevel;

	unsigned int Pack() override;
	BlockVertex Unpack(unsigned int packedData);
};

struct DroppedItemVertex
{
	glm::vec3 position;
	uint8_t texture;
};