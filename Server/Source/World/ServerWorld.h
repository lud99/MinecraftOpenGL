#pragma once

#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/hash.hpp>

#include <mutex>

#include <Common/World/IWorld.h>
#include <Common/Utils/Utils.h>

//#include <Graphics/Mesh.hpp>
//#include <Graphics/BasicVertices.h>
#include "ServerPlayer.h"
#include <Common/Chunk/ChunkBuilder.h>
#include <Common/World/IWorld.h>

class Chunk;
class ServerPlayer;

class ServerWorld : public IWorld
{
public:
	void OnInit() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;
	void OnTickUpdate() override;

	void HandleCreatingNewChunks() override;
	//void UnloadChunksOutsideRenderDistance();

public:

};

namespace Settings 
{
	extern int RenderDistance;
}