#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../../Graphics/Mesh.h"

#ifdef cols
	const glm::vec4 colors[36] = {
		glm::vec4(0.583f,  0.771f,  0.014f, 1.0f),
		glm::vec4(0.609f,  0.115f,  0.436f, 1.0f),
		glm::vec4(0.327f,  0.483f,  0.844f, 1.0f),
		glm::vec4(0.822f,  0.569f,  0.201f, 1.0f),
		glm::vec4(0.435f,  0.602f,  0.223f, 1.0f),
		glm::vec4(0.310f,  0.747f,  0.185f, 1.0f),
		glm::vec4(0.597f,  0.770f,  0.761f, 1.0f),
		glm::vec4(0.559f,  0.436f,  0.730f, 1.0f),
		glm::vec4(0.359f,  0.583f,  0.152f, 1.0f),
		glm::vec4(0.483f,  0.596f,  0.789f, 1.0f),
		glm::vec4(0.559f,  0.861f,  0.639f, 1.0f),
		glm::vec4(0.195f,  0.548f,  0.859f, 1.0f),
		glm::vec4(0.014f,  0.184f,  0.576f, 1.0f),
		glm::vec4(0.771f,  0.328f,  0.970f, 1.0f),
		glm::vec4(0.406f,  0.615f,  0.116f, 1.0f),
		glm::vec4(0.676f,  0.977f,  0.133f, 1.0f),
		glm::vec4(0.971f,  0.572f,  0.833f, 1.0f),
		glm::vec4(0.140f,  0.616f,  0.489f, 1.0f),
		glm::vec4(0.997f,  0.513f,  0.064f, 1.0f),
		glm::vec4(0.945f,  0.719f,  0.592f, 1.0f),
		glm::vec4(0.543f,  0.021f,  0.978f, 1.0f),
		glm::vec4(0.279f,  0.317f,  0.505f, 1.0f),
		glm::vec4(0.167f,  0.620f,  0.077f, 1.0f),
		glm::vec4(0.347f,  0.857f,  0.137f, 1.0f),
		glm::vec4(0.055f,  0.953f,  0.042f, 1.0f),
		glm::vec4(0.714f,  0.505f,  0.345f, 1.0f),
		glm::vec4(0.783f,  0.290f,  0.734f, 1.0f),
		glm::vec4(0.722f,  0.645f,  0.174f, 1.0f),
		glm::vec4(0.302f,  0.455f,  0.848f, 1.0f),
		glm::vec4(0.225f,  0.587f,  0.040f, 1.0f),
		glm::vec4(0.517f,  0.713f,  0.338f, 1.0f),
		glm::vec4(0.053f,  0.959f,  0.120f, 1.0f),
		glm::vec4(0.393f,  0.621f,  0.362f, 1.0f),
		glm::vec4(0.673f,  0.211f,  0.457f, 1.0f),
		glm::vec4(0.820f,  0.883f,  0.371f, 1.0f),
		glm::vec4(0.982f,  0.099f,  0.879f, 1.0f),
	};
#endif // !cols

class ChunkBlock;
class ChunkMesh;

class Chunk
{
public:
	Chunk(glm::ivec2 position);

	void RebuildMesh();

	void Render();

	void Fill(const glm::vec4* colors);
	void CreateSphere(const glm::vec4* colors);
	void GenerateTerrain();

	void SetBlockAt(glm::ivec3 position, ChunkBlock* newBlock);
	void SetBlockAt(glm::vec3 position, ChunkBlock* newBlock);

	ChunkBlock*** GetAllBlocks();
	ChunkBlock* GetBlockAt(glm::ivec3 position);
	ChunkBlock* GetBlockAt(glm::vec3 position);

	bool BlockExistsAt(glm::vec3 localPosition);

	glm::ivec2& GetPosition();
	glm::ivec2 GetWorldPosition();
	void SetPosition(glm::ivec2 position);

	int GetIndex();

	~Chunk();

public:
	static const int Width = 16;
	static const int Height = 128;
	static const int Depth = 16;

	static const int BlockCount = Width * Height * Depth;

	bool m_HasGenerated = false;

	ChunkBlock*** m_Blocks;

	Mesh m_OpaqueMesh;
	Mesh m_WaterMesh;

private:
	glm::ivec2 m_Position = glm::ivec2(0, 0);

	int m_Index = 0;
};