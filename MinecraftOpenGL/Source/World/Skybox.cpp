#include "Skybox.h"

#include "WorldRenderer.h"

#include "Player/ClientPlayer.h"

#include <Common/Graphics/BasicVertices.h>

#include <Common/World/IWorld.h>

Skybox::Skybox()
{

}

void Skybox::Init()
{
	m_Shader = ShaderLoader::CreateShader("Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag");

	m_Mesh.CreateVao();

	// Load cubemap
	std::vector<std::string> cubemapFacesFilepaths = {
		"Resources/Images/Skybox/right.png",
		"Resources/Images/Skybox/left.png",
		"Resources/Images/Skybox/top.png",
		"Resources/Images/Skybox/bottom.png",
		"Resources/Images/Skybox/front.png",
		"Resources/Images/Skybox/back.png"
	};

	m_Cubemap.Load(cubemapFacesFilepaths);

	// Create vertices

	for (int i = 0; i < 36; i++)
	{
		BasicVertex vertex;

		vertex.position = BasicVertices::Cube::FullCube[i];

		m_Mesh.AddVertex(vertex);
	}

	m_Mesh.Update();
}

void Skybox::Render()
{
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL); // Makes it possible to draw the skybox last, so that occluded pixels aren't rendered

	m_Shader.Bind();

	glm::mat4 view = glm::mat4(glm::mat3(ClientPlayer::GetCamera().m_ViewMatrix)); // Remove translation and keep only the rotation

	m_Shader.SetUniform("u_ProjectionMatrix", ClientPlayer::GetCamera().m_ProjectionMatrix);
	m_Shader.SetUniform("u_ViewMatrix", view);

	m_Mesh.Render();

	m_Shader.Unbind();

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS); // Set depth function back to default
}

Skybox::~Skybox()
{

}