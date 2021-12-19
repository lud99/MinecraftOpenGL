#include "ModelParser.h"

#include <iostream>
#include <fstream>
#include <vector>

#include <Common/DebugConsole.h>

#include <glm/vec3.hpp>

#include "Mesh.hpp"

static std::vector<std::string> split(const std::string& txt, char ch)
{
	std::vector<std::string> strs;

	size_t pos = txt.find(ch);
	size_t initialPos = 0;

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

	return strs;
}

void ModelParser::Parse(const std::string& filepath, Mesh<TextureVertex>& model)
{
	std::vector<glm::vec3> vertexPositions;
	std::vector<glm::vec2> textureCoordinates;
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
			std::vector<std::string> strComponents = split(line.substr(2, line.length()), ' ');

			// Convert to floats
			glm::vec3 vertex(std::stof(strComponents[0]), std::stof(strComponents[1]), std::stof(strComponents[2]));

			vertexPositions.push_back(vertex);
		}

		// A texture coordinate
		if (firstWord == "vt")
		{
			// Split the positions into 2 string components
			std::vector<std::string> strComponents = split(line.substr(3, line.length()), ' ');

			// Convert to floats
			glm::vec2 textureCoordinate(std::stof(strComponents[0]), std::stof(strComponents[1]));

			textureCoordinates.push_back(textureCoordinate);
		}

		// A triangle
		if (firstWord == "f") // vertex_index/texture_index/normal_index
		{
			// Split the 'indicies' into 3 string components
			std::vector<std::string> strComponents = split(line.substr(2, line.length()), ' ');

			for (int i = 0; i < 3; i++)
			{
				int vertexIndex = std::stoi(split(strComponents[i], '/')[0]);
				int textureIndex = std::stoi(split(strComponents[i], '/')[1]);
				int normalIndex = std::stoi(split(strComponents[i], '/')[2]);

				glm::vec3 position = vertexPositions[vertexIndex - 1]; // .obj indicies starts at 1 instead of 0
				glm::vec2 textureCoordinate = textureCoordinates[textureIndex - 1]; // .obj indicies starts at 1 instead of 0

				TextureVertex vertex;
				vertex.position = position;
				vertex.textureCoord = textureCoordinate;

				model.AddVertex(vertex);
			}
		}
	}

	Console::Log("Resources") << "Loaded model " << filepath;

	model.m_Texture = new Texture2D("Resources/Models/Door/top.png");
}