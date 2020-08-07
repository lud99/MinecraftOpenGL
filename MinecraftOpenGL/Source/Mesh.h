#pragma once

#include <vector>

#include <glm/vec3.hpp>

#include "Vertex.h"
#include "Textures/Texture2D.h"

enum Faces {
	Left = 0,
	Right = 1,
	Bottom = 2,
	Top = 3,
	Back = 4,
	Front = 5
};

namespace CubeFaces {
	const static glm::vec3 Top[4] = {
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3( 0.5f,  0.5f,  0.5f),
		glm::vec3( 0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
	};

	const static glm::vec3 Bottom[4] = {
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3( 0.5f, -0.5f,  0.5f),
		glm::vec3( 0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
	};

	const static glm::vec3 Left[4] = {
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
	};

	const static glm::vec3 Right[4] = {
		glm::vec3( 0.5f, -0.5f, -0.5f),
		glm::vec3( 0.5f, -0.5f,  0.5f),
		glm::vec3( 0.5f,  0.5f,  0.5f),
		glm::vec3( 0.5f,  0.5f, -0.5f),
	};

	const static glm::vec3 Front[4] = {
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3( 0.5f, -0.5f,  0.5f),
		glm::vec3( 0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),
	};

	const static glm::vec3 Back[4] = {
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3( 0.5f, -0.5f, -0.5f),
		glm::vec3( 0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
	};

	const glm::vec3* GetFace(int id);

	const static unsigned int Indices[6] = {
		0, 1, 2,
		2, 3, 0
	};
};

class Mesh
{
public:
	Mesh();
	Mesh(std::vector<Vertex>& vertices);

	void CreateVao();

	void Update();
	void UpdateVertices(const std::vector<Vertex>&);
	void SetVertexAttributes();

	void BindVao();
	void BindBuffer(unsigned int buffer);
	void UnbindVao();
	void UnbindBuffer();

	void Clear();

	void Render();
	void Render(int verticesCount);

	std::vector<Vertex>& GetVertices();
	const unsigned int GetVao();

	~Mesh();

public:
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;

	Texture2D* m_Texture;

protected:
	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;
	unsigned int m_Ebo;
};