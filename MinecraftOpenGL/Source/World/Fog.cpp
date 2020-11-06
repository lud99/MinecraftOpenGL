#include "Fog.h"

#include "WorldRenderer.h"
#include "World.h"

Fog::Fog()
{
}

void Fog::Init()
{
	m_Shader = ShaderLoader::CreateShader("Resources/Shaders/Fog.vert", "Resources/Shaders/Fog.frag");

	m_Mesh.CreateVao();

	/*GLFWwindow* window = World::m_Renderer->m_Window;
	int windowWidth, windowHeight;

	glfwGetWindowSize(window, &windowWidth, &windowHeight);*/

	// Create vertices
	for (int i = 0; i < 4; i++) {
		Vertex vertex;
		VertexData data;
		
		glm::vec3 pos(CubeFaces::Front[i]);

		data.index = 0;
		data.lightLevel = 0;
		data.texture = 0;
		data.position = pos;

		vertex.SetData(data);

		m_Mesh.AddVertex(vertex);
	}
	// Indices
	for (int i = 0; i < 6; i++) {
		m_Mesh.AddIndex(CubeFaces::Indices[i]);
	}

	m_Mesh.Update();
}

void Fog::Render(WorldRenderer* worldRenderer)
{
	m_Shader.Bind();

	m_Shader.SetUniform("u_ProjectionMatrix", worldRenderer->m_ProjectionMatrix);
	m_Shader.SetUniform("u_ViewMatrix", worldRenderer->m_ViewMatrix);

	m_Mesh.Render();

	m_Shader.Unbind();
}

Fog::~Fog()
{

}