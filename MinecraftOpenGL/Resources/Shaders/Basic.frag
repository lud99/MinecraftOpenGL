#version 330 core

layout (location = 0) out vec4 color;

in vec3 outPosition;
in vec2 textureCoord;
in vec4 col;

uniform sampler2D tex;

const float BorderThickness = 0.025;

void main()
{
	vec4 textureColor = texture(tex, textureCoord);

	if (textureColor.a == 0.0)
		discard;

	color = textureColor;

	//if (mod(outPosition.x - 0.5, 1.0) > 1 - BorderThickness || mod(outPosition.x - 0.5, 1.0) < 0 + BorderThickness)
		//color = vec4(0.5, 0.5, 0.5, 1.0);

	//if (mod(outPosition.z - 0.5, 1.0) > 1 - BorderThickness || mod(outPosition.z - 0.5, 1.0) < 0 + BorderThickness)
		//color = vec4(0.5, 0.5, 0.5, 1.0);

	//if (mod(outPosition.y - 0.5, 1.0) < 1 - BorderThickness * 2&& mod(outPosition.y - 0.5, 1.0) > 0 + BorderThickness * 2)
		//color = textureColor;

}