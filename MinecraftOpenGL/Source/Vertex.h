#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Vertex
{
	Vertex(glm::vec3 position = glm::vec3(0.0f), glm::vec4 color = glm::vec4(1.0f)) : 
		position(position), color(color) {}

	glm::vec3 position;
	glm::vec4 color;
};