#include "NetworkThread.h"

#include <iostream>

#include <glm/vec2.hpp>

#include <enet/enet.h>
#include <Common/NetworkConstants.h>

#include "World/Chunk/Chunk.h"
#include "World/Chunk/ChunkBlock.h"

NetworkThread::NetworkThread()
{
}

NetworkThread& NetworkThread::Instance()
{
	static NetworkThread instance;
	return instance;
}

bool NetworkThread::Connect(const std::string& adress, int port)
{
	m_Adress = adress;
	m_Port = port;

	bool success = false;
	NetworkThread::_ConnectThread(&success);

	// Setup send thread
	m_Thread = std::thread([this]() {
		while (true)
		{
			if (m_ShouldExit) break;

			PullPackets();

			std::vector<std::string> queue = m_SendQueue;

			// Send all queued packets
			for (size_t i = 0; i < queue.size(); i++)
			{
				ENetPacket* packet = enet_packet_create(queue[i].c_str(), queue[i].length() + 1, ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(m_Connection, 0, packet);
			}

			// Reset queue
			if (!m_SendQueue.empty()) m_SendQueue.clear();
		}
	});

	return success;
}

void NetworkThread::SendJson(json& message)
{
	std::cout << "Sending message " << message["Type"] << "\n";
	std::string stringified = message.dump();

	SendString(stringified);
}

void NetworkThread::SendString(const std::string& data)
{
	//std::cout << "Sending message: " << data << "\n";

	m_SendQueue.push_back(data);
}

bool NetworkThread::_ConnectThread(bool* success)
{
	enet_initialize();

	m_Client = enet_host_create(NULL, MaxClients, Channels, MaxInBandwidth, MaxOutBandwidth);

	if (m_Client == NULL) std::cout << "An error occurred while trying to create an ENet client host.\n";

	ENetAddress address;
	ENetEvent event;
	ENetPeer* peer = nullptr;

	enet_address_set_host(&address, m_Adress.c_str());
	address.port = m_Port;

	peer = enet_host_connect(m_Client, &address, Channels, 0 /* to send at connect */);
	if (peer == NULL)
	{
		std::cout << "No available peers for initiating an ENet connection.\n";
		*success = false;
		return false;
	}

	NetworkClient* connection = new NetworkClient(peer);

	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(m_Client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		std::cout << "Connection to " << m_Adress << " succeeded \n";

		m_Connection = peer;

		*success = true;
		return true;
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset(peer);
		std::cout << "Connection to " << m_Adress << " failed \n";

		*success = false;
		return false;
	}

	*success = true;
}

void NetworkThread::_SendThread(const char* data, size_t length)
{
	ENetPacket* packet = enet_packet_create(data, length, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(m_Connection, 0, packet);
}

ENetPacket* NetworkThread::PullPackets()
{
	ENetEvent event;

	// The incoming messages are handled by this function. Needs to be called regularly. A 0 timeout means that it's non-blocking
	int eventStatus = enet_host_service(m_Client, &event, 1000);

	if (eventStatus > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
		{
			json packetJson = json::parse((const char*)event.packet->data);

			std::cout << "Recieved message " << packetJson["Type"] << "\n";

			if (packetJson["Type"] == "JoinWorld")
				HandleJoinWorldPacket(packetJson);
			if (packetJson["Type"] == "ChunkData")
				HandleChunkDataPacket(packetJson);

			enet_packet_destroy(event.packet);
			break;
		}

		case ENET_EVENT_TYPE_DISCONNECT: // unused
			printf("(Client) %s disconnected.\n", event.peer->data);
			// Reset client's information
			event.peer->data = NULL;
			break;
		}
	}
	else
	{
		std::cout << "Err packet pull failed\n";
	}

	// Send queded packets?
	enet_host_flush(m_Client);

	return nullptr;
}

void NetworkThread::HandleJoinWorldPacket(json& packet)
{
	json msg;
	msg["Type"] = "JoinWorld";
	msg["Data"]["SessionName"] = "Minecraft";
	//SendJson(msg);
}

void NetworkThread::HandleChunkDataPacket(json& packet)
{
	glm::ivec2 pos(packet["Data"]["Position"]["X"], packet["Data"]["Position"]["Z"]);

	Chunk* chunk = World::GetChunkAt(pos);
	for (int x = 0; x < Chunk::Width; x++)
	{
		for (int z = 0; z < Chunk::Depth; z++)
		{
			ChunkBlock* block = chunk->GetBlockAt(glm::vec3(x, 0, z));
			block->m_BlockId = packet["Data"]["Blocks"][x + z];
		}
	}

	chunk->SetDirty(true);
	chunk->m_HasGenerated = true;

	chunk->RebuildMeshThreaded(ChunkAction::Priority::Normal);
}

NetworkThread::~NetworkThread()
{
	enet_deinitialize();
	if (m_Thread.joinable())
		m_Thread.join();
}