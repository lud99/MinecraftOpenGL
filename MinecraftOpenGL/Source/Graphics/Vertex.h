#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Vertex
{
	Vertex(
		glm::vec3 position = glm::vec3(0),
		glm::vec2 textureCoordinates = glm::vec2(0, 0)):

		position(position), 
		textureCoordinates(textureCoordinates) {}

	glm::vec3 position;
	glm::vec2 textureCoordinates;
};