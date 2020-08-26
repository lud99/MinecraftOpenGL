#include "WorldRenderer.h"

#include <map>

#include "Player.h"
#include "World.h"
#include "Chunk/Chunk.h"
#include "../Crosshair.h"

WorldRenderer::WorldRenderer()
{
	m_BlockShader = ShaderLoader::CreateShader("Resources/Shaders/Basic.vert", "Resources/Shaders/Basic.frag");
}

void WorldRenderer::Render()
{
	// Update the view matrix
	UpdateViewMatrix();

	m_BlockShader.Bind();

	m_BlockShader.SetUniform("u_ProjectionMatrix", m_ProjectionMatrix);
	m_BlockShader.SetUniform("u_ViewMatrix", m_ViewMatrix);

	std::map<int, Chunk*>& chunks = World::GetChunks();
	for (auto const& entry : chunks)
		entry.second->m_OpaqueMesh.Render();

	for (auto const& entry : chunks)
		entry.second->m_WaterMesh.Render();

	if (World::m_LookingAtCollider.m_Enabled) World::m_LookingAtCollider.RenderHitbox();

	//World::GetPlayer().GetCrosshair()->Render();

	/*glVertex3f(World::GetPlayer().m_LookingAtPosition.x, World::GetPlayer().m_LookingAtPosition.y, World::GetPlayer().m_LookingAtPosition.z);
	glVertex3f(World::GetPlayer().m_Position.x, World::GetPlayer().m_Position.y, World::GetPlayer().m_Position.z);
	
	glEnd();*/
}

void WorldRenderer::UpdateViewMatrix()
{
	Player& player = World::GetPlayer();

	m_ViewMatrix = glm::lookAt(player.m_Position, player.m_Position + player.GetCamera().m_Front, player.GetCamera().m_Up);
}

WorldRenderer::~WorldRenderer()
{

}