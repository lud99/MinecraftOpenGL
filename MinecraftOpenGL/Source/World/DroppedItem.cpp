#include "DroppedItem.h"

#include <Common/World/IWorld.h>
#include "WorldRenderer.h"
#include <Common/Blocks/Blocks.h>
#include <Graphics/BasicVertices.h>

DroppedItem::DroppedItem()
{
	BlockType& blockType = BlockTypes::Blocks[m_BlockId];

	for (int f = 0; f < 6; f++)
	{
		for (int i = 0; i < 4; i++)
		{
			glm::u8vec3 position(blockType.faces[f].positions[i]);

			BasicVertex vertex;
			vertex.position = (glm::vec3)position;

			m_Mesh.AddVertex(vertex);
		}

		for (int i = 0; i < 6; i++)
		{
			m_Mesh.AddIndex(BasicVertices::Cube::Indices[i] + (uint16_t)(m_Mesh.GetVertices().size() - 4));
		}
	}

	m_Shader = ShaderLoader::CreateShader("Resources/Shaders/DroppedItem.vert", "Resources/Shaders/DroppedItem.frag");
	m_Mesh.m_Texture = TextureAtlas::Get().Texture;

	m_Mesh.Update();
}

void DroppedItem::Render()
{
	m_Shader.Bind();

	glm::mat4 modelMatrix(1.0);
	modelMatrix = glm::translate(modelMatrix, m_Position);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(m_Rotation), glm::vec3(0, 1, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));

	m_Shader.SetUniform("u_ProjectionMatrix", WorldRenderer::Get().m_ProjectionMatrix);
	m_Shader.SetUniform("u_ViewMatrix", WorldRenderer::Get().m_ViewMatrix);
	m_Shader.SetUniform("u_ModelMatrix", modelMatrix);

	m_Shader.SetUniform("u_Texture", 0);
	
	m_Mesh.Render();

	m_Shader.Unbind();

	m_Rotation += 1.0f;
}

DroppedItem::~DroppedItem()
{

}