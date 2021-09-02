#pragma once

#include <Common/Player/IPlayer.h>

#include <glm/vec3.hpp>

#include "../../InputHandler.h"

#include "Crosshair.h"

class Chunk;
class ChunkBlock;
class NetworkClient;
struct GLFWwindow;

class ClientPlayer : public IPlayer
{
public:
	ClientPlayer();

	void OnInit() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;
	void OnTickUpdate() override;
	void UpdateCameraPosition();

	void MouseCallback(GLFWwindow* window, double xpos, double ypos);

	static Camera& GetCamera();

	~ClientPlayer();

private:
	void HandleMovement();
	void HandleCollision();
	void HandleBlockBreaking();
	void HandleBlockPlacing();

public:
	//glm::vec3 m_Position = glm::vec3(0.0f, 70.0f, 0.0f);
	glm::vec3 m_LookingAtPosition = glm::vec3(0.0f); 
	glm::vec3 m_LastLookingAtPosition = glm::vec3(0.0f);
	Crosshair m_Crosshair;

	Mesh<TextureVertex>* m_PlayerModel;

	// If this player is the real player that should do logic
	bool m_IsMe = false;

private:
	ChunkBlock* m_HighlightedBlock = nullptr;
	Chunk* m_HighlightedBlockChunk = nullptr;

	//glm::vec3 m_Velocity = glm::vec3(0.0f);
	//float m_MovementSpeed = 0.0f;
	//float m_Acceleration = 0.0f;
	//float m_BaseAcceleration = 8.0f;
	//float m_MaxVelocity = 10.0f; // Meters per second
	//float m_PreviousBlockFriction = 0.25f;

	//bool m_IsStandingOnGround = false;

	//float m_EyeOffset = 1.62f;
	//float m_Height = 1.85f;

	float m_BlockBreakProgress = 0.0f;
	ChunkBlock* m_PrevBreakingBlock = nullptr;

	// Mouse variables
	// TODO: seperate class
public:
	bool m_FirstMouse = true;
	float m_LastMouseX = 400, m_LastMouseY = 300;
	float m_MouseSensitivity = 0.1f;
	float m_MouseYaw = 0, m_MousePitch = 0;

private:
	bool m_ShouldJumpNextUpdate = false;

	static Camera m_Camera;
};

