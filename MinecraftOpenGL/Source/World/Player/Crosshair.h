#pragma once

#include "../../Graphics/Mesh.hpp"
#include "../../Graphics/Shaders/Shader.h"

class WorldRenderer;
class Texture2D;

class Crosshair
{
public:
	Crosshair();

	void Init();

	void Render(WorldRenderer* worldRenderer);

	~Crosshair();

public:
	Mesh<TextureVertex> m_Mesh;
	Texture2D* m_Texture;
	Shader m_Shader;
};