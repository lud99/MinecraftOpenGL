#include "TextureList.h"

using namespace TextureIds;

void BlockTexture::SetTextureAtlasOffset(glm::ivec2 offset)
{
	float blockSize = 1.0f / TextureList::BlockCountRow;

	textureAtlasOffset = glm::vec2(offset.x * blockSize, 1.0f - blockSize - (blockSize * offset.y));
}

void BlockTexture::CalculateCoordinates()
{
	float blockSize = 1.0f / TextureList::BlockCountRow;

	textureCoordinates[0] = glm::vec2(textureAtlasOffset.x, textureAtlasOffset.y);
	textureCoordinates[1] = glm::vec2(textureAtlasOffset.x + blockSize, textureAtlasOffset.y);
	textureCoordinates[2] = glm::vec2(textureAtlasOffset.x + blockSize, textureAtlasOffset.y + blockSize);
	textureCoordinates[3] = glm::vec2(textureAtlasOffset.x, textureAtlasOffset.y + blockSize);
}

void TextureList::Load()
{
	Atlas = new Texture2D("Resources/Images/textures.png");

	// Grass
	BlockTextures[GrassSide].SetTextureAtlasOffset(glm::ivec2(0, 0));
	BlockTextures[GrassSide].CalculateCoordinates();

	BlockTextures[GrassTop].SetTextureAtlasOffset(glm::ivec2(1, 0));
	BlockTextures[GrassTop].CalculateCoordinates();

	// Dirt
	BlockTextures[Dirt].SetTextureAtlasOffset(glm::ivec2(2, 0));
	BlockTextures[Dirt].CalculateCoordinates();
}

BlockTexture& TextureList::operator[] (int id)
{
	return BlockTextures[id];
}

BlockTexture TextureList::BlockTextures[Count];

Texture2D* TextureList::Atlas;