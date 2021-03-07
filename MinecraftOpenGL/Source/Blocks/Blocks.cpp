#include "Blocks.h"

void Blocks::StoneBlock::OnBlockClick(int button, int action, int mods)
{
	std::cout << "Clicked on stone block!\n";

	m_BlockData += 1;
}