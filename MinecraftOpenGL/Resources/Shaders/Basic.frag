#version 330 core

layout (location = 0) out vec4 color;

in vec2 passTextureCoord;
in float passLightLevel;

uniform sampler2D tex;
uniform int u_Rebuilding = 0;
uniform int u_Generating = 0;

void main()
{
	vec4 textureColor = texture(tex, passTextureCoord);

	if (u_Rebuilding == 1)
		color = vec4(1, 0, 0, 1);
	else if (u_Generating == 1)
		color = vec4(0, 1, 0, 1);
	else
		color = textureColor * vec4(passLightLevel, passLightLevel, passLightLevel, 1);

	if (color.a == 0.0)
		discard;
}