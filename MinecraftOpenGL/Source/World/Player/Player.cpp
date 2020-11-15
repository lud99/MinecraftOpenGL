#include "Player.h"

#include <glm/vec3.hpp>

#include <iostream>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp> 

#include "../World.h"
#include "../Chunk/Chunk.h"
#include "../Chunk/ChunkBlock.h"
#include "../../Utils/Raycast.h"
#include "../../Utils/Utils.h"

#include <GLFW/glfw3.h>

Player::Player()
{
}

Player::Player(GLFWwindow* window)
{
	m_Window = window;
	m_Input.SetWindow(window);
}

void Player::Init()
{

}

void Player::Update(float deltaTime)
{
	HandleMovement(deltaTime);
	HandleCollision(deltaTime);

	UpdateCameraPosition();

	// Block breaking
	Raycast raycast(m_Position, m_Camera.m_Front);
	raycast.m_MaxDistance = 7.0f;

	while (raycast.Step(0.1f))
	{
		// Reached block breaking limit
		if (raycast.m_HasReachedMaxDistance)
		{
			m_LookingAtPosition = raycast.m_CurrentPosition;
			m_HighlightedBlock = nullptr;
			World::m_LookingAtCollider.m_Enabled = false;

			raycast.Stop();
		}

		Chunk* chunk = World::GetChunkAt(Utils::WorldPositionToChunkPosition(raycast.m_CurrentPosition));

		if (!chunk) continue;

		glm::ivec3 localRaycastPosition = glm::mod(glm::floor(raycast.m_CurrentPosition), glm::vec3(Chunk::Width, Chunk::Height, Chunk::Depth));

		m_HighlightedBlock = chunk->GetBlockAt(localRaycastPosition);

		if (m_HighlightedBlock && m_HighlightedBlock->m_BlockId != BlockIds::Air)
		{
			World::m_LookingAtCollider.m_Enabled = true;
			World::m_LookingAtCollider.m_Position = glm::floor(raycast.m_CurrentPosition) + 0.5f;

			m_LookingAtPosition = raycast.m_CurrentPosition;

			raycast.Stop();
		}
	}
}

void Player::UpdateCameraPosition()
{
	m_Camera.m_Position = m_Position;
}

void Player::HandleMovement(float deltaTime)
{
	// TODO

	float friction = 0.1f;

	int aDInput = m_Input.IsKeyPressed(GLFW_KEY_D) - m_Input.IsKeyPressed(GLFW_KEY_A);
	int wSInput = m_Input.IsKeyPressed(GLFW_KEY_W) - m_Input.IsKeyPressed(GLFW_KEY_S);
	int shiftSpaceInput = m_Input.IsKeyPressed(GLFW_KEY_SPACE) - m_Input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT);

	if (aDInput != 0) {
		m_MovementSpeed += aDInput * m_Acceleration;

		if (m_MovementSpeed > m_MaxVelocity) m_MovementSpeed = m_MaxVelocity;
		if (m_MovementSpeed < -m_MaxVelocity) m_MovementSpeed = -m_MaxVelocity;

		m_Velocity += glm::normalize(glm::cross(m_Camera.m_Front2D, m_Camera.m_Up)) * m_MovementSpeed;
	}
	if (wSInput != 0) {
		m_MovementSpeed += wSInput * m_Acceleration;

		if (m_MovementSpeed > m_MaxVelocity) m_MovementSpeed = m_MaxVelocity;
		if (m_MovementSpeed < -m_MaxVelocity) m_MovementSpeed = -m_MaxVelocity;

		m_Velocity += m_Camera.m_Front2D * m_MovementSpeed;
	}

	if (m_Input.IsKeyPressed(GLFW_KEY_SPACE))
		m_Velocity.y = 10;

	// Increase and decrease movement speed
	if (m_Input.IsKeyPressed(GLFW_KEY_E))
		m_MaxVelocity += 0.025f;
	if (m_Input.IsKeyPressed(GLFW_KEY_Q))
		m_MaxVelocity -= 0.025f;

	// Lerp movement speed towards 0
	m_Velocity.x = Utils::Math::Lerp(m_Velocity.x, 0, friction);
	m_Velocity.y = Utils::Math::Lerp(m_Velocity.y, 0, friction);
	m_Velocity.z = Utils::Math::Lerp(m_Velocity.z, 0, friction);
}

void Player::HandleCollision(float deltaTime)
{
	float camOffset = 1.5f;
	float gravity = -1.0f;

	// Apply gravity and collision detection
	Chunk* chunk = World::GetChunkAt(Utils::WorldPositionToChunkPosition(m_Position));
	glm::vec3 positionInChunk = glm::mod(glm::floor(m_Position), glm::vec3(Chunk::Width, Chunk::Height, Chunk::Depth));

	if (chunk)
	{
		glm::vec3 raycastStart = m_Position - glm::vec3(0, camOffset, 0);

		// Downwards if the velocity is negative, and upwards if the velocity is positive
		glm::vec3 rayDirection(glm::vec3(0, Utils::Math::Sign(m_Velocity.y), 0));

		Raycast raycast(raycastStart, rayDirection);
		raycast.m_MaxDistance = 2.0f;

		while (raycast.Step(0.1f))
		{
			if (raycast.m_HasReachedMaxDistance)
				raycast.Stop();

			glm::ivec3 localRaycastPosition = glm::mod(glm::floor(raycast.m_CurrentPosition), glm::vec3(Chunk::Width, Chunk::Height, Chunk::Depth));

			ChunkBlock* blockBelow = chunk->GetBlockAt(localRaycastPosition);

			if (blockBelow)
			{
				if (blockBelow->m_BlockId == BlockIds::Air)
					m_Velocity.y += gravity;
				else
					m_Velocity.y = 0.0f;

				raycast.Stop();
			}
		}
	}

	m_Position += m_Velocity * deltaTime;
}

void Player::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos;
	lastX = (float)xpos;
	lastY = (float)ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;


	glm::vec3 direction2D;
	direction2D.x = cos(glm::radians(yaw));
	direction2D.y = 0;
	direction2D.z = sin(glm::radians(yaw));
	m_Camera.m_Front2D = glm::normalize(direction2D);

	glm::vec3 direction;
	direction.x = direction2D.x * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = direction2D.z * cos(glm::radians(pitch));
	m_Camera.m_Front = glm::normalize(direction);
}

void Player::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (!m_HighlightedBlock) return;

		Chunk* chunk = m_HighlightedBlock->GetChunk();
		/*
		glm::vec3 highlightedBlockPosition = m_HighlightedBlock->GetWorldPosition();
		glm::vec3 delta = highlightedBlockPosition - m_Position;

		float distanceToBlock = glm::distance(m_Position, highlightedBlockPosition);

		glm::vec3 directionToBlock(delta / distanceToBlock);

		//glm::vec3 directionToBlock(highlightedBlockPosition / distanceToBlock);*/

		//glm::vec3 chunkLocalLookingAtPosition = glm::mod(m_LookingAtPosition, glm::vec3(Chunk::Width, Chunk::Height, Chunk::Depth));
		//glm::vec3 blockLocalLookingAtPosition = glm::mod(chunkLocalLookingAtPosition, 

		//std::cout << m_LastLookingAtPosition.x << ", " << m_LastLookingAtPosition.y << ", " << m_LastLookingAtPosition.z << "\n";

		//glm::vec3 blockPos = glm::round(m_LastLookingAtPosition);

		ChunkBlock* block = chunk->GetBlockAt(m_HighlightedBlock->GetLocalPosition());
		block->m_BlockId = BlockIds::Air;

		World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, chunk));

		//m_HighlightedBlock->m_BlockId = BlockIds::Air;

		//m_HighlightedBlock->GetChunk()->RebuildMeshThreaded();

		//.Rebuild(chunk);
	}
}
   
void Player::SetWindow(GLFWwindow* window)
{
	m_Window = window;
	m_Input.SetWindow(window);
}

Camera& Player::GetCamera()
{
	return m_Camera;
}

Player::~Player()
{
}
