#pragma once

#include <glm/vec3.hpp>

class Raycast
{
public:
	enum class States {
		ShouldStep,
		ReachedMaxDistance,
		Finished,
	};

public:
	Raycast();
	Raycast(glm::vec3 startPosition, glm::vec3 direction, float maxDistance = 10.0f) : 
		m_StartPosition(startPosition), 
		m_CurrentPosition(startPosition), 
		m_Direction(direction), 
		m_MaxDistance(maxDistance) {};

	bool Step(float stepSize = 1.0f);

	void Stop();

public:
	glm::vec3 m_StartPosition = glm::vec3(0);
	glm::vec3 m_EndPosition = glm::vec3(0);

	glm::vec3 m_CurrentPosition = glm::vec3(0);
	glm::vec3 m_Direction = glm::vec3(0);

	float m_StepResolution = 1.0f;
	float m_MaxDistance = 10.0f;

	States m_State = States::ShouldStep;

	bool m_IsFinished = false;
	bool m_HasReachedMaxDistance = false;
};