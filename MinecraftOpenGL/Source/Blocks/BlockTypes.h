#pragma once

#include "Block.h"
#include "BlockIds.h"

namespace BlockTypes {
	extern Block Blocks[BlockIds::Count];

	void CreateBlocks();
}