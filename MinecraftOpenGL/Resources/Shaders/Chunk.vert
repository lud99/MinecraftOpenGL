#version 410 core

layout(location = 0) in uint packedData;

out vec4 gl_Position;
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
	uint x = (packedData >> 26)&((1 << 5) - 1);
	uint y = (packedData >> 17)&((1 << 9) - 1);
	uint z = (packedData >> 12)&((1 << 5) - 1);

	uint index = (packedData >> 10)&((1 << 2) - 1);
	uint textureIndex = (packedData >> 4)&((1 << 6) - 1);
	uint lightLevel = (packedData >> 0)&((1 << 4) - 1);

	vec3 position = vec3(x, y, z) + vec3(u_ChunkPosition.x, 0, u_ChunkPosition.y) + 0.0;

	gl_Position = u_MVP * vec4(position, 1.0);

	passTextureCoord = CalculateTextureCoordinates(textureIndex, index);
	passLightLevel = float(lightLevel) / 15;
	chunk = float(u_ChunkPosition.x * u_ChunkPosition.y);
}