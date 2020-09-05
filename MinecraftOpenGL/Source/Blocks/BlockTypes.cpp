#include "BlockTypes.h"

#include "BlockIds.h"
#include "../Graphics/Textures/TextureIds.h"

using namespace BlockIds;

namespace BlockTypes {
	void CreateBlocks()
	{
		for (int i = 0; i < Count; i++) {
			Blocks[i].id = i;
			
			for (int j = 0; j < Block::facesCount; j++) {
				Blocks[i].faces[j].positions = CubeFaces::GetFace(j);
				Blocks[i].faces[j].direction = j;
			}
		}

		// Set all sides to the same texture as the default
		Blocks[Dirt].SetTexture(TextureIds::Dirt);
		Blocks[Stone].SetTexture(TextureIds::Stone);
		Blocks[Water].SetTexture(TextureIds::Water);
		Blocks[Water].isTransparent = true;

		Blocks[Grass].faces[Left].textureId = TextureIds::GrassSide;
		Blocks[Grass].faces[Right].textureId = TextureIds::GrassSide;
		Blocks[Grass].faces[Bottom].textureId = TextureIds::Dirt;
		Blocks[Grass].faces[Top].textureId = TextureIds::GrassTop;
		Blocks[Grass].faces[Front].textureId = TextureIds::GrassSide;
		Blocks[Grass].faces[Back].textureId = TextureIds::GrassSide;
	}
}

Block BlockTypes::Blocks[Count];