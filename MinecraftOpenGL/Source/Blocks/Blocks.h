#pragma once

#include "../World/Chunk/ChunkBlock.h"

namespace Blocks
{
	class StoneBlock : public ChunkBlock
	{
	public:
		void OnBlockClick(int button, int action, int mods);
	};
};