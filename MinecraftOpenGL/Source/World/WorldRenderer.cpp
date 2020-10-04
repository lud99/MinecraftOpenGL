#include "WorldRenderer.h"

#include <map>

#include "Player/Player.h"
#include "World.h"
#include "Chunk/Chunk.h"
#include "Player/Crosshair.h"

WorldRenderer::WorldRenderer()
{
	m_ChunkShader = ShaderLoader::CreateShader("Resources/Shaders/Chunk.vert", "Resources/Shaders/Chunk.frag");
}

void WorldRenderer::Render()
{
	auto start = std::chrono::high_resolution_clock::now();

	// Update the view matrix
	UpdateViewMatrix();

	m_ChunkShader.Bind();

	m_ChunkShader.SetUniform("u_ProjectionMatrix", m_ProjectionMatrix);
	m_ChunkShader.SetUniform("u_ViewMatrix", m_ViewMatrix);

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
			m_ChunkShader.SetUniform("u_Rebuilding", entry.second->m_IsRebuilding);
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
			m_ChunkShader.SetUniform("u_Rebuilding", entry.second->m_IsRebuilding);
			m_ChunkShader.SetUniform("u_ShouldBeRemoved", entry.second->m_ShouldBeRemoved);

			entry.second->m_WaterMesh.Render();
		}
	}

	if (World::m_LookingAtCollider.m_Enabled) World::m_LookingAtCollider.RenderHitbox();

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	//std::cout << "Render took " << duration.count() << " ms\n";
}

void WorldRenderer::UpdateViewMatrix()
{
	Player& player = World::GetPlayer();

	m_ViewMatrix = glm::lookAt(player.m_Position, player.m_Position + player.GetCamera().m_Front, player.GetCamera().m_Up);
}

WorldRenderer::~WorldRenderer()
{

}