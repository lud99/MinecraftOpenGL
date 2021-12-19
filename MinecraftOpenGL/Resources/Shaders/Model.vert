#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoord;

out vec4 gl_Position;
out vec2 passTextureCoord;

uniform mat4 u_MVP = mat4(1.0);

void main() 
{
	gl_Position = u_MVP * vec4(position, 1.0);
	passTextureCoord = textureCoord;
}