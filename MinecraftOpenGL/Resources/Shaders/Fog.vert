#version 410 core

layout(location = 0) in uint packedData;

out vec4 gl_Position;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

const int windowWidth = 1280, windowHeight = 720;

void main() {
	uint x = (packedData >> 26)&((1 << 5) - 1);
	uint y = (packedData >> 17)&((1 << 9) - 1);
	uint z = (packedData >> 12)&((1 << 5) - 1);

	uint index = (packedData >> 10)&((1 << 2) - 1);
	uint textureIndex = (packedData >> 4)&((1 << 6) - 1);
	uint lightLevel = (packedData >> 0)&((1 << 4) - 1);

	vec3 position = vec3(x, y, z);

	

	gl_Position = u_ProjectionMatrix * vec4((position.xy - 0.5) * vec2(windowWidth, windowHeight), -1.0, 1.0);
}