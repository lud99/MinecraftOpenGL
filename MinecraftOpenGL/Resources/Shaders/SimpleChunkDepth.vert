#version 410 core

layout (location = 0) in vec3 localPosition;

uniform ivec2 u_ChunkPosition;
uniform mat4 u_VP;

void main()
{
	vec3 position = localPosition + vec3(u_ChunkPosition.x, 0, u_ChunkPosition.y) + 0.0;

    gl_Position = u_VP * vec4(position, 1.0);
}  