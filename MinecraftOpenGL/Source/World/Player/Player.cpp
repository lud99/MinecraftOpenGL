#include "Player.h"

#include <glm/vec3.hpp>

#include <iostream>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp> 

#include "../Blocks/Blocks.h"
#include "../World.h"
#include "../Chunk/Chunk.h"
#include "../Chunk/ChunkBlock.h"
#include "../Chunk/BlockEntity.h"
#include "../../Utils/Raycast.h"
#include "../../Utils/Utils.h"

#include <GLFW/glfw3.h>

class ChunkChestBlock : public ChunkBlock
{
public:
	inline int AMethod() { return 42; }

public:
	glm::ivec2 pos;
};

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
	m_Crosshair.Init();
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
	if (m_Input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
		m_Velocity.y = -10 * deltaTime;

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
	m_Velocity.y += gravity;

	// Check for collision with block below
	if (m_Velocity.y < 0)
	{
		ChunkBlock* blockBelow = World::GetBlockAt(glm::floor(m_Position - glm::vec3(0.0f, ChunkBlock::Size, 0.0f)));

		// Only check collision with solid blocks
		if (blockBelow && blockBelow->m_BlockId != BlockIds::Air)
		{
			// Calculate the distance between the feet and top of the block below
			float distanceToBlock = m_Position.y - (blockBelow->GetLocalPosition().y + ChunkBlock::Size);

			// Check if the player will be inside the top block after the velocity (aka the velocity is larger than the distance to the block above)
			if (std::abs(m_Velocity.y) > distanceToBlock)
				m_Velocity.y = -distanceToBlock;
		}
	}

	// Check for collision with block above
	if (m_Velocity.y > 0)
	{
		// A margin so that when the block pos is floored, it always rounds to the block directly above and never 2 blocks above when exactly below it
		float smallMargin = 0.1f;
		ChunkBlock* blockAbove = World::GetBlockAt(glm::floor(m_Position + glm::vec3(0.0f, m_Height + ChunkBlock::Size - smallMargin, 0.0f)));

		// Only check collision with solid blocks
		if (blockAbove && blockAbove->m_BlockId != BlockIds::Air)
		{
			// Calculate the distance between the head and bottom of the block above
			float distanceToBlock = blockAbove->GetLocalPosition().y - (m_Position.y + m_Height);

			// Check if the player will be inside the top block after the velocity (aka the velocity is larger than the distance to the block above)
			if (std::abs(m_Velocity.y) > distanceToBlock)
				m_Velocity.y = distanceToBlock;
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

	//Chunk* chunk = m_HighlightedBlock->GetChunk();

	/*switch (m_HighlightedBlock->m_BlockId)
	{
	case BlockIds::Stone: 
		((Blocks::StoneBlock*) m_HighlightedBlock)->OnBlockClick(0, 0, 0);

		break;
			
	default:*/
	//m_HighlightedBlock->OnBlockClick(0, 0, 0);
	//}

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
			//blockToReplace->m_BlockId = BlockIds::OakLeaves;

			/*BlockEntity* chest = new BlockEntity();
			chest->m_BlockId = BlockIds::Chest;
			chest->SetLocalPosition(blockToReplace->GetLocalPosition());
			chest->m_ChunkPosition = blockToReplace->GetChunk()->GetPosition();

			BlockEntitiesMap& entities = blockToReplace->GetChunk()->m_BlockEntities;
			uint16_t index = Utils::BlockPositionToIndex(blockToReplace->GetLocalPosition());
			entities[index] = chest;*/

			/*ChunkChestBlock* chest = new ChunkChestBlock();
			chest->m_BlockId = BlockIds::OakLeaves;
			chest->SetLocalPosition(blockToReplace->GetLocalPosition());
			chest->m_ChunkPosition = blockToReplace->GetChunk()->GetPosition();

			blockToReplace->GetChunk()->SetBlockAt((glm::ivec3)blockToReplace->GetLocalPosition(), (ChunkBlock*)chest);
			ChunkChestBlock* b = (ChunkChestBlock*) blockToReplace->GetChunk()->GetBlockAt(blockToReplace->GetLocalPosition());

			std::cout << b->AMethod() << "\n";
			std::cout << (b->pos).x << "\n";
			
			World::m_ChunkBuilder.AddToQueue(ChunkAction(ChunkAction::ActionType::Rebuild, blockToReplace->GetChunk()));*/
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
