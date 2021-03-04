#include "TextureAtlas.h"

#include <iostream>
using namespace TextureIds;

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

	// Grass
	/*BlockTextures[GrassSide].SetTextureAtlasOffset(glm::vec2(0, 0));
	BlockTextures[GrassSide].CalculateCoordinates();

	BlockTextures[GrassTop].SetTextureAtlasOffset(glm::vec2(1, 0));
	BlockTextures[GrassTop].CalculateCoordinates();

	// Dirt
	BlockTextures[Dirt].SetTextureAtlasOffset(glm::ivec2(2, 0));
	BlockTextures[Dirt].CalculateCoordinates();

	// Stone
	BlockTextures[Stone].SetTextureAtlasOffset(glm::ivec2(3, 0));
	BlockTextures[Stone].CalculateCoordinates();

	// Water
	BlockTextures[Water].SetTextureAtlasOffset(glm::ivec2(4, 0));
	BlockTextures[Water].CalculateCoordinates();

	// Gravel
	BlockTextures[Gravel].SetTextureAtlasOffset(glm::ivec2(5, 0));
	BlockTextures[Gravel].CalculateCoordinates();

	// Sand
	BlockTextures[Sand].SetTextureAtlasOffset(glm::ivec2(6, 0));
	BlockTextures[Sand].CalculateCoordinates();

	// Oak log
	BlockTextures[OakLogSide].SetTextureAtlasOffset(glm::ivec2(7, 0));
	//BlockTextures[OakLogSide].CalculateCoordinates();
	BlockTextures[OakLogTop].SetTextureAtlasOffset(glm::ivec2(0, 1));
	//BlockTextures[OakLogTop].CalculateCoordinates();

	// Oak leaves
	BlockTextures[OakLeaves].SetTextureAtlasOffset(glm::ivec2(1, 1));
	BlockTextures[OakLeaves].CalculateCoordinates();

	BlockTextures[Chest].SetTextureAtlasOffset(glm::ivec2(0, 0));
	BlockTextures[Chest].CalculateCoordinates();*/
}

BlockTexture& TextureAtlas::operator[] (int id)
{
	return BlockTextures[id];
}

TextureAtlas::~TextureAtlas()
{
	delete Texture;
}

BlockTexture TextureAtlas::BlockTextures[Count];

Texture2D* TextureAtlas::Texture;