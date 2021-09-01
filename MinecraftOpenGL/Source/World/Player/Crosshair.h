#pragma once

#include <Graphics/Mesh.hpp>
#include <Graphics/Shader.h>

class WorldRenderer;
class Texture2D;
class ClientPlayer;

class Crosshair
{
public:
	Crosshair();

	void Init();

	void Render();

	~Crosshair();

public:
	Mesh<TextureVertex> m_Mesh;
	Texture2D* m_Texture;
	Shader m_Shader;

	ClientPlayer* m_Player;
};