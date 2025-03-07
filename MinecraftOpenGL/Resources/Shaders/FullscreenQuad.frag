#version 410 core

in vec2 texcoords;

out vec4 color;

uniform sampler2D tex;


void main()
{

	float depth = texture(tex, texcoords).r;
	color = vec4(vec3(depth), 1.0);

	//vec4(texcoords.x, texcoords.y, 1.0, 1.0);//
}