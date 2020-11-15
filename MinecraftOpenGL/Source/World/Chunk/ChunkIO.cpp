#include "ChunkIO.h"

#include "Chunk.h"

#include <iostream>
#include <fstream>

void ChunkIO::WriteFile(const std::string& filepath, Chunk* chunk)
{
	std::ofstream file;

	file.open(filepath);

	// Position
	file << chunk->GetPosition().x << "\n" << chunk->GetPosition().y << "\n";

	// Blocks
	for (int x = 0; x < Chunk::Width; x++)
	{
		for (int y = 0; y < Chunk::Height; y++)
		{
			for (int z = 0; z < Chunk::Depth; z++)
			{
				//file << "; Block\n";

				// Block position
				//file << x << ", " << y << ", " << z << "\n";

				// Block id
				file << (int)chunk->GetBlockAt(glm::vec3(x, y, z))->m_BlockId << "\n";
			}
		}
	}

	file.close();
}

void ChunkIO::WriteFile(const std::string& filepath, ChunkFile chunk)
{
	std::ofstream file;

	file.open("example.txt");
	file << "Writing this to a file.\n";

	file.close();
}

ChunkFile ChunkIO::ReadFile(const std::string& filepath)
{
	std::ifstream file;
	std::string line;

	ChunkFile chunkFile;

	// Create blocks
	chunkFile.blocks = new ChunkBlock**[Chunk::Width];

	// Create the blocks on the x axis
	for (int x = 0; x < Chunk::Width; x++)
	{
		chunkFile.blocks[x] = new ChunkBlock*[Chunk::Height]; // Blocks on x reference an array of y blocks 

		for (int y = 0; y < Chunk::Height; y++)
		{
			chunkFile.blocks[x][y] = new ChunkBlock[Chunk::Depth]; // Blocks on xy reference array of z blocks
		}
	}


	file.open(filepath);

	if (file.is_open())
	{
		// Chunk position
		std::getline(file, line); // x
		chunkFile.position.x = std::stoi(line);
		std::getline(file, line); // y
		chunkFile.position.y = std::stoi(line);

		// Iterate through all blocks
		for (int x = 0; x < Chunk::Width; x++)
		{
			for (int y = 0; y < Chunk::Height; y++)
			{
				for (int z = 0; z < Chunk::Depth; z++)
				{
					if (std::getline(file, line))
					{
						// Skip commented lines
						if (line.at(0) != ';') {
							ChunkBlock& block = chunkFile.blocks[x][y][z];

							block.SetLocalPosition(glm::u8vec3(x, y, z));
							block.m_BlockId = (int)std::stoi(line);
						}


					}
				}
			}
		}

		/*while (std::getline(file, line))
		{
			// Skip commented lines
			if (line.at(0) == ';')
			{
				if (line.compare("; Chunk Position"))
				{
					chunkFile.position = 
				}
			}


		}*/

		file.close();
	}

	return chunkFile;
}