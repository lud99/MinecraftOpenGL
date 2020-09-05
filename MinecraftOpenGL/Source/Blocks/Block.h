#pragma once

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../Graphics/Mesh.h"

#include "../Graphics/Textures/Texture2D.h"

struct BlockFace
{
	const glm::u8vec3* positions;

	int direction;

	int textureId;
};

struct Block
{
	const std::string name;
	int id;
	bool isTransparent = false;

	BlockFace faces[6];

	static constexpr int facesCount = 6;

	void SetTexture(int textureId)
	{
		for (int i = 0; i < facesCount; i++)
			faces[i].textureId = textureId;
	}
};