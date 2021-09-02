#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp> 

Camera::Camera()
{
}

void Camera::UpdateMatrices(int windowWidth, int windowHeight)
{
	m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), (float)windowWidth / (float)windowHeight, m_Near, m_Far);

	m_UiProjectionMatrix = glm::ortho(0.0f, (float)windowWidth, (float)-windowHeight, 0.0f, -1.0f, 1.0f);

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

Camera::~Camera()
{
}
