#include "BlockTypes.h"

#include "BlockIds.h"
#include "../Textures/TextureIds.h"

using namespace BlockIds;

namespace BlockTypes {
	void CreateBlocks()
	{
		for (int i = 0; i < Count; i++) {
			for (int j = 0; j < Block::facesCount; j++) {
				Blocks[i].faces[j].positions = CubeFaces::GetFace(j);
			}
		}

		Blocks[Grass].faces[Left].textureId = TextureIds::GrassSide;
		Blocks[Grass].faces[Right].textureId = TextureIds::GrassSide;
		Blocks[Grass].faces[Bottom].textureId = TextureIds::Dirt;
		Blocks[Grass].faces[Top].textureId = TextureIds::GrassTop;
		Blocks[Grass].faces[Front].textureId = TextureIds::GrassSide;
		Blocks[Grass].faces[Back].textureId = TextureIds::GrassSide;

		for (int i = 0; i < Blocks[Dirt].facesCount; i++)
			Blocks[Dirt].faces[i].textureId = TextureIds::Dirt;
	}
}

Block BlockTypes::Blocks[BlockIds::Count];