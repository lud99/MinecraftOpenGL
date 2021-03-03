#include "BlockTypes.h"

#include "BlockIds.h"
#include "../Graphics/Mesh.hpp"
#include "../Graphics/BasicVertices.h"
#include "../Graphics/Textures/TextureIds.h"

namespace BlockTypes {
	void CreateBlocks()
	{
		using namespace BasicVertices;

		for (int i = 0; i < Count; i++) {
			Blocks[i].id = i;
			
			for (int j = 0; j < Block::facesCount; j++) {
				Blocks[i].faces[j].positions = Cube::GetFace(j);
				Blocks[i].faces[j].direction = j;
			}
		}

		Blocks[Air].isTransparent = true;
		Blocks[Air].isOpaque = false;

		// Set all sides to the same texture as the default

		// Dirt
		Blocks[Dirt].SetTexture(TextureIds::Dirt);
		// Stone
		Blocks[Stone].SetTexture(TextureIds::Stone);
		// Water
		Blocks[Water].SetTexture(TextureIds::Water);
		Blocks[Water].isTranslucent = true;
		Blocks[Water].isOpaque = false;
		// Gravel
		Blocks[Gravel].SetTexture(TextureIds::Gravel);
		// Sand
		Blocks[Sand].SetTexture(TextureIds::Sand);

		// Set all the textures for the oak log
		Blocks[OakLog].faces[Cube::Faces::Left].textureId = TextureIds::OakLogSide;
		Blocks[OakLog].faces[Cube::Faces::Right].textureId = TextureIds::OakLogSide;
		Blocks[OakLog].faces[Cube::Faces::Bottom].textureId = TextureIds::OakLogTop;
		Blocks[OakLog].faces[Cube::Faces::Top].textureId = TextureIds::OakLogTop;
		Blocks[OakLog].faces[Cube::Faces::Front].textureId = TextureIds::OakLogSide;
		Blocks[OakLog].faces[Cube::Faces::Back].textureId = TextureIds::OakLogSide;

		// Oak leaves
		Blocks[OakLeaves].SetTexture(TextureIds::OakLeaves);
		Blocks[OakLeaves].isTransparent = true;
		Blocks[OakLeaves].isOpaque = false;

		// Set all the textures for the grass block
		Blocks[Grass].faces[Cube::Faces::Left].textureId = TextureIds::GrassSide;
		Blocks[Grass].faces[Cube::Faces::Right].textureId = TextureIds::GrassSide;
		Blocks[Grass].faces[Cube::Faces::Bottom].textureId = TextureIds::Dirt;
		Blocks[Grass].faces[Cube::Faces::Top].textureId = TextureIds::GrassTop;
		Blocks[Grass].faces[Cube::Faces::Front].textureId = TextureIds::GrassSide;
		Blocks[Grass].faces[Cube::Faces::Back].textureId = TextureIds::GrassSide;
	}
}

Block BlockTypes::Blocks[Count];