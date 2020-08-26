#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "../../Graphics/Textures/Texture2D.h"
#include "../../Graphics/Textures/TextureAtlas.h"
#include "../World.h"
#include "../../Blocks/BlockTypes.h"

#include "../../Collider.h"

class Chunk;

struct BlockFaceData
{
	std::vector<Vertex> opaqueVertices;
	std::vector<Vertex> waterVertices;
	std::vector<unsigned int> opaqueIndices;
	std::vector<unsigned int> waterIndices;
};

struct AdjacentChunks
{
	AdjacentChunks();

	Chunk* Left = nullptr;
	Chunk* Right = nullptr;
	Chunk* Top = nullptr;
	Chunk* Bottom = nullptr;
	Chunk* Back = nullptr;
	Chunk* Front = nullptr;
};

/*std::vector<Vertex>& opaqueVertices, std::vector<Vertex>& waterVertices,
		std::vector<unsigned int>& opaqueIndices, std::vector<unsigned int>& waterIndices*/

class ChunkBlock
{
public:
	ChunkBlock();

	void AddBlockFace(BlockFace& face, Mesh* opaqueMesh = nullptr, Mesh* waterMesh = nullptr);

	void AddBlockFaces(Mesh* opaqueMesh = nullptr, Mesh* waterMesh = nullptr);
	void AddAllBlockFaces();

	Chunk* GetChunk();

	inline const glm::ivec3 GetLocalPosition() { return m_LocalPosition; };
	const glm::ivec3 GetWorldPosition();

	inline void SetLocalPosition(glm::ivec3 position) { m_LocalPosition = position; }

	inline void SetEnabled(bool flag) { m_Enabled = flag; };
	inline bool IsEnabled() { return m_Enabled; }

	inline Block* GetBlockType() { return &BlockTypes::Blocks[m_BlockId]; }

	~ChunkBlock();

private:
	Chunk* GetChunkAtRelativePosition(glm::ivec2 offset);
	ChunkBlock* GetBlockAtRelativePosition(glm::ivec3 offset);

	bool ChunkExistsAtRelativePosition(glm::ivec3 offset);
	bool BlockExistsAtRelativePosition(glm::ivec3 offset);

	bool ShouldAddBlockFace(Directions direction, Chunk* adjacentChunk);

	AdjacentChunks GetAdjacentChunks();

public:
	int m_ChunkIndex = 0;

	int m_BlockId = BlockIds::Air;

private:
	glm::ivec3 m_LocalPosition;

	bool m_Enabled = true;
	bool m_Highlighted = false;
};