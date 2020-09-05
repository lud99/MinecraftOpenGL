#include "Collider.h"

#include <GL/glew.h>

#include "Graphics/Shaders/ShaderLoader.h"

#include "World/World.h"

Collider::Collider()
{

}

void Collider::Init()
{
	m_Shader = ShaderLoader::CreateShader("Resources/Shaders/Collider.vert", "Resources/Shaders/Collider.frag");

	/*for (int i = 0; i < 8; i++)
		m_Mesh.AddVertex(Vertex(BasicColliders::Box[i] * 1.00625f));

	for (int i = 0; i < 36; i++)
		m_Mesh.AddIndex(BasicColliders::BoxIndices[i]);*/

	m_Mesh.Update(); 
}

void Collider::RenderHitbox()
{
	// Wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	m_Shader.Bind();

	m_Shader.SetUniform("u_ProjectionMatrix", World::m_Renderer->m_ProjectionMatrix);
	m_Shader.SetUniform("u_ViewMatrix", World::m_Renderer->m_ViewMatrix);
	m_Shader.SetUniform("u_ModelMatrix", glm::translate(glm::mat4(1.0), m_Position));

	m_Mesh.Render();

	m_Shader.Unbind();

	// Turn off wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

Collider::~Collider()
{

}