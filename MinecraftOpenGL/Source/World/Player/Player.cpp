#include "Player.h"

#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>

#include <iostream>

#include <glm/gtc/matrix_transform.hpp> 

#include "../World.h"
#include "../Chunk/Chunk.h"
#include "../Chunk/ChunkBlock.h"
#include "Crosshair.h"

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
		m_MovementSpeed += 0.025f;
	if (m_Input.IsKeyPressed(GLFW_KEY_Q))
		m_MovementSpeed -= 0.025f;

	UpdateCameraPosition();

	// Chunk loading

	int renderDistance = 8;

	ChunkMap chunks = World::GetChunks();

	//std::cout << "Loaded chunks: " << chunks.size() << "\n";

	for (auto entry : chunks) 
	{
		Chunk* chunk = entry.second;

		glm::vec2 pos = Utils::WorldPositionToChunkPosition(m_Position + 8.0f);

		float left = pos.x - renderDistance - 1;
		float right = pos.x + renderDistance;
		float top = pos.y - renderDistance - 1;
		float bottom = pos.y + renderDistance;

		glm::vec2 chunkPosition = chunk->GetPosition();

		if (chunkPosition.x > left && chunkPosition.x < right && chunkPosition.y > top && chunkPosition.y < bottom)
		{
		}
		else {
			//chunk->m_ShouldBeRemoved = true;
			//std::cout << "removing chunk " << chunkPosition.x << ", " << chunkPosition.y << "\n";
			//World::RemoveChunk(chunk);
		}

		// Check all the chunks in a box around the player
		/*for (int x = m_Position.x - renderDistance * Chunk::Width; x < m_Position.x + renderDistance * Chunk::Width; x += Chunk::Width)
		{
			for (int z = m_Position.z - renderDistance * Chunk::Depth; z < m_Position.z + renderDistance * Chunk::Depth; z += Chunk::Depth)
			{
				glm::ivec2 chunkPosition = Utils::WorldPositionToChunkPosition(glm::vec3(x, 0, z) + 8.0f);
			}
		}

		glm::ivec2 chunkPosition = chunk->GetWorldPosition() + Chunk::Width / 2;

		glm::ivec2 delta = chunkPosition - glm::ivec2(m_Position.x, m_Position.z);
		int distanceToChunk = std::abs(delta.x) + std::abs(delta.y);

		if (distanceToChunk > renderDistance * 2 * Chunk::Width)
		{
			World::RemoveChunk(chunk);
		}*/
	}

	glm::ivec3 position = glm::floor(m_Position);

	// Check all the chunks in a box around the player
	for (int x = position.x - renderDistance * Chunk::Width; x < position.x + renderDistance * Chunk::Width; x += Chunk::Width)
	{
		for (int z = position.z - renderDistance * Chunk::Depth; z < position.z + renderDistance * Chunk::Depth; z += Chunk::Depth)
		{
			glm::ivec2 chunkPos = Utils::WorldPositionToChunkPosition(glm::vec3(x, 0, z) + 8.0f);

			// Create the chunk here if a chunk at this position doesn't exist
			if (!World::ChunkExistsAt(chunkPos))
				World::CreateChunk(chunkPos);
		}
	}

	// Check all the chunks in a box around the player
	/*for (int x = origin.x - renderDistance; x < origin.x + renderDistance; x++)
	{
		for (int z = origin.y - renderDistance; z < origin.y + renderDistance; z++)
		{
			// Create the chunk here if a chunk at this position doesn't exist
			if (!World::ChunkExistsAt(glm::ivec2(x, z)))
				World::CreateChunk(glm::ivec2(x, z));
		}
	}*/

	std::cout << "Y: " << m_Position.y << "\n";

	// Generate the terrain
	for (int x = position.x - renderDistance * Chunk::Width; x < position.x + renderDistance * Chunk::Width; x += Chunk::Width)
	{
		for (int z = position.z - renderDistance * Chunk::Depth; z < position.z + renderDistance * Chunk::Depth; z += Chunk::Depth)
		{
			glm::ivec2 chunkPos = Utils::WorldPositionToChunkPosition(glm::vec3(x, 0, z) + 8.0f);

			Chunk* chunk = World::GetChunkAt(chunkPos);

			if (chunk->m_HasGenerated || chunk->m_IsGenerating) // Don't generate the terrain twice
				continue;

			ChunkAction* nextAction = new ChunkAction(ChunkAction::ActionType::RebuildAdjacentChunks, chunk);
			nextAction->type = ChunkAction::ActionType::RebuildAdjacentChunks;
			nextAction->chunk = chunk;
			nextAction->SetTimestamp();

			chunk->GenerateTerrainThreaded(nextAction);
		}
	}

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

		Chunk* chunk = World::GetChunkAt(Utils::WorldPositionToChunkPosition(raycastPosition));

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
