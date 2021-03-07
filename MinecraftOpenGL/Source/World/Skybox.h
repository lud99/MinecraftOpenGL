#pragma once

#include <Graphics/Mesh.hpp>
#include <Graphics/Shader.h>
#include <Graphics/Textures/Cubemap.h>

class WorldRenderer;

class Skybox
{
public:
	Skybox();

	void Init();

	void Render(WorldRenderer* worldRenderer);

	~Skybox();

public:
	Mesh<BasicVertex> m_Mesh;
	Cubemap m_Cubemap;
	Shader m_Shader;

	unsigned int m_Vbo = 0;
	unsigned int m_Vao = 0;

};