#include <stdio.h>
#include <enet/enet.h>
#include <iostream>
#include <optick.h>

#include <Common/json.hpp>

#include <Common/Net/NetworkMessages.h>
#include <Common/Net/NetworkSession.h>
#include <Common/Net/NetworkClient.h>
#include <Common/DebugConsole.h>

#include "Net/ServerNetworkThread.h"

#include "World/ServerWorld.h"

int main(int argc, char** argv)
{
	Console::m_Enabled = true;

	// Start the server
	NetworkThread& net = NetworkThread::Get();
	bool serverIsRunning = net.StartServer(7777);

	while (true)
	{
		//OPTICK_FRAME("MainThread");
		
		//net.PullPackets();

		// Iterate through all sessions and update the worlds
		for (auto& entry : net.m_Sessions)
		{
			ServerWorld* world = (ServerWorld*)entry.second.m_World;
			if (!world) continue;

			if (!world->m_IsInitialized) world->OnInit();

			world->CommonOnUpdate();

			// Todo: tick update
		}

		Console::Render();
	}

	net.m_Thread.join();

	return 0;
}
