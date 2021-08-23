#pragma once

#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/hash.hpp>

#include <mutex>

#include <Common/World/IWorld.h>
#include <Common/Utils/Utils.h>
#include <Graphics/Textures/TextureAtlas.h>
#include <Graphics/Mesh.hpp>
#include <Graphics/BasicVertices.h>
#include "Collider.h"
#include "Player/ClientPlayer.h"
#include <Common/Chunk/ChunkBuilder.h>

class Chunk;
class WorldRenderer;
class ClientPlayer;

namespace irrklang
{
	class ISoundEngine;
};

class ClientWorld : public IWorld
{
public:
	virtual void OnInit() override;
	virtual void OnUpdate() override;
	virtual void OnFixedUpdate() override;
	virtual void OnTickUpdate() override;

	void Render();

	void HandleCreatingNewChunks() override;
	//void UnloadChunksOutsideRenderDistance();

public:
	ClientPlayer* m_LocalPlayer = nullptr;

	Collider m_LookingAtCollider;
	WorldRenderer* m_Renderer = nullptr;

	irrklang::ISoundEngine* SoundEngine;

private:

	/*namespace {
		GLFWwindow* m_Window;

		ChunkMap m_Chunks;

		Player m_Player;

		std::recursive_mutex m_ChunkMutex;
	}*/
};

namespace Settings 
{
	extern int RenderDistance;
}