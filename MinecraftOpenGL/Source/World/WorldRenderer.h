#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#include <Graphics/Shader.h>

class ClientWorld;
class Skybox;

class WorldRenderer 
{
public:
	static WorldRenderer& Get();

	void Init();

	void Render();

	~WorldRenderer();

private:
	WorldRenderer();

public:
	Shader m_ChunkShader;

	ClientWorld* m_World = nullptr;
	Skybox* m_Skybox = nullptr;

private:
	Shader m_PlayerModelShader;
};