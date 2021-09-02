#include "WorldRenderer.h"

#include <map>
#include <optick.h>

#include "../Window.h"
#include "../LogRenderer.h"
#include "Player/ClientPlayer.h"
#include <Common/Net/NetworkClient.h>
#include "ClientWorld.h"
#include "Chunk/ChunkMesh.h"
#include "Skybox.h"
#include <Common/Chunk/Chunk.h>
#include "DroppedItem.h"

#include <GLFW/glfw3.h>

WorldRenderer::WorldRenderer()
{
}

WorldRenderer& WorldRenderer::Get()
{
	static WorldRenderer instance;
	return instance;
}

void WorldRenderer::Init()
{
	m_ChunkShader = ShaderLoader::CreateShader("Resources/Shaders/Chunk.vert", "Resources/Shaders/Chunk.frag");
	m_PlayerModelShader = ShaderLoader::CreateShader("Resources/Shaders/Collider.vert", "Resources/Shaders/Collider.frag");
	//m_Fog.Init();

	m_Skybox = new Skybox();
	m_Skybox->Init();

	m_Font.Load("Resources/Fonts/arial.ttf");
	LogRenderer::Get().Init();
}

void WorldRenderer::Render()
{
	OPTICK_EVENT();
	
	auto start = std::chrono::high_resolution_clock::now();

	m_ChunkShader.Bind();

	if (!m_World->m_LocalPlayer)
		return;

	Camera& camera = m_World->m_LocalPlayer->GetCamera();

	// Update the matricies
	int windowWidth, windowHeight;
	glfwGetWindowSize(InputHandler::GetWindow(), &windowWidth, &windowHeight);

	camera.UpdateMatrices(windowWidth, windowHeight);

	glm::mat4 mvp = camera.m_ProjectionMatrix * camera.m_ViewMatrix;
	m_ChunkShader.SetUniform("u_MVP", mvp);

	ChunkMap& chunks = m_World->GetChunks();
	for (auto const& entry : chunks)
	{
		if (!entry.second->m_ChunkMesh) continue;

		ClientPlayer* player = m_World->m_LocalPlayer;
		glm::vec2 front(player->GetCamera().m_Front2D.x, player->GetCamera().m_Front2D.z);

		glm::vec2 chunkPosition = entry.second->GetWorldPosition();
		glm::vec2 pPos(player->m_Position.x, player->m_Position.z);

		float dot = glm::dot(chunkPosition - pPos, front);

		// Chunk is in front of camera
		//if (dot > 0)
		{
			m_ChunkShader.SetUniform("u_ChunkPosition", entry.second->GetWorldPosition());
			m_ChunkShader.SetUniform("u_Dirty", entry.second->IsDirty());
			m_ChunkShader.SetUniform("u_ShouldBeRemoved", entry.second->m_ShouldBeRemoved);

			entry.second->m_ChunkMesh->m_OpaqueMesh.Render();
			//entry.second->m_WaterMesh.Render();
		}
	}

	for (auto const& entry : chunks) 
	{
		if (!entry.second->m_ChunkMesh) continue;

		ClientPlayer* player = m_World->m_LocalPlayer;
		glm::vec2 front(player->GetCamera().m_Front2D.x, player->GetCamera().m_Front2D.z);

		glm::vec2 chunkPosition = entry.second->GetWorldPosition();
		glm::vec2 pPos(player->m_Position.x, player->m_Position.z);

		float dot = glm::dot(chunkPosition - pPos, front);

		// Chunk is in front of camera
		//if (dot > 0)
		{
			m_ChunkShader.SetUniform("u_ChunkPosition", entry.second->GetWorldPosition());
			m_ChunkShader.SetUniform("u_Dirty", entry.second->IsDirty() || entry.second->m_IsRebuilding);
			m_ChunkShader.SetUniform("u_ShouldBeRemoved", entry.second->m_ShouldBeRemoved);

			entry.second->m_ChunkMesh->m_WaterMesh.Render();
		}
	}

	//if (World::m_LookingAtCollider.m_Enabled) 
		//World::m_LookingAtCollider.RenderHitbox();

	for (auto const& entry : chunks)
	{
		for (unsigned int i = 0; i < entry.second->m_DroppedItems.size(); i++)
			entry.second->m_DroppedItems[i]->Render();
	}

	//m_Fog.Render(this);

	if (m_World->m_LocalPlayer)
		m_World->m_LocalPlayer->m_Crosshair.Render();

	// Render all players
	m_PlayerModelShader.Bind();

	for (auto& entry : m_World->GetPlayers())
	{
		ClientPlayer* player = (ClientPlayer*)entry.second;
		if (!player) continue;
		if (player->m_IsMe) continue; // Don't render the local player

		// Set the player position as a uniform
		glm::mat4 modelMatrix(1.0f);
		modelMatrix = glm::translate(modelMatrix, player->m_Position);

		m_PlayerModelShader.SetUniform("u_ProjectionMatrix", camera.m_ProjectionMatrix);
		m_PlayerModelShader.SetUniform("u_ViewMatrix", camera.m_ViewMatrix);
		m_PlayerModelShader.SetUniform("u_ModelMatrix", modelMatrix);

		player->m_PlayerModel->Render();
	}

	m_Skybox->Render();

	// Render text
	LogRenderer::Get().Render();

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	//std::cout << "Render took " << duration.count() << " ms\n";
}

WorldRenderer::~WorldRenderer()
{

}