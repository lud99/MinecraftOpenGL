#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <mutex>
#include <unordered_map>
#include <vector>

#include <Common/ThreadPool.h>

class IWorld;
class ChunkBlock;
class BlockEntity;
class ChunkMesh;
class DroppedItem;

class Chunk;

struct AdjacentChunks
{
	Chunk* West = nullptr;
	Chunk* East = nullptr;
	Chunk* Top = nullptr;
	Chunk* Bottom = nullptr;
	Chunk* South = nullptr;
	Chunk* North = nullptr;
};

typedef std::unordered_map<uint16_t, BlockEntity*> BlockEntitiesMap;

class Chunk
{
public:
	Chunk(glm::ivec2 position);
	
	void Init();
	void CreateAndGenerateTerrain(ChunkAction::Priority priority, ChunkAction* nextAction = nullptr);

	void GenerateTerrain();
	void GenerateTerrainThreaded(ChunkAction::Priority priority, ChunkAction* nextAction = nullptr);
	void GetDataNet();
	void GetDataNetThreaded(ChunkAction::Priority priority, ChunkAction* nextAction = nullptr);

	std::string Serialize();

	void Fill(const glm::vec4* colors);
	void CreateSphere(const glm::vec4* colors);

	void Render();

	void SetBlockAt(glm::ivec3 position, ChunkBlock* newBlock);
	void SetBlockAt(glm::vec3 position, ChunkBlock* newBlock);

	ChunkBlock* GetAllBlocks();
	ChunkBlock* GetBlockAt(glm::u8vec3 position);
	ChunkBlock* GetBlockAt(glm::ivec3 position);
	ChunkBlock* GetBlockAt(glm::vec3 position);

	bool BlockExistsAt(glm::vec3 localPosition);

	glm::ivec2& GetPosition();
	glm::ivec2 GetWorldPosition();
	void SetPosition(glm::ivec2 position);

	AdjacentChunks GetAdjacentChunks();

	void SetDirty(bool flag);
	bool IsDirty();

	~Chunk();

public:
	static const int Width = 16;
	static const int Height = 256;
	static const int Depth = 16;

	static const int BlockCount = Width * Height * Depth;

	bool m_IsInitialized = false;
	bool m_HasGenerated = false;
	bool m_IsGenerating = false;
	bool m_ShouldRebuild = false;
	bool m_ShouldBeRemoved = false;
	bool m_IsRebuilding = false;

	AdjacentChunks m_AdjacentChunksWhenLastRebuilt;

	ChunkBlock* m_Blocks;
	BlockEntitiesMap m_BlockEntities;
	std::vector<DroppedItem*> m_DroppedItems;

	uint8_t m_HeightMap[Width][Depth];

#ifndef SERVER_BUILD
	ChunkMesh* m_ChunkMesh = nullptr;
#endif
	IWorld* m_World = nullptr;

private:
	glm::ivec2 m_Position = glm::ivec2(0, 0);

	// Flag to indicate if the chunk has changed and needs rebuilding
	bool m_IsDirty = false;
};