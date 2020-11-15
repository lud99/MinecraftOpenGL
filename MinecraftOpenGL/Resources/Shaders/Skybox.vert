#version 330 core

layout (location = 0) in vec3 Position;

out vec3 TexCoords;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

void main()
{
	vec4 pos = u_ProjectionMatrix * u_ViewMatrix * vec4(Position, 1.0);

    TexCoords = Position;
    gl_Position = pos.xyww;
}  