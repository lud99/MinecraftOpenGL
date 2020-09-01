#pragma once

#include <vector>
#include <mutex>

#include <cstdint>

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
		glm::vec3(-0.5,  0.5,  0.5),
		glm::vec3( 0.5,  0.5,  0.5),
		glm::vec3( 0.5,  0.5, -0.5),
		glm::vec3(-0.5,  0.5, -0.5),
	};

	const static glm::vec3 Bottom[4] = {
		glm::vec3(-0.5, -0.5,  0.5),
		glm::vec3( 0.5, -0.5,  0.5),
		glm::vec3( 0.5, -0.5, -0.5),
		glm::vec3(-0.5, -0.5, -0.5),
	};

	const static glm::vec3 Left[4] = {
		glm::vec3(-0.5, -0.5, -0.5),
		glm::vec3(-0.5, -0.5,  0.5),
		glm::vec3(-0.5,  0.5,  0.5),
		glm::vec3(-0.5,  0.5, -0.5),
	};

	const static glm::vec3 Right[4] = {
		glm::vec3( 0.5, -0.5, -0.5),
		glm::vec3( 0.5, -0.5,  0.5),
		glm::vec3( 0.5,  0.5,  0.5),
		glm::vec3( 0.5,  0.5, -0.5),
	};

	const static glm::vec3 Front[4] = {
		glm::vec3(-0.5, -0.5,  0.5),
		glm::vec3( 0.5, -0.5,  0.5),
		glm::vec3( 0.5,  0.5,  0.5),
		glm::vec3(-0.5,  0.5,  0.5),
	};

	const static glm::vec3 Back[4] = {
		glm::vec3(-0.5, -0.5, -0.5),
		glm::vec3( 0.5, -0.5, -0.5),
		glm::vec3( 0.5,  0.5, -0.5),
		glm::vec3(-0.5,  0.5, -0.5),
	};

	const glm::vec3* GetFace(int id);

	const static uint16_t Indices[6] = {
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

	void AddVertex(Vertex vertex);
	void SetVertices(std::vector<Vertex> vertices);
	void SetVertices(Mesh mesh);

	void AddIndex(uint16_t index);
	void SetIndices(std::vector<uint16_t> indices);
	void SetIndices(Mesh mesh);

	std::vector<Vertex>& GetVertices();
	std::vector<uint16_t>& GetIndices();

	const unsigned int GetVao();

	~Mesh();

public:
	Texture2D* m_Texture;

protected:
	std::vector<Vertex> m_Vertices;
	std::vector<uint16_t> m_Indices;

	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ebo;

private:
	std::mutex m_VerticesMutex;
	std::mutex m_IndicesMutex;
};