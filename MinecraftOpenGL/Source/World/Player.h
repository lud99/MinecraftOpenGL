#pragma once

#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>

#include "../InputHandler.h"

#include "Camera.h"

class ChunkBlock;
class Crosshair;

class Player
{
public:
	Player();
	Player(GLFWwindow* window);

	void Init();

	void Update();
	void UpdateCameraPosition();

	void MouseCallback(GLFWwindow* window, double xpos, double ypos);
	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	void SetWindow(GLFWwindow* window);

	Camera& GetCamera();
	inline Crosshair* GetCrosshair() { return m_Crosshair; }

	~Player();
public:
	glm::vec3 m_Position = glm::vec3(0.0f, 70.0f, 0.0f);
	glm::vec3 m_LookingAtPosition, m_LastLookingAtPosition;

	GLFWwindow* m_Window;

private:
	InputHandler m_Input;
	Camera m_Camera;
	Crosshair* m_Crosshair;

	ChunkBlock* m_HighlightedBlock;

	float m_MovementSpeed = 0.05f;

	float lastX = 400, lastY = 300;
	float yaw = 0, pitch = 0;

	bool firstMouse = true;
};

