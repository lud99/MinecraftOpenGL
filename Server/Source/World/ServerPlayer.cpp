#include "ServerPlayer.h"

#include <glm/vec3.hpp>

#include <iostream>
#include <algorithm>

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

ServerPlayer::ServerPlayer()
{
}

void ServerPlayer::OnInit()
{

}

void ServerPlayer::OnUpdate()
{
	if (!m_IsInitialized)
		Init();
}

void ServerPlayer::OnFixedUpdate()
{
	/*HandleMovement();
	HandleCollision();

	UpdateCameraPosition();*/
}

void ServerPlayer::OnTickUpdate()
{
	
}

ServerPlayer::~ServerPlayer()
{
}
