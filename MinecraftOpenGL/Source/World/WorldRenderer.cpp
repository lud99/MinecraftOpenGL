#include "WorldRenderer.h"

#include <map>

#include "Player/Player.h"
#include "World.h"
#include "Skybox.h"
#include "Chunk/Chunk.h"
#include "DroppedItem.h"

WorldRenderer::WorldRenderer()
{
	m_ChunkShader = ShaderLoader::CreateShader("Resources/Shaders/Chunk.vert", "Resources/Shaders/Chunk.frag");
	//m_Fog.Init();

	m_Skybox = new Skybox();
	m_Skybox->Init();
}

WorldRenderer::WorldRenderer(GLFWwindow* window)
{
	m_Window = window;
}

void WorldRenderer::Render()
{
	auto start = std::chrono::high_resolution_clock::now();

	// Update the view matrix
	UpdateViewMatrix();

	m_ChunkShader.Bind();

	glm::mat4 mvp = m_ProjectionMatrix * m_ViewMatrix;
	m_ChunkShader.SetUniform("u_MVP", mvp);

	ChunkMap& chunks = World::GetChunks();
	for (auto const& entry : chunks)
	{
		Player& player = World::GetPlayer();
		glm::vec2 front(player.GetCamera().m_Front2D.x, player.GetCamera().m_Front2D.z);

		glm::vec2 chunkPosition = entry.second->GetWorldPosition();
		glm::vec2 pPos(player.m_Position.x, player.m_Position.z);

		float dot = glm::dot(chunkPosition - pPos, front);

		// Chunk is in front of camera
		//if (dot > 0)
		{
			m_ChunkShader.SetUniform("u_ChunkPosition", entry.second->GetWorldPosition());
			m_ChunkShader.SetUniform("u_Dirty", entry.second->IsDirty());
			m_ChunkShader.SetUniform("u_ShouldBeRemoved", entry.second->m_ShouldBeRemoved);

			entry.second->m_OpaqueMesh.Render();
			//entry.second->m_WaterMesh.Render();
		}
	}

	for (auto const& entry : chunks) 
	{
		Player& player = World::GetPlayer();
		glm::vec2 front(player.GetCamera().m_Front2D.x, player.GetCamera().m_Front2D.z);

		glm::vec2 chunkPosition = entry.second->GetWorldPosition();
		glm::vec2 pPos(player.m_Position.x, player.m_Position.z);

		float dot = glm::dot(chunkPosition - pPos, front);

		// Chunk is in front of camera
		//if (dot > 0)
		{
			m_ChunkShader.SetUniform("u_ChunkPosition", entry.second->GetWorldPosition());
			m_ChunkShader.SetUniform("u_Dirty", entry.second->IsDirty() || entry.second->m_IsRebuilding);
			m_ChunkShader.SetUniform("u_ShouldBeRemoved", entry.second->m_ShouldBeRemoved);

			entry.second->m_WaterMesh.Render();
		}
	}

	if (World::m_LookingAtCollider.m_Enabled) 
		World::m_LookingAtCollider.RenderHitbox();

	for (auto const& entry : chunks)
	{
		for (unsigned int i = 0; i < entry.second->m_DroppedItems.size(); i++)
			entry.second->m_DroppedItems[i]->Render();
	}

	//m_Fog.Render(this);

	World::GetPlayer().m_Crosshair.Render(this);

	m_Skybox->Render(this);

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	//std::cout << "Render took " << duration.count() << " ms\n";
}

void WorldRenderer::UpdateViewMatrix()
{
	Player& player = World::GetPlayer();
	Camera& camera = player.GetCamera();
	glm::vec3 cameraPosition = camera.m_Position;

	m_ViewMatrix = glm::lookAt(cameraPosition, cameraPosition + camera.m_Front, camera.m_Up);
}

WorldRenderer::~WorldRenderer()
{

}