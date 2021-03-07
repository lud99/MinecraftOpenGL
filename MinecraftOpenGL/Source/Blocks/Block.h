#pragma once

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <Graphics/Textures/Texture2D.h>

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
	bool isTranslucent = false; // For example water. Has opacity but not no parts with 0 opacity
	bool isTransparent = false; // For example leaves. Has some parts which are fully see-through
	bool isOpaque = true; // Solid blocks. Neither transparent or translucent

	BlockFace faces[6];

	static constexpr int facesCount = 6;

	void SetTexture(int textureId)
	{
		for (int i = 0; i < facesCount; i++)
			faces[i].textureId = textureId;
	}
};