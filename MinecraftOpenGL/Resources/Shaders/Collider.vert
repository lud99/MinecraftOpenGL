#version 410 core

layout(location = 0) in vec3 position;

out vec4 gl_Position;

uniform mat4 u_ProjectionMatrix = mat4(1.0);
uniform mat4 u_ViewMatrix = mat4(1.0);
uniform mat4 u_ModelMatrix = mat4(1.0);

void main() 
{
	gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * vec4(position, 1.0);
}