#version 330 core

layout (location = 0) out vec4 color;

in vec2 passTextureCoord;

uniform sampler2D tex;

void main()
{
	color = texture(tex, passTextureCoord);

	if (color.a == 0.0) discard;
}