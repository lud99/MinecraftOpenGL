#include "INetworkThread.h"

#include <iostream>
#include <optick.h>

#include <glm/vec2.hpp>

#include <enet/enet.h>
#include "NetworkConstants.h"
#include <Common/ThreadPool.h>
#include <Common/Player/IPlayer.h>

#include <Common/DebugConsole.h>

INetworkThread::INetworkThread()
{
}

void INetworkThread::SetupThread()
{
	// Setup send thread
	m_Thread = std::thread([this]() {
		OPTICK_THREAD("NetworkThread");

		// Lower means packets are send and recieved more often, but will take up bandwidth and (maybe result in packet loss)
		int netRefreshRateMs = 1000/60; // 60fps

		while (true)
		{
			if (m_ShouldExit) break;

			std::this_thread::sleep_for(std::chrono::milliseconds(netRefreshRateMs));

			PullPackets();

			std::vector<NetworkPacket> queue = m_SendQueue; // Make a local copy

			// Send all queued packets
			for (size_t i = 0; i < queue.size(); i++)
			{
				NetworkPacket& entry = queue[i];
				//ENetPacket* packet = enet_packet_create(entry.data.c_str(), entry.data.length() + 1, ENET_PACKET_FLAG_RELIABLE);
				//enet_peer_send(entry.client->m_Peer, 0, packet);
				m_SendingOnChannel++;
				if (m_SendingOnChannel >= ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT) m_SendingOnChannel = 0;

				//std::cout << "Sent\n";
			}

			// Reset queue
			m_QueueLock.lock();
			if (!m_SendQueue.empty()) m_SendQueue.clear();
			m_QueueLock.unlock();
		}
	});
}

void INetworkThread::SendJson(json& message, NetworkClient* client)
{
	std::string s = message["Type"];
#ifdef SERVER_BUILD
	Console::Log("Sent Packets", message["Type"]) << client->m_Id;
	//std::cout << "Sending message " + s +" to client " + std::to_string(client->m_Id) + std::string("... ");
#else
	Console::Log("Sent Packets", message["Type"]) << message["Data"];
#endif

	std::string stringified = message.dump();

	SendString(stringified, client);
}

void INetworkThread::SendString(const std::string& data, NetworkClient* client)
{
	OPTICK_EVENT();
//	m_QueueLock.lock();
	ENetPacket* packet = enet_packet_create(data.c_str(), data.length() + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(client->m_Peer, 0, packet);
	
	//std::cout << "Sending message " << data << "\n";
	//m_SendQueue.emplace_back(data, conn);
	//m_QueueLock.unlock();
}

void INetworkThread::Broadcast(json& packet, const std::string& sessionName, int clientToExclude)
{
	if (m_Sessions.count(sessionName) == 0)
	{
		Console::Log("Errors") << "Tried broadcasting to session " + sessionName + " that doesn't exist";
		return;
	}

	PlayersMap players = m_Sessions[sessionName].m_World->GetPlayers();
	for (auto& entry : players)
	{
		IPlayer* player = entry.second;
		if (!player) continue;

		// Exclude the player that for example sent the original message
		if (player->m_NetClient->m_Id == clientToExclude)
			continue;

		// Send to this client
		SendJson(packet, player->m_NetClient);
	}
}

ENetPacket* INetworkThread::PullPackets()
{
	OPTICK_EVENT();
	ENetEvent event;

	// The incoming messages are handled by this function. Needs to be called regularly. A 0 timeout means that it's non-blocking
	int eventStatus = enet_host_service(m_EnetClient, &event, 0);

	if (eventStatus > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT: { // Only used for server
			printf("A new client connected from %x:%u\n", event.peer->address.host, event.peer->address.port);
			break;
		}
		case ENET_EVENT_TYPE_RECEIVE:
		{
			json packet = json::parse((const char*)event.packet->data);

			// For the server, create a NetworkClient and attatch it to the enet peer, for easy access
#ifdef SERVER_BUILD
			NetworkClient* conn = (NetworkClient*)event.peer->data;
			if (!conn) conn = new NetworkClient(event.peer); // Create a default client
			event.peer->data = (void*)conn;

			HandlePacket(packet, conn, event.peer);
#else
			// For the client, use the 'me' client member variable in the ClientNetworkThread
			HandlePacket(packet, nullptr /* to use the 'me' client*/, event.peer);
#endif

			enet_packet_destroy(event.packet);
			break;
		}

		case ENET_EVENT_TYPE_DISCONNECT: // Only used for server
			std::cout << "Client disconnected\n";
			// Reset client's information
			event.peer->data = nullptr;
			break;
		}
	}
	else if (eventStatus < 0)
	{
		std::cout << "Err packet pull failed\n";
	}

	// Send queded packets?
	enet_host_flush(m_EnetClient);

	return nullptr;
}

	/*NetworkClient* client = (NetworkClient*)event.peer->data;

	glm::ivec2 chunkPos(message["Data"]["Position"]["X"], message["Data"]["Position"]["Z"]);

	Chunk* chunk = World::GetChunkAt(chunkPos);

	ChunkAction action(ChunkAction::ActionType::Serialize, chunk, ChunkAction::Priority::High);
	action.extraData = (void*)client;

	action.runWhenDone = [](ChunkAction _action, void* result) {
		NetworkClient* client = (NetworkClient*)_action.extraData;

		const char* string = (const char*)result;
		std::string json = string;

		std::cout << "Sending...";

		INetworkThread& net = net.Instance();
		net.SendString(json, client->m_Peer);
	};

	World::m_ChunkBuilder.AddToQueue(action);*/


INetworkThread::~INetworkThread()
{
	enet_host_destroy(m_EnetClient);
	enet_deinitialize();
	if (m_Thread.joinable())
		m_Thread.join();
}