#pragma once

#include <glm/vec3.hpp>

#include "../../InputHandler.h"

#include "Camera.h"

class ChunkBlock;
struct GLFWwindow;

class Player
{
public:
	Player();
	Player(GLFWwindow* window);

	void Init();

	void Update(float deltaTime);
	void UpdateCameraPosition();

	void MouseCallback(GLFWwindow* window, double xpos, double ypos);
	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	void SetWindow(GLFWwindow* window);

	Camera& GetCamera();

	~Player();

private:
	void HandleMovement(float deltaTime);
	void HandleCollision(float deltaTime);
	void HandleBlockBreak(float deltaTime);

public:
	glm::vec3 m_Position = glm::vec3(0.0f, 70.0f, 0.0f);
	glm::vec3 m_LookingAtPosition = glm::vec3(0); 
	glm::vec3 m_LastLookingAtPosition = glm::vec3(0);

	GLFWwindow* m_Window = nullptr;

private:
	InputHandler m_Input;
	Camera m_Camera;

	ChunkBlock* m_HighlightedBlock = nullptr;

	float m_MovementSpeed = 0.0f;
	glm::vec3 m_Velocity;

	float lastX = 400, lastY = 300;
	float yaw = 0, pitch = 0;

	float m_Acceleration = 2.0f;
	float m_MaxVelocity = 0.5f;

	bool firstMouse = true;
};

