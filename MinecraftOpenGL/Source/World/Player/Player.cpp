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

	// Get the block that is currently highlighted or looked at
	Raycast raycast(m_Camera.m_Position, m_Camera.m_Front);
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

		m_HighlightedBlock = World::GetBlockAt(glm::floor(raycast.m_CurrentPosition));

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
	m_Camera.m_Position = glm::vec3(m_Position.x, m_Position.y + m_EyeOffset, m_Position.z);
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

		m_Velocity += glm::normalize(glm::cross(m_Camera.m_Front2D, m_Camera.m_Up)) * m_MovementSpeed * deltaTime;
	}
	if (wSInput != 0) {
		m_MovementSpeed += wSInput * m_Acceleration;

		if (m_MovementSpeed > m_MaxVelocity) m_MovementSpeed = m_MaxVelocity;
		if (m_MovementSpeed < -m_MaxVelocity) m_MovementSpeed = -m_MaxVelocity;

		m_Velocity += m_Camera.m_Front2D * m_MovementSpeed * deltaTime;
	}

	if (m_Input.IsKeyPressed(GLFW_KEY_SPACE))
		m_Velocity.y = 10 * deltaTime;

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
	float gravity = -1.0f * deltaTime;

	// Apply gravity and collision detection
	Chunk* chunk = World::GetChunkAt(Utils::WorldPositionToChunkPosition(m_Position));

	if (chunk)
	{
		// Cast a ray downwards to look for a block
		// Downwards if the velocity is negative, and upwards if the velocity is positive
		glm::vec3 rayDirection(glm::vec3(0, Utils::Math::Sign(m_Velocity.y), 0));

		Raycast raycast(m_Position, rayDirection);
		raycast.m_MaxDistance = 2.0f;

		while (raycast.Step(0.01f))
		{
			if (raycast.m_HasReachedMaxDistance)
				raycast.Stop();

			ChunkBlock* blockBelow = World::GetBlockAt(glm::floor(raycast.m_CurrentPosition));

			if (blockBelow)
			{
				if (blockBelow->m_BlockId == BlockIds::Air)
				{
					// Calculate where the position would be next frame
					glm::vec3 newPosition = m_Position + glm::vec3(0.0f, m_Velocity.y + gravity, 0.0f);

					ChunkBlock* potentialBlockBelow = World::GetBlockAt(glm::floor(newPosition));
					if (potentialBlockBelow && potentialBlockBelow->m_BlockId != BlockIds::Air)
					{
						// Calculate the distance between the feet and top of the block below
						float distanceToBlock = m_Position.y - (potentialBlockBelow->GetLocalPosition().y + ChunkBlock::Size);

						m_Velocity.y = -distanceToBlock;
					}
					else
					{
						m_Velocity.y += gravity;
					}
				}
				else
				{
					// Calculate the distance between the feet and top of the block below
					float distanceToBlockBelow = m_Position.y - (blockBelow->GetLocalPosition().y + ChunkBlock::Size);

					// Check if the player will be inside a block after applying gravity (aka the gravity is larger than the distance to the block below)
					if (gravity > distanceToBlockBelow)
					{
						// If too far above block
						if (distanceToBlockBelow > 0)
							m_Velocity.y = -distanceToBlockBelow;
						// If inside block (update the position directly like an impulse instead of force over time)
						else if (distanceToBlockBelow < 0)
							m_Position.y -= distanceToBlockBelow;
					}
					else
					{
						m_Velocity.y = 0.0f;
					}
				}
					
				raycast.Stop();
			}
		}
	}

	m_Position += m_Velocity;
}

void Player::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	float x = (float)xpos;
	float y = (float)ypos;

	if (m_FirstMouse)
	{
		m_LastMouseX = x;
		m_LastMouseY = y;
		m_FirstMouse = false;
	}

	float xoffset = (x - m_LastMouseX) * m_MouseSensitivity;
	float yoffset = (m_LastMouseY - y) * m_MouseSensitivity;

	m_MouseYaw += xoffset;
	m_MousePitch += yoffset;

	if (m_MousePitch > 89.0f)
		m_MousePitch = 89.0f;
	if (m_MousePitch < -89.0f)
		m_MousePitch = -89.0f;

	glm::vec3 direction2D;
	direction2D.x = cos(glm::radians(m_MouseYaw));
	direction2D.y = 0;
	direction2D.z = sin(glm::radians(m_MouseYaw));
	m_Camera.m_Front2D = glm::normalize(direction2D);

	glm::vec3 direction;
	direction.x = direction2D.x * cos(glm::radians(m_MousePitch));
	direction.y = sin(glm::radians(m_MousePitch));
	direction.z = direction2D.z * cos(glm::radians(m_MousePitch));
	m_Camera.m_Front = glm::normalize(direction);
	
	m_LastMouseX = x;
	m_LastMouseY = y;
}

void Player::HandleBlockBreaking()
{
	if (!m_HighlightedBlock) return;

	Chunk* chunk = m_HighlightedBlock->GetChunk();

	// Don't update the block if it's already air
	if (m_HighlightedBlock->m_BlockId != BlockIds::Air)
	{
		m_HighlightedBlock->m_BlockId = BlockIds::Air;

		World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, chunk));

		// Rebuild the adjacent chunk if it exists
		glm::u8vec3 blockPosition = m_HighlightedBlock->GetLocalPosition();
		glm::ivec2 chunkPosition = chunk->GetPosition();
		std::vector<Chunk*> adjacentChunks;

		// Logic to get the chunk at edge depending on the block position
		if (blockPosition.x == 0)
			adjacentChunks.push_back(World::GetChunkAt(chunkPosition + glm::ivec2(-1, 0)));
		if (blockPosition.x == Chunk::Width - 1)
			adjacentChunks.push_back(World::GetChunkAt(chunkPosition + glm::ivec2(1, 0)));
		if (blockPosition.z == 0)
			adjacentChunks.push_back(World::GetChunkAt(chunkPosition + glm::ivec2(0, -1)));
		if (blockPosition.z == Chunk::Depth - 1)
			adjacentChunks.push_back(World::GetChunkAt(chunkPosition + glm::ivec2(0, 1)));

		// Add each of the adjacent chunks to the rebuild queue
		for (unsigned int i = 0; i < adjacentChunks.size(); i++)
			World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, adjacentChunks[i]));
	}
}

void Player::HandleBlockPlacing()
{
	Raycast raycast(m_Camera.m_Position, m_Camera.m_Front);
	raycast.m_MaxDistance = 7.0f;

	while (raycast.Step(0.01f))
	{
		// Reached block breaking limit
		if (raycast.m_HasReachedMaxDistance)
			raycast.Stop();

		glm::vec3 raycastPositionFloored = glm::floor(raycast.m_CurrentPosition);
		ChunkBlock* lookingAtBlock = World::GetBlockAt(raycastPositionFloored);

		// Run next iteration if not looking at a block, or the block exists but is air
		if (!lookingAtBlock || (lookingAtBlock && lookingAtBlock->m_BlockId == BlockIds::Air)) continue;

		ChunkBlock* blockToReplace = nullptr;
		float faceThreshold = 0.05f;

		glm::vec3 raycastPositionDecimals = raycast.m_CurrentPosition - raycastPositionFloored;

		// Determine what face of the block the raycast hit

		// X
		if (raycastPositionDecimals.x < faceThreshold)
			blockToReplace = World::GetBlockAt(raycastPositionFloored + glm::vec3(-1, 0, 0));
		else if (raycastPositionDecimals.x > 1 - faceThreshold)
			blockToReplace = World::GetBlockAt(raycastPositionFloored + glm::vec3(1, 0, 0));

		// Y
		if (raycastPositionDecimals.y < faceThreshold)
			blockToReplace = World::GetBlockAt(raycastPositionFloored + glm::vec3(0, -1, 0));
		else if (raycastPositionDecimals.y > 1 - faceThreshold)
			blockToReplace = World::GetBlockAt(raycastPositionFloored + glm::vec3(0, 1, 0));

		// Z
		else if (raycastPositionDecimals.z < faceThreshold)
			blockToReplace = World::GetBlockAt(raycastPositionFloored + glm::vec3(0, 0, -1));
		else if (raycastPositionDecimals.z > 1 - faceThreshold)
			blockToReplace = World::GetBlockAt(raycastPositionFloored + glm::vec3(0, 0, 1));

		if (blockToReplace)
		{
			blockToReplace->m_BlockId = BlockIds::Gravel;
			World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, blockToReplace->GetChunk()));
		}

		raycast.Stop();
	}
}

void Player::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	// Block breaking
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		HandleBlockBreaking();

	// Block placing
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		HandleBlockPlacing();
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
