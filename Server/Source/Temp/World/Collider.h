#pragma once

#include <Graphics/Mesh.hpp>
#include <Graphics/Shader.h>

#include <glm/vec3.hpp>

struct BasicColliders
{
	static constexpr glm::vec3 Box[8] = {
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3( 0.5f,  0.5f,  0.5f),
		glm::vec3( 0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),

		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3( 0.5f,  0.5f, -0.5f),
		glm::vec3( 0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
	};

	static constexpr unsigned int BoxIndices[36] = {
		0, 1, 3, 3, 1, 2,
		1, 5, 2, 2, 5, 6,
		5, 4, 6, 6, 4, 7,
		4, 0, 7, 7, 0, 3,
		3, 2, 7, 7, 2, 6,
		4, 5, 0, 0, 5, 1
	};
};

class Collider
{
public:
	Collider();

	void Init();

	void RenderHitbox();

	inline Mesh<BasicVertex>& GetMesh() { return m_Mesh; };
	inline Shader& GetShader() { return m_Shader; }

	~Collider();

public:
	glm::vec3 m_Position;

	bool m_Enabled = false;

private:
	Mesh<BasicVertex> m_Mesh;
	Shader m_Shader;

	const glm::vec3* m_Corners = BasicColliders::Box;
};