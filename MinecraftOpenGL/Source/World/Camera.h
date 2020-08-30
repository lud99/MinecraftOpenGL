#pragma once

#include <glm/vec3.hpp>

class Camera
{
public:
	Camera();

	glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_Front = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 m_Front2D = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

	~Camera();
};