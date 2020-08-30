#include "Crosshair.h"

#include "Graphics/Shaders/ShaderLoader.h"

#include "World/World.h"

Crosshair::Crosshair()
{
	m_Shader = ShaderLoader::CreateShader("Resources/Shaders/Crosshair.vert", "Resources/Shaders/Crosshair.frag");

	/*for (int i = 0; i < 4; i++)
		m_Mesh.m_Vertices.push_back(Vertex(CubeFaces::Back[i]));

	for (int i = 0; i < 6; i++)
		m_Mesh.m_Indices.push_back(CubeFaces::Indices[i]);*/

	//m_Mesh.Update();
}

void Crosshair::Render()
{
	glm::vec3& lookingAt = World::GetPlayer().m_LookingAtPosition;

	m_Mesh.Clear();

	/*m_Mesh.m_Vertices.push_back(Vertex(World::GetPlayer().m_Position));
	m_Mesh.m_Vertices.push_back(Vertex(World::GetPlayer().m_Position + glm::vec3(0.1, 0.1, 0.1)));
	m_Mesh.m_Vertices.push_back(Vertex(lookingAt));
	m_Mesh.m_Vertices.push_back(Vertex(lookingAt + glm::vec3(0.1, 0.1, 0.1)));*/

	for (int i = 0; i < 6; i++)
		m_Mesh.AddIndex(CubeFaces::Indices[i]);

	m_Mesh.Update();

	m_Shader.Bind();

	m_Shader.SetUniform("u_ProjectionMatrix", World::m_Renderer->m_ProjectionMatrix);
	m_Shader.SetUniform("u_ViewMatrix", World::m_Renderer->m_ViewMatrix);
	//m_Shader.SetUniform("u_Size", 10.0f);

	m_Mesh.Render();

	m_Shader.Unbind();
}

Crosshair::~Crosshair()
{

}