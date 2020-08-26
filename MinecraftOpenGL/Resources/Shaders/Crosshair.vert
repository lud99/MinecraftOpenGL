#version 410 core

layout(location = 0) in vec3 position;

out vec4 gl_Position;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

void main() {
	gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(position, 1.0);
}