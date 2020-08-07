#include "Mesh.h"

#include <iostream>

#include <GL/glew.h>

const glm::vec3* CubeFaces::GetFace(int id)
{
	switch (id)
	{
	case Faces::Left: return Left;
	case Faces::Right: return Right;
	case Faces::Bottom: return Bottom;
	case Faces::Top: return Top;
	case Faces::Back: return Back;
	case Faces::Front: return Front;
	}

	return nullptr;
}

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

void Mesh::Update()
{
	UpdateVertices(m_Vertices);
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

	// Generate element array object if none has been created before
	if (m_Ebo == 0)
		glGenBuffers(1, &m_Ebo);

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

	SetVertexAttributes();

	UnbindBuffer();
	UnbindVao();
}

void Mesh::SetVertexAttributes()
{
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3 /* x, y, z */, GL_FLOAT, GL_FALSE, sizeof(Vertex) /* Number of total bytes */, 0);

	// Color
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 4 /* r, g, b, a */, GL_FLOAT, GL_FALSE, sizeof(Vertex) /* Number of total bytes */, (const void*)offsetof(Vertex, color));

	// Texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2 /* x, y */, GL_FLOAT, GL_FALSE, sizeof(Vertex) /* Number of total bytes */, (const void*)offsetof(Vertex, textureCoordinates));
}

void Mesh::BindVao() { glBindVertexArray(m_Vao); }

void Mesh::UnbindVao() { glBindVertexArray(0); }

void Mesh::BindBuffer(unsigned int buffer) { glBindBuffer(GL_ARRAY_BUFFER, buffer); }

void Mesh::UnbindBuffer() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void Mesh::Clear() 
{
	m_Vertices.clear();
	m_Indices.clear();
}

void Mesh::Render()
{
	BindVao();
	m_Texture->Bind();

	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);

	m_Texture->Unbind();
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
	glDeleteVertexArrays(1, &m_Vao);

	glDeleteBuffers(1, &m_Vbo);
	glDeleteBuffers(1, &m_Ibo);
	glDeleteBuffers(1, &m_Ebo);
}
