#version 330 core

layout (location = 0) out vec4 color;

in vec2 passTextureCoord;
in float passLightLevel;
in float chunk;

uniform sampler2D tex;
uniform int u_Rebuilding = 0;
uniform int u_ShouldBeRemoved = 0;

void main()
{
	vec4 textureColor = texture(tex, passTextureCoord);

	float c = chunk;
	if (chunk != 0)
		c = (1 / chunk) * 100.0;
	else
		c += 0.5;

	//color = mix(textureColor, vec4(c, c, c, 1.0), 0.5);

	if (u_Rebuilding == 1)
		color = vec4(0, 1, 0, 1);
	if (u_ShouldBeRemoved == 1)
		color = vec4(1, 0, 0, 1);
	if (u_Rebuilding == 0 && u_ShouldBeRemoved == 0)
		color = textureColor * vec4(passLightLevel, passLightLevel, passLightLevel, 1);
		
	if (color.a == 0.0)
		discard;
}