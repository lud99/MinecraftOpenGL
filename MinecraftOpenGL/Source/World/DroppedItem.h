#pragma once

#include <glm/vec3.hpp>

#include <Graphics/Mesh.hpp>
#include <Graphics/Shader.h>

#include <Common/Blocks/Blocks.h>

class DroppedItem
{
public:
	DroppedItem();

	void Render();

	~DroppedItem();

public:
	glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	float m_Rotation = 0.0f;

	uint8_t m_BlockId = BlockIds::Dirt;

	Mesh<BasicVertex> m_Mesh;
	Shader m_Shader;
};