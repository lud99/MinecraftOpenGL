#pragma once

#include <glm/vec3.hpp>
#include <Common/NetworkClient.h>

#include "../../InputHandler.h"

#include "Camera.h"
#include "Crosshair.h"

class Chunk;
class ChunkBlock;
struct GLFWwindow;

class Player
{
public:
	Player();
	Player(GLFWwindow* window);

	void Init();

	void OnUpdate();
	void OnFixedUpdate();
	void UpdateCameraPosition();

	void MouseCallback(GLFWwindow* window, double xpos, double ypos);

	void SetWindow(GLFWwindow* window);

	Camera& GetCamera();

	~Player();

private:
	void HandleMovement();
	void HandleCollision();
	void HandleBlockBreaking();
	void HandleBlockPlacing();

public:
	glm::vec3 m_Position = glm::vec3(0.0f, 70.0f, 0.0f);
	glm::vec3 m_LookingAtPosition = glm::vec3(0.0f); 
	glm::vec3 m_LastLookingAtPosition = glm::vec3(0.0f);

	GLFWwindow* m_Window = nullptr;
	Crosshair m_Crosshair;

	int m_ClientId = -1;

private:
	InputHandler m_Input;
	Camera m_Camera;

	ChunkBlock* m_HighlightedBlock = nullptr;
	Chunk* m_HighlightedBlockChunk = nullptr;

	glm::vec3 m_Velocity = glm::vec3(0.0f);
	float m_MovementSpeed = 0.0f;
	float m_Acceleration = 0.0f;
	float m_BaseAcceleration = 8.0f;
	float m_MaxVelocity = 10.0f; // Meters per second
	float m_PreviousBlockFriction = 0.25f;

	bool m_IsStandingOnGround = false;

	float m_EyeOffset = 1.62f;
	float m_Height = 1.85f;

	float m_BlockBreakProgress = 0.0f;
	ChunkBlock* m_PrevBreakingBlock = nullptr;

	// Mouse variables
	// TODO: seperate class
	bool m_FirstMouse = true;
	float m_LastMouseX = 400, m_LastMouseY = 300;
	float m_MouseSensitivity = 0.1f;
	float m_MouseYaw = 0, m_MousePitch = 0;

	bool m_ShouldJumpNextUpdate = false;

	static constexpr int HotbarSlots = 14;
	int m_Hotbar[HotbarSlots];
	int m_CurrentHotbarSlot = 0;
};

