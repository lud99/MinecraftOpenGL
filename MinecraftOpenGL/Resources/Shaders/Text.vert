#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 textureCoords;

out vec2 TexCoords;

uniform mat4 u_ProjectionMatrix = mat4(1.0);
uniform mat4 u_ViewMatrix = mat4(1.0);
uniform mat4 u_ModelMatrix = mat4(1.0);
uniform vec2 u_Size = vec2(1.0, 1.0);

void main()
{
    gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * vec4(vertex.xy * u_Size, 0.0, 1.0);
    TexCoords = textureCoords;
} 