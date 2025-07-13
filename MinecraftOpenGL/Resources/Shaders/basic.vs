#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

out vec4 passColor;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

void main()
{
	gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(position, 1.0);
	passColor = color;
}