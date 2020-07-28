#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in float texIndex;

out vec4 gl_Position;
out vec2 textureCoord;
out vec4 col;
out float textureIndex;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

void main() {
	gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(position, 1.0);

	textureCoord = texCoord;
	col = vertexColor;
	textureIndex = texIndex;
}