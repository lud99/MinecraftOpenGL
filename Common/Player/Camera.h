#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera
{
public:
	Camera();

	void UpdateMatrices(int windowWidth, int windowHeight);

	~Camera();
public:


	glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_Front = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 m_Front2D = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 m_ProjectionMatrix = glm::mat4(1.0);
	glm::mat4 m_UiProjectionMatrix = glm::mat4(1.0);
	glm::mat4 m_ViewMatrix = glm::mat4(1.0);

	// Measured in degrees
	float m_Fov = 70;

	float m_Near = 0.1f;
	float m_Far = 2500.0f;
};