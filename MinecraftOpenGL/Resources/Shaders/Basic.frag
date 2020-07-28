#version 330 core

layout (location = 0) out vec4 color;

in vec2 textureCoord;
in vec4 col;
in float textureIndex;

const int TEXTURE_SIZE = 128;
const int BLOCK_SIZE = 1 / 16;

uniform sampler2D tex;

vec2 getTextureOffset() 
{
	return vec2(1 / (mod(textureIndex, TEXTURE_SIZE)), 1 / (mod(textureIndex, TEXTURE_SIZE)));
}

void main()
{
	vec2 pos = textureCoord;

	color = texture(tex, pos);
}