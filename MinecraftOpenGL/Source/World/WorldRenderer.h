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

	void UpdateViewMatrix();

	~WorldRenderer();

private:
	WorldRenderer();

public:
	glm::mat4 m_ProjectionMatrix = glm::perspective(glm::radians(70.0f), 1920.0f / 1080.0f, 0.1f, 2500.0f);
	glm::mat4 m_ViewMatrix = glm::mat4(1.0);

	Shader m_ChunkShader;

	ClientWorld* m_World = nullptr;
	Skybox* m_Skybox = nullptr;
};