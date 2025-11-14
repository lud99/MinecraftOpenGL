#version 330 core

layout (location = 0) out vec4 color;

in vec4 passColor;

uniform float u_Time = 0;

void main()
{
	color = passColor;
	color.rb *= sin(u_Time * 1.3) * 0.3 + 1;
	color.g *= sin((u_Time + 9.8) * 1.3) * 0.1 + 1;
}