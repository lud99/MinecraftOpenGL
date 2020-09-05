#version 330 core

layout (location = 0) out vec4 color;

in vec2 passTextureCoord;
in float passLightLevel;

uniform sampler2D tex;

void main()
{
	vec4 textureColor = texture(tex, passTextureCoord);

	color = textureColor * vec4(passLightLevel, passLightLevel, passLightLevel, 1);

	if (color.a == 0.0)
		discard;
}