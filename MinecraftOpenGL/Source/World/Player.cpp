#include "Player.h"

#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>

#include <iostream>

#include <glm/gtc/matrix_transform.hpp> 

#include "World.h"
#include "Chunk/Chunk.h"
#include "Chunk/ChunkBlock.h"
#include "../Crosshair.h"

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
	m_Crosshair = new Crosshair();
}

void Player::Update()
{
	// Forward, Backward
	if (m_Input.IsKeyPressed(GLFW_KEY_W))
		m_Position += m_MovementSpeed * m_Camera.m_Front2D;
	if (m_Input.IsKeyPressed(GLFW_KEY_S))
		m_Position -= m_MovementSpeed * m_Camera.m_Front2D;

	// Left, Right
	if (m_Input.IsKeyPressed(GLFW_KEY_A))
		m_Position -= glm::normalize(glm::cross(m_Camera.m_Front2D, m_Camera.m_Up)) * m_MovementSpeed;
	if (m_Input.IsKeyPressed(GLFW_KEY_D))
		m_Position += glm::normalize(glm::cross(m_Camera.m_Front2D, m_Camera.m_Up)) * m_MovementSpeed;

	// Up, Down
	if (m_Input.IsKeyPressed(GLFW_KEY_SPACE))
		m_Position.y += m_MovementSpeed;
	if (m_Input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
		m_Position.y -= m_MovementSpeed;

	// Increase and decrease movement speed
	if (m_Input.IsKeyPressed(GLFW_KEY_E))
		m_MovementSpeed += 0.0025f;
	if (m_Input.IsKeyPressed(GLFW_KEY_Q))
		m_MovementSpeed -= 0.0025f;

	UpdateCameraPosition();

	// Raycast
	float maxDistance = 7.0f;
	float resolution = 0.1f;

	glm::vec3 raycastPosition = m_Position;

	while (true)
	{
		raycastPosition += m_Camera.m_Front * resolution;

		glm::vec3 diff = raycastPosition - m_Position;
		float distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

		if (distanceSquared > maxDistance * maxDistance)
		{
			m_LookingAtPosition = raycastPosition;
			m_HighlightedBlock = nullptr;
			World::m_LookingAtCollider.m_Enabled = false;

			break;
		}

		Chunk* chunk = World::GetChunkAtPosition(Utils::WorldPositionToChunkPosition(raycastPosition));

		if (!chunk) continue;

		glm::ivec3 localRaycastPosition = glm::mod(glm::floor(raycastPosition), glm::vec3(Chunk::Width, Chunk::Height, Chunk::Depth));

		//std::cout << localRaycastPosition.x << ", " << localRaycastPosition.y << ", " << localRaycastPosition.z << "\n";

		m_HighlightedBlock = chunk->GetBlockAt(localRaycastPosition);

		if (m_HighlightedBlock && m_HighlightedBlock->m_BlockId != BlockIds::Air)
		{
			World::m_LookingAtCollider.m_Enabled = true;
			World::m_LookingAtCollider.m_Position = glm::floor(raycastPosition) + 0.5f;

			m_LookingAtPosition = raycastPosition;

			break;
		}
	}
}

void Player::UpdateCameraPosition()
{
	m_Camera.m_Position = m_Position;
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

		World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::Rebuild, chunk));

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
