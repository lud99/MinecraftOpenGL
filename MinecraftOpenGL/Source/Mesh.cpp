#include "Mesh.h"

#include <iostream>

#include <GL/glew.h>

Mesh::Mesh()
{
}

Mesh::Mesh(std::vector<Vertex>& vertices)
{
	m_Vertices = vertices;

	CreateVao();
}

void Mesh::CreateVao()
{
	// Generate vertex array object
	glGenVertexArrays(1, &m_Vao);
}

void Mesh::UpdateVertices(const std::vector<Vertex>& vertices)
{
	// Don't update the vertices if there are none
	if (vertices.empty())
		return;

	m_Vertices = vertices;

	BindVao();

	// Generate a vertex buffer if none has been created before
	if (m_Vbo == 0)
		glGenBuffers(1, &m_Vbo);

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	SetVertexAttributes();

	UnbindBuffer();
	UnbindVao();
}

void Mesh::UpdateVertices(const std::vector<Vertex>& vertices, const unsigned int indices[])
{
	// Don't update the vertices if there are none
	if (vertices.empty())
		return;

	m_Vertices = vertices;

	BindVao();

	// Generate a vertex buffer if none has been created before
	if (m_Vbo == 0)
		glGenBuffers(1, &m_Vbo);

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	SetVertexAttributes();

	// Generate a index buffer if none has been created before
	if (m_Ibo == 0)
		glGenBuffers(1, &m_Ibo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	UnbindBuffer();
	UnbindVao();
}

void Mesh::SetVertexAttributes()
{
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3 /* x, y, z */, GL_FLOAT, GL_FALSE, sizeof(Vertex) /* Number of total bytes */, 0);

	// Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4 /* r, g, b, a */, GL_FLOAT, GL_FALSE, sizeof(Vertex) /* Number of total bytes */, (const void*)offsetof(Vertex, color));
}

void Mesh::BindVao() { glBindVertexArray(m_Vao); }

void Mesh::UnbindVao() { glBindVertexArray(0); }

void Mesh::BindBuffer(unsigned int buffer) { glBindBuffer(GL_ARRAY_BUFFER, buffer); }

void Mesh::UnbindBuffer() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void Mesh::Render()
{
	BindVao();
	//BindBuffer(m_Ibo);

	//glDrawElements(GL_TRIANGLES, m_Vertices.size(), GL_UNSIGNED_INT, nullptr);
	glDrawArrays(GL_TRIANGLES, 0, m_Vertices.size());

	//UnbindBuffer();
	UnbindVao();
}

void Mesh::Render(int verticesCount)
{
	BindVao();

	glDrawArrays(GL_TRIANGLES, 0, verticesCount);

	UnbindVao();
}

std::vector<Vertex>& Mesh::GetVertices() { return m_Vertices; }
const unsigned int Mesh::GetVao() { return m_Vao; }

Mesh::~Mesh()
{
}
