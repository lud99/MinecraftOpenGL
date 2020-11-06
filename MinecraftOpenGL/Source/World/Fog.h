#pragma once

#include <glm/vec3.hpp>

#include "../Graphics/Mesh.h"
#include "../Graphics/Shaders/Shader.h"

class WorldRenderer;

class Fog
{
public:
	Fog();

	void Init();

	void Render(WorldRenderer* worldRenderer);

	~Fog();

public:
	Mesh m_Mesh;

	Shader m_Shader;
};