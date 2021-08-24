#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <string>

#include "ChunkBlock.h"

class Chunk;

struct ChunkFile
{
	glm::ivec2 position;
	ChunkBlock*** blocks;
};

namespace ChunkIO
{
	ChunkFile ReadFile(const std::string& filepath);

	void WriteFile(const std::string& filepath, Chunk* chunk);
	void WriteFile(const std::string& filepath, ChunkFile chunk);
};