#include "ClientPlayer.h"

#include <glm/vec3.hpp>

#include <iostream>
#include <algorithm>
#include <optick.h>

#include <glm/gtc/matrix_transform.hpp> 

#include <Common/Blocks/Blocks.h>
#include <Common/World/IWorld.h>
#include <Common/Chunk/Chunk.h>
#include <Common/Chunk/ChunkBlock.h>
//#include "../Chunk/BlockEntity.h"
#include <Common/Utils/Raycast.h>
#include <Common/Utils/Utils.h>
#include <Common/Time.h>
#include <Common/Net/INetworkThread.h>

#include <Common/Net/NetworkClient.h>
#include <Common/World/IWorld.h>

#include <GLFW/glfw3.h>

ClientPlayer::ClientPlayer()
{

}

void ClientPlayer::OnInit()
{
	m_Crosshair.Init();
}

void ClientPlayer::OnUpdate()
{
	OPTICK_EVENT();

	if (!m_IsInitialized)
		Init();

	// Update hotbar slot
	if (InputHandler::IsKeyPressed(GLFW_KEY_1))
		m_CurrentHotbarSlot = 0;
	if (InputHandler::IsKeyPressed(GLFW_KEY_2))
		m_CurrentHotbarSlot = 1;
	if (InputHandler::IsKeyPressed(GLFW_KEY_3))
		m_CurrentHotbarSlot = 2;
	if (InputHandler::IsKeyPressed(GLFW_KEY_4))
		m_CurrentHotbarSlot = 3;
	if (InputHandler::IsKeyPressed(GLFW_KEY_5))
		m_CurrentHotbarSlot = 4;
	if (InputHandler::IsKeyPressed(GLFW_KEY_6))
		m_CurrentHotbarSlot = 5;
	if (InputHandler::IsKeyPressed(GLFW_KEY_7))
		m_CurrentHotbarSlot = 6;
	if (InputHandler::IsKeyPressed(GLFW_KEY_8))
		m_CurrentHotbarSlot = 7;
	if (InputHandler::IsKeyPressed(GLFW_KEY_9))
		m_CurrentHotbarSlot = 8;
	if (InputHandler::IsKeyPressed(GLFW_KEY_0))
		m_CurrentHotbarSlot = 9;
	if (InputHandler::IsKeyPressed(GLFW_KEY_KP_0))
		m_CurrentHotbarSlot = 10;
	if (InputHandler::IsKeyPressed(GLFW_KEY_KP_1))
		m_CurrentHotbarSlot = 11;
	if (InputHandler::IsKeyPressed(GLFW_KEY_KP_2))
		m_CurrentHotbarSlot = 12;
	if (InputHandler::IsKeyPressed(GLFW_KEY_KP_3))
		m_CurrentHotbarSlot = 13;

	// Block breaking
	if (InputHandler::IsMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT))
		HandleBlockBreaking();
	else
		m_BlockBreakProgress = 0.0f;

	if (InputHandler::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
	{
		if (m_HighlightedBlock && m_HighlightedBlockChunk)
		{
			// Remember to de-aloc when done!
			Block* block = m_HighlightedBlock->GetBlock(m_World, m_HighlightedBlockChunk);

			if (block)
			{
				bool doDefault = block->OnBlockLeftClick();
				//if (doDefault) block->Break();

				delete block;
			}
		}
	}
	// Block placing
	if (InputHandler::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
		HandleBlockPlacing();

	if (m_IsStandingOnGround && InputHandler::IsKeyPressed(GLFW_KEY_SPACE))
		m_ShouldJumpNextUpdate = true;

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
			//World::m_LookingAtCollider.m_Enabled = false;

			raycast.Stop();
		}

		glm::ivec3 flooredRayPos = glm::floor(raycast.m_CurrentPosition);

		ChunkBlock* highligtedBlock = m_World->GetBlockAt(flooredRayPos);
		Chunk* highlightedChunk = m_World->GetChunkAt(Utils::WorldPositionToChunkPosition(flooredRayPos));

		if (!highligtedBlock || !highlightedChunk)
		{
			raycast.Stop();
			break;
		}

		if (highligtedBlock && highligtedBlock->m_BlockId != BlockIds::Air)
		{
			// If no block has been highlighted
			if (m_HighlightedBlock == nullptr)
				m_HighlightedBlock = highligtedBlock;

			// Only run if a new block is highlighted
			if (highligtedBlock != m_HighlightedBlock)
			{
				m_HighlightedBlock = highligtedBlock;
				m_HighlightedBlockChunk = highlightedChunk;

				//World::m_LookingAtCollider.m_Enabled = true;
				//World::m_LookingAtCollider.m_Position = (glm::vec3)flooredRayPos + 0.5f;

				m_LookingAtPosition = raycast.m_CurrentPosition;
			}

			raycast.Stop();
		}
	}
}

void ClientPlayer::OnFixedUpdate()
{
	OPTICK_EVENT();
	HandleMovement();
	HandleCollision();

	UpdateCameraPosition();
}

void ClientPlayer::OnTickUpdate()
{
	// Send position
	{
		json msg;
		msg["Type"] = "PlayerPosition";
		msg["Data"]["ClientId"] = m_NetClient->m_Id;
		msg["Data"]["X"] = m_Position.x;
		msg["Data"]["Y"] = m_Position.y;
		msg["Data"]["Z"] = m_Position.z;

		m_NetClient->SendJson(msg);
	}
}

void ClientPlayer::UpdateCameraPosition()
{
	m_Camera.m_Position = glm::vec3(m_Position.x, m_Position.y + m_EyeOffset, m_Position.z);
}

void ClientPlayer::HandleMovement()
{
	OPTICK_EVENT();
	float friction = 0.0f;

	ChunkBlock* blockBelow = m_World->GetBlockAt(glm::floor(m_Position - glm::vec3(0, ChunkBlock::Size, 0)));
	if (blockBelow && blockBelow->m_BlockId != BlockIds::Air)
		friction = blockBelow->GetBlockType()->friction;
	else
		friction = m_PreviousBlockFriction;

	if (friction == 0.0f)
		friction = 0.25f;

	// Make the acceleration based on the friction of the block. 
	// This makes it so it's more slippery to move on ice, compared to dirt
	m_Acceleration = m_BaseAcceleration * (friction == 0.0f ? 0.25f : friction) * (float)Time::FixedTimestep;

	glm::vec3 newVelocity(m_Velocity);
	glm::vec3 newMovementDirection(0.0f);

	if (InputHandler::IsKeyHeld(GLFW_KEY_W))
		newMovementDirection += glm::normalize(m_Camera.m_Front2D);
	if (InputHandler::IsKeyHeld(GLFW_KEY_A))
		newMovementDirection -= glm::normalize(glm::cross(m_Camera.m_Front2D, m_Camera.m_Up));
	if (InputHandler::IsKeyHeld(GLFW_KEY_S))
		newMovementDirection -= glm::normalize(m_Camera.m_Front2D);
	if (InputHandler::IsKeyHeld(GLFW_KEY_D))
		newMovementDirection += glm::normalize(glm::cross(m_Camera.m_Front2D, m_Camera.m_Up));

	// Is undefined if x and z are equal to 0
	if (newMovementDirection.x != 0 || newMovementDirection.z != 0)
		newMovementDirection = glm::normalize(newMovementDirection);

	// Jump
	//if (m_Input.IsKeyPressed(GLFW_KEY_SPACE))
	//	m_Velocity.y = 0.212132034356f;//6.f * Time::FixedTimestep * m_Acceleration;
	if (InputHandler::IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
		m_Velocity.y = -6.f * Time::FixedTimestep * m_Acceleration;

	// Increase and decrease movement speed
	if (InputHandler::IsKeyHeld(GLFW_KEY_E))
	{
		m_MaxVelocity += 50.0f * Time::FixedTimestep;
		m_BaseAcceleration += m_BaseAcceleration * Time::FixedTimestep;
	}
	if (InputHandler::IsKeyHeld(GLFW_KEY_Q))
	{
		m_MaxVelocity -= 50.0f * Time::FixedTimestep;
		m_BaseAcceleration -= m_BaseAcceleration * Time::FixedTimestep;
	}

	// Only jump if on ground
	if (m_IsStandingOnGround && m_ShouldJumpNextUpdate)
	{
		m_Velocity.y = 0.212132034356f; // v0 = sqrt(2*g*h)

		m_ShouldJumpNextUpdate = false;
	}

	// Fly
	if (InputHandler::IsKeyHeld(GLFW_KEY_SPACE))
		m_Velocity.y = 0.212132034356f; // v0 = sqrt(2*g*h)

	m_Velocity += newMovementDirection * m_Acceleration;

	glm::vec3 frictionVelocity(Utils::Math::Lerp(m_Velocity.x, 0.0f, friction), 0.0f, Utils::Math::Lerp(m_Velocity.z, 0.0f, friction));

	float mag = glm::length(frictionVelocity);
	float maxVelocity = m_MaxVelocity * Time::FixedTimestep;

	//std::cout << mag << "; ";
	if (mag > maxVelocity && mag != 0)
	{
		float scale = maxVelocity / mag;

		m_Velocity.x *= scale;
		m_Velocity.z *= scale;
	}
	else
	{
		m_Velocity = glm::vec3(frictionVelocity.x, m_Velocity.y, frictionVelocity.z);
	}

	float velMag = glm::length(glm::vec2(m_Velocity.x, m_Velocity.z));
	if (velMag > maxVelocity && velMag != 0)
	{
		float scale = maxVelocity / velMag;

		m_Velocity.x *= scale;
		m_Velocity.z *= scale;
	}

	m_Velocity.y += IWorld::Gravity * Time::FixedTimestep;

	//std::cout << glm::length(m_Velocity) << "\n";

	//std::cout << World::Gravity * Time::DeltaTime << "; " << World::Gravity * Time::DeltaTime  * 500 << "\n";

	m_PreviousBlockFriction = friction;
}

void ClientPlayer::HandleCollision()
{
	OPTICK_EVENT();
	m_IsStandingOnGround = false;

	// Don't do collision checks if not inside a chunk or it hasn't generated the terrain yet
	Chunk* inChunk = m_World->GetChunkAt(Utils::WorldPositionToChunkPosition(m_Position));
	if (!inChunk || !inChunk->m_HasGenerated)
	{
		m_Velocity.y = 0.0f;//m_Velocity.y = glm::vec3(0.0f);
		m_Position += m_Velocity;
		return;
	}

	// Check for collision with block below
	if (m_Velocity.y < 0)
	{
		ChunkBlock* blockBelow = m_World->GetBlockAt(glm::floor(m_Position - glm::vec3(0.0f, ChunkBlock::Size, 0.0f)));

		// Only check collision with solid blocks
		if (blockBelow && blockBelow->m_BlockId != BlockIds::Air)
		{
			// Calculate the distance between the feet and top of the block below
			float distanceToBlock = m_Position.y - (blockBelow->GetLocalPosition().y + ChunkBlock::Size);

			// Check if the player will be inside the top block after the velocity (aka the velocity is larger than the distance to the block above)
			if (std::abs(m_Velocity.y) > distanceToBlock)
				m_Velocity.y = -distanceToBlock;

			m_IsStandingOnGround = true;
		}
	}

	// Check for collision with block above
	if (m_Velocity.y > 0)
	{
		// A margin so that when the block pos is floored, it always rounds to the block directly above and never 2 blocks above when exactly below it
		float smallMargin = 0.1f;
		ChunkBlock* blockAbove = m_World->GetBlockAt(glm::floor(m_Position + glm::vec3(0.0f, m_Height + ChunkBlock::Size - smallMargin, 0.0f)));

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

void ClientPlayer::MouseCallback(GLFWwindow* window, double xpos, double ypos)
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

	glm::vec3 direction2D(0.0f);
	direction2D.x = cos(glm::radians(m_MouseYaw));
	direction2D.y = 0;
	direction2D.z = sin(glm::radians(m_MouseYaw));
	m_Camera.m_Front2D = glm::normalize(direction2D);

	glm::vec3 direction(0.0f);
	direction.x = direction2D.x * cos(glm::radians(m_MousePitch));
	direction.y = sin(glm::radians(m_MousePitch));
	direction.z = direction2D.z * cos(glm::radians(m_MousePitch));
	m_Camera.m_Front = glm::normalize(direction);
	
	m_LastMouseX = x;
	m_LastMouseY = y;
}

void ClientPlayer::HandleBlockBreaking()
{
	OPTICK_EVENT();

	float blockBreakSpeed = 6.0f * Time::DeltaTime;
	if (!m_HighlightedBlock)
	{
		m_BlockBreakProgress = 0.0f;

		return;
	}

	// Reset break progess if the highlighted block has changed
	if (m_HighlightedBlock != m_PrevBreakingBlock)
		m_BlockBreakProgress = 0.0f;

	m_BlockBreakProgress += blockBreakSpeed;

	if (m_BlockBreakProgress >= 1.0f)
	{
		m_BlockBreakProgress = 0.0f;

		// Remember to de-aloc when done!
		Block* block = m_HighlightedBlock->GetBlock(m_World, m_HighlightedBlockChunk);

		if (!block) return;

		bool doDefault = block->OnBlockClick();
		if (doDefault) block->Break();

		delete block;
	}

	m_PrevBreakingBlock = m_HighlightedBlock;
}

void ClientPlayer::HandleBlockPlacing()
{
	OPTICK_EVENT();

	//if (World::ChunkExistsAt(glm::vec2(0, 0)))
	//{


	//	if (m_HighlightedBlock && m_HighlightedBlockChunk) {
	//		float dx = m_Position.x * m_Camera.m_Front.x - m_HighlightedBlock->GetWorldPosition(m_HighlightedBlockChunk).x - 0.5f;
	//		float dy = m_Position.z * m_Camera.m_Front.z - m_HighlightedBlock->GetWorldPosition(m_HighlightedBlockChunk).z - 0.5f;
	//		float angle = std::atan2(dy, dx) * (180 / 3.14f);
	//		std::cout << angle << "\n";

	//		if (angle < 45) {
	//			BlockTypes::Blocks[DoorBottom].faces[BasicVertices::Cube::Faces::Left].textureId = TextureIds::DoorBottom;
	//			BlockTypes::Blocks[DoorBottom].faces[BasicVertices::Cube::Faces::Right].textureId = -1;
	//			BlockTypes::Blocks[DoorBottom].faces[BasicVertices::Cube::Faces::Top].textureId = -1;
	//			BlockTypes::Blocks[DoorBottom].faces[BasicVertices::Cube::Faces::Bottom].textureId = -1;
	//			BlockTypes::Blocks[DoorBottom].faces[BasicVertices::Cube::Faces::Front].textureId = -1;
	//			BlockTypes::Blocks[DoorBottom].faces[BasicVertices::Cube::Faces::Back].textureId = -1;
	//		}
	//		else if (angle > 45) {
	//			BlockTypes::Blocks[DoorBottom].faces[BasicVertices::Cube::Faces::Left].textureId = -1;
	//			BlockTypes::Blocks[DoorBottom].faces[BasicVertices::Cube::Faces::Right].textureId = -1;
	//			BlockTypes::Blocks[DoorBottom].faces[BasicVertices::Cube::Faces::Top].textureId = -1;
	//			BlockTypes::Blocks[DoorBottom].faces[BasicVertices::Cube::Faces::Bottom].textureId = -1;
	//			BlockTypes::Blocks[DoorBottom].faces[BasicVertices::Cube::Faces::Front].textureId = -1;
	//			BlockTypes::Blocks[DoorBottom].faces[BasicVertices::Cube::Faces::Back].textureId = TextureIds::DoorBottom;
	//		}

	//		//m_HighlightedBlock->m_BlockId = BlockIds::DoorBottom;
	//		m_HighlightedBlockChunk->GetBlockAt(m_HighlightedBlock->GetLocalPosition() + glm::u8vec3(0, 1, 0))->m_BlockId = BlockIds::DoorBottom;
	//		m_HighlightedBlockChunk->SetDirty(true);
	//	}
	//}

	Raycast raycast(m_Camera.m_Position, m_Camera.m_Front);
	raycast.m_MaxDistance = 7.0f;

	while (raycast.Step(0.01f))
	{
		// Reached block breaking limit
		if (raycast.m_HasReachedMaxDistance)
			raycast.Stop();

		glm::vec3 raycastPositionFloored = glm::floor(raycast.m_CurrentPosition);
		ChunkBlock* block = m_World->GetBlockAt(raycastPositionFloored);

		// Run next iteration if not looking at a block, or the block exists but is air
		if (!block) continue;
		if (block->m_BlockId == BlockIds::Air) continue;

		// A block has been reached, so do the block placing
		ChunkBlock* blockToReplace = m_World->GetBlockAt(raycast.GetPreviousPosition());

		if (blockToReplace)
		{
			Block* block = blockToReplace->GetBlock(m_World, m_HighlightedBlockChunk);

			if (block)
			{
				bool doDefault = block->OnBlockRightClick();
				if (doDefault)
				{
					blockToReplace->m_BlockId = m_Hotbar[m_CurrentHotbarSlot];
					m_HighlightedBlockChunk->SetDirty(true);
				}

				delete block;
			}
			else 
			{
				blockToReplace->m_BlockId = m_Hotbar[m_CurrentHotbarSlot];
				m_HighlightedBlockChunk->SetDirty(true);
			}
		}

		raycast.Stop();
	}
}
   
ClientPlayer::~ClientPlayer()
{
}
