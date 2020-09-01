#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in float texIndex;

out vec4 gl_Position;
out vec3 outPosition;
out vec2 textureCoord;
out vec4 col;
out float textureIndex;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

uniform ivec2 u_ChunkPosition;

void main() {
	vec3 pos = position + vec3(u_ChunkPosition.x, 1, u_ChunkPosition.y);

	gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(pos, 1.0);

	outPosition = position;

	textureCoord = texCoord;
	col = vertexColor;
	textureIndex = texIndex;
}