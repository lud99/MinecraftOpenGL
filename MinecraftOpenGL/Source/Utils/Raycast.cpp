#include "Raycast.h"

bool Raycast::Step(float stepSize)
{
	// Stop stepping
	if (m_IsFinished) return false;

	// Step forward
	m_CurrentPosition += m_Direction * stepSize;

	// Calculate the distance
	glm::vec3 diff = m_CurrentPosition - m_StartPosition;
	float distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

	if (distanceSquared > m_MaxDistance * m_MaxDistance) // Has reached max distance
		m_HasReachedMaxDistance = true;

	return true;
}

void Raycast::Stop()
{
	m_IsFinished = true;

	m_EndPosition = m_CurrentPosition;
}