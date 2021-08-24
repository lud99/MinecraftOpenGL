#include "TextureAtlas.h"

#include <iostream>
// namespace TextureIds;

void BlockTexture::SetTextureAtlasOffset(glm::vec2 offset)
{
	float blockSize = 1.0f / TextureAtlas::BlockCountRow;

	textureAtlasOffset = glm::vec2(offset.x * blockSize, 1.0f - blockSize - (blockSize * offset.y));
}

void BlockTexture::CalculateCoordinates()
{
	float blockSize = 1.0f / TextureAtlas::BlockCountRow;

	textureCoordinates[0] = glm::vec2(textureAtlasOffset.x, textureAtlasOffset.y);
	textureCoordinates[1] = glm::vec2(textureAtlasOffset.x + blockSize, textureAtlasOffset.y);
	textureCoordinates[2] = glm::vec2(textureAtlasOffset.x + blockSize, textureAtlasOffset.y + blockSize);
	textureCoordinates[3] = glm::vec2(textureAtlasOffset.x, textureAtlasOffset.y + blockSize);
}

void TextureAtlas::Load()
{
	Texture = new Texture2D("Resources/Images/textures.png");
}

BlockTexture& TextureAtlas::operator[] (int id)
{
	return BlockTextures[id];
}

TextureAtlas::~TextureAtlas()
{
	delete Texture;
}

BlockTexture TextureAtlas::BlockTextures[1];

Texture2D* TextureAtlas::Texture;