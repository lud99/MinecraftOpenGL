#version 410 core

layout(location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoord;

out vec4 gl_Position;
out vec2 passTextureCoord;

uniform mat4 u_ProjectionMatrix = mat4(1.0);
uniform float u_Scale = 0.025;

void main() {
	gl_Position = u_ProjectionMatrix * vec4(position.xy * u_Scale, -1.0, 1.0);

	passTextureCoord = textureCoord;
}