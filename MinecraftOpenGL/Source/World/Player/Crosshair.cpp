#include "Crosshair.h"

#include "../WorldRenderer.h"

#include "../../Graphics/BasicVertices.h"
#include "../../Graphics/Textures/Texture2D.h"

#include "../World.h"

Crosshair::Crosshair()
{

}

void Crosshair::Init()
{
	m_Shader = ShaderLoader::CreateShader("Resources/Shaders/Crosshair.vert", "Resources/Shaders/Crosshair.frag");
	m_Texture = new Texture2D("Resources/Images/crosshair.png");

	// Create vertices
	for (int i = 0; i < 4; i++)
	{
		TextureVertex vertex;

		vertex.position = BasicVertices::Quad::Face[i];
		vertex.textureCoord = BasicVertices::Quad::TextureCoordinates[i];

		m_Mesh.AddVertex(vertex);
	}
	for (int i = 0; i < 6; i++)
	{
		m_Mesh.AddIndex(BasicVertices::Quad::Indices[i]);
	}

	m_Mesh.Update();
}

void Crosshair::Render(WorldRenderer* worldRenderer)
{
	m_Shader.Bind();
	m_Texture->Bind();

	m_Shader.SetUniform("u_ProjectionMatrix", worldRenderer->m_ProjectionMatrix);

	m_Mesh.Render();

	m_Texture->Unbind();
	m_Shader.Unbind();
}

Crosshair::~Crosshair()
{
	delete m_Texture;
}