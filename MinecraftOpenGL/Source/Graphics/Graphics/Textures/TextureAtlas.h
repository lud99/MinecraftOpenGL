#pragma once

#include "Texture2D.h"

#include <map>

#include <glm/vec2.hpp>

//class BlockTexture 
//{
//public:
//	void SetTextureAtlasOffset(glm::vec2 offset);
//	void CalculateCoordinates();
//
//public:
//	glm::vec2 textureCoordinates[4] = {
//		glm::vec2(0.0f, 0.0f),
//		glm::vec2(1.0f, 0.0f),
//		glm::vec2(1.0f, 1.0f),
//		glm::vec2(0.0f, 1.0f)
//	};
//
//	glm::vec2 textureAtlasOffset;
//};

class TextureAtlas 
{
public:
	static TextureAtlas& Get();

	void Load();

	//BlockTexture& operator[] (int id);

	~TextureAtlas();

private:
	TextureAtlas();

public:
	Texture2D* Texture = nullptr;

	const int Size = 128;
	const int BlockSize = 16;
	const int BlockCountRow = Size / BlockSize;

	//static BlockTexture BlockTextures[1];
};