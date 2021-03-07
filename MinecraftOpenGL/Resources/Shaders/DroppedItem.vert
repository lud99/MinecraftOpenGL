#version 330 core

layout (location = 0) in vec3 Position;

out vec4 gl_Position;
out vec2 TexCoords;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ModelMatrix;

void main()
{
	vec4 pos = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * vec4(Position, 1.0);

    TexCoords = vec2(0.0, 0.0);
    gl_Position = pos;
}  