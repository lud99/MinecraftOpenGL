#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Vertex
{
	Vertex(
		glm::vec3 position = glm::vec3(0.0f), 
		glm::vec4 color = glm::vec4(1.0f), 
		glm::vec2 textureCoordinates = glm::vec2(0, 0)):

		position(position), 
		color(color), 
		textureCoordinates(textureCoordinates) {}

	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 textureCoordinates;
};