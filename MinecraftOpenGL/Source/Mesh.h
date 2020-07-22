#pragma once

#include <vector>

#include <glm/vec3.hpp>

#include "Vertex.h"

namespace CubeFaces {
	const static float Top[18] = {
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,

		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f,
	};

	const static float Bottom[] = {
		 0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f,-0.5f,

		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
	};

	const static float Left[18] = {
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};

	const static float Right[18] = {
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,

		0.5f,  0.5f,  0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
	};

	const static float Front[18] = {
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 
		-0.5f,  0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		 0.5f, -0.5f, 0.5f,
	};

	const static float Back[18] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,

		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
	};

	const static float _Back[12] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,

		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
	};

	const static unsigned int BackIndices[6] = {
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

	void UpdateVertices(const std::vector<Vertex>&);
	void UpdateVertices(const std::vector<Vertex>&, const unsigned int indices[]);
	void SetVertexAttributes();

	void BindVao();
	void BindBuffer(unsigned int buffer);
	void UnbindVao();
	void UnbindBuffer();

	void Render();
	void Render(int verticesCount);

	std::vector<Vertex>& GetVertices();
	const unsigned int GetVao();

	~Mesh();
	unsigned int test = 1;

public:
	std::vector<Vertex> m_Vertices;

protected:
	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;
};