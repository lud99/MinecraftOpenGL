#version 330 core

layout (location = 0) out vec4 color;

in vec2 textureCoord;
in vec4 col;

uniform sampler2D tex;

void main()
{
	vec4 textureColor = texture(tex, textureCoord);

	if (textureColor.a == 0.0)
		discard;

	color = textureColor;
}