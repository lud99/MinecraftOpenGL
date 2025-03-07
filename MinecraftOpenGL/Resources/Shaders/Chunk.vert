#version 410 core

layout(location = 0) in vec3 localPosition;
layout(location = 1) in int index;
layout(location = 2) in int textureIndex;
layout(location = 3) in int lightLevel;
layout(location = 4) in vec3 normal;


out vec4 gl_Position;
out vec3 passWorldPos;
out vec3 passNormal;
out vec2 passTextureCoord;
out float passLightLevel;
out float chunk;

uniform mat4 u_MVP;

uniform ivec2 u_ChunkPosition;

const int TextureAtlasSize = 128;
const int TextureAtlasBlockSize = 16;
const int TextureAtlasBlockCountRow = TextureAtlasSize / TextureAtlasBlockSize;

const float BlockSize = 1.0 / TextureAtlasBlockCountRow;

vec2 CalculateTextureCoordinates(uint textureIndex, uint index)
{
	vec2 offset = vec2(textureIndex % 8, floor(textureIndex / 8));

	vec2 textureAtlasOffset = vec2(offset.x * BlockSize, 1.0 - BlockSize - (BlockSize * offset.y));

	switch(index) {
	case 0: return vec2(textureAtlasOffset.x, textureAtlasOffset.y);
	case 1: return vec2(textureAtlasOffset.x + BlockSize, textureAtlasOffset.y);
	case 2: return vec2(textureAtlasOffset.x + BlockSize, textureAtlasOffset.y + BlockSize);
	case 3: return vec2(textureAtlasOffset.x, textureAtlasOffset.y + BlockSize);
	}
}

void main() {
	vec3 position = localPosition + vec3(u_ChunkPosition.x, 0, u_ChunkPosition.y) + 0.0;

	gl_Position = u_MVP * vec4(position, 1.0);
	passWorldPos = position;

	passTextureCoord = CalculateTextureCoordinates(textureIndex, index);
	passLightLevel = float(lightLevel) / 15;
	passNormal = normal;
	chunk = float(u_ChunkPosition.x * u_ChunkPosition.y);
}