#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 vertexColor;

out vec4 gl_Position;
out vec4 fragmentColor;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

void main() {
	gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(position, 1.0);

	fragmentColor = vertexColor;
}