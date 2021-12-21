#include "ServerNetworkThread.h"

#include "../World/ServerWorld.h"

#include <Common/Chunk/Chunk.h>
#include <Common/Chunk/ChunkBlock.h>
#include <Common/DebugConsole.h>

#include <Common/Net/NetworkConstants.h>

ServerNetworkThread& ServerNetworkThread::Get()
{
	static ServerNetworkThread instance;
	return instance;
}

bool ServerNetworkThread::StartServer(int port)
{
	m_Port = port;

	if (enet_initialize() != 0)
	{
		std::cout << "An error occurred while initializing ENet.\n";
		return false;
	}

	ENetAddress address;

	address.host = ENET_HOST_ANY; // This allows connections from anywhere
	address.port = m_Port;

	m_EnetClient = enet_host_create(&address, MaxClients, Channels, MaxInBandwidth, MaxOutBandwidth);

	if (m_EnetClient == NULL)
	{
		std::cout << "An error occurred while trying to create an ENet server host\n";
		return false;
	}

	std::cout << "Server running on port " << m_Port << "\n";

	SetupThread();

	return true;
}

ServerNetworkThread::ServerNetworkThread()
{
}

void ServerNetworkThread::HandlePacket(json& packet, NetworkClient* client, ENetPeer* peer)
{
	Console::Log("Recieved Packets", packet["Type"]) << packet["Data"];

	if (packet["Type"] == "JoinWorld")
	{
		OnClientJoinWorld(packet, client);
	}
	else if (packet["Type"] == "ConfirmJoinWorld")
	{
		OnClientConfirmJoinWorld(packet, client);
	}
	else if (packet["Type"] == "PlayerPosition")
	{
		OnClientPositionUpdate(packet, client);
	}
	else 
	{
		Console::Log("Recieved Packets", "Unhandled Packet") << packet["Data"];
	}
}

void ServerNetworkThread::OnClientJoinWorld(json& packet, NetworkClient* client)
{
	std::string sessionName = packet["Data"]["SessionName"];

	if (client->m_HasJoinedSession)
	{
		std::cout << "Error: Client " << client->m_Id << " has already joined another session\n";
		return;
	}

	client->m_Id = rand() % 65535;
	client->m_SessionName = sessionName;

	// Create the session if it doesn't exist
	if (m_Sessions.count(sessionName) == 0)
	{
		m_Sessions[sessionName] = NetworkSession(sessionName);
		std::cout << "Created session " << sessionName << "\n";

		// Create the world
		m_Sessions[sessionName].m_World = (IWorld*)new ServerWorld();
	}

	NetworkSession& session = m_Sessions[sessionName];

	// Add the player to the world
	ServerPlayer* player = new ServerPlayer();
	player->m_NetClient = client;
	session.m_World->AddPlayer((IPlayer*)player);

	client->m_HasJoinedSession = true;

	std::cout << "Client " << client->m_Id << " joined world " << sessionName << ". ";
	std::cout << session.m_World->GetPlayers().size() << " players are connected\n";

	//// Serialize the existing chunks

	// Send back a join packet
	json responsePacket;
	responsePacket["Type"] = "JoinWorld";
	responsePacket["Data"]["SessionName"] = sessionName;
	responsePacket["Data"]["ClientId"] = client->m_Id;

	// Iterate all players
	std::vector<int> otherClients;
	for (auto& entry : session.m_World->GetPlayers())
	{
		int id = entry.second->m_NetClient->m_Id;

		if (id != client->m_Id)
			otherClients.push_back(id);
	}

	responsePacket["Data"]["OtherClients"] = otherClients;

	client->SendJson(responsePacket);

	// Broadcast to all other clients
	json broadcastPacket;
	broadcastPacket["Type"] = "NewPlayer";
	broadcastPacket["Data"]["ClientId"] = client->m_Id;
	Broadcast(broadcastPacket, sessionName, client->m_Id /* Exclude */);
}

void ServerNetworkThread::OnClientConfirmJoinWorld(json& packet, NetworkClient* client)
{
	ServerPlayer* player = (ServerPlayer*)m_Sessions[client->m_SessionName].m_World->GetPlayer(client->m_Id);

	player->m_ConfirmedJoiningWorld = true;

	Console::Log("World") << "Player " << client->m_Id << " has confirmed joining of world " << client->m_SessionName;
}

void ServerNetworkThread::OnClientPositionUpdate(json& packet, NetworkClient* client)
{
	ServerPlayer* player = (ServerPlayer*)m_Sessions[client->m_SessionName].m_World->GetPlayer(client->m_Id);

	glm::vec3 newPosition(packet["Data"]["X"], packet["Data"]["Y"], packet["Data"]["Z"]);

	player->m_Position = newPosition;

	// Broadcast the position update to the other players
	json packetToBroadcast;
	packetToBroadcast = packet;
	packetToBroadcast["Data"]["ClientId"] = client->m_Id;
	Broadcast(packetToBroadcast, client->m_SessionName, client->m_Id /* Exclude */);
}
