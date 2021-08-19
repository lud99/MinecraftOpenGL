#include "ModelParser.h"

#include <iostream>
#include <fstream>
#include <vector>

#include <glm/vec3.hpp>

#include "Mesh.hpp"

size_t split(const std::string& txt, std::vector<std::string>& strs, char ch)
{
	size_t pos = txt.find(ch);
	size_t initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

	return strs.size();
}

Mesh<TextureVertex>* ModelParser::Parse(const std::string& filepath)
{
	Mesh<TextureVertex>* model = new Mesh<TextureVertex>();

	std::vector<glm::vec3> vertexPositions;
	std::vector<uint16_t> indices;

	std::ifstream file(filepath);

	std::string line;
	while (std::getline(file, line))
	{
		int strpos = line.find(" ");
		std::string firstWord = line.substr(0, strpos);
		
		// A vertex
		if (firstWord == "v")
		{
			// Split the positions into 3 string components
			std::vector<std::string> strComponents;
			split(line.substr(2, line.length()), strComponents, ' ');

			// Convert to floats
			glm::vec3 vertex(std::stof(strComponents[0]), std::stof(strComponents[1]), std::stof(strComponents[2]));

			vertexPositions.push_back(vertex);
		}

		// A triangle
		if (firstWord == "f")
		{
			// Split the 'indicies' into 3 string components
			std::vector<std::string> strComponents;
			split(line.substr(2, line.length()), strComponents, ' ');

			for (int i = 0; i < 3; i++)
			{
				int index = std::stoi(strComponents[i]);
				glm::vec3 position = vertexPositions[index - 1]; // .obj indicies starts at 1 instead of 0

				TextureVertex vertex;
				vertex.position = position;

				model->AddVertex(vertex);
			}
		}
	}

	model->Update();

	return model;
};