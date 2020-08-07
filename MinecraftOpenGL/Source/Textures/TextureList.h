#pragma once

#include "Texture2D.h"

#include <map>

#include <glm/vec2.hpp>

#include "TextureIds.h"

struct BlockTexture {
	glm::vec2 textureCoordinates[4] = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f)
	};

	glm::vec2 textureAtlasOffset;

	void SetTextureAtlasOffset(glm::vec2 offset);
	void CalculateCoordinates();
};

struct TextureList {
	static Texture2D Atlas;

	const static int TextureMapSize = 128;
	const static int BlockSize = 16;
	const static int BlockCountRow = TextureMapSize / BlockSize;

	static BlockTexture BlockTextures[TextureIds::Count];

	static void Load();
	
	BlockTexture& operator[] (int id);
};