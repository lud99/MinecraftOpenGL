#include "ServerNetworkThread.h"

#include "../World/ServerWorld.h"

#include <Common/Chunk/Chunk.h>
#include <Common/Chunk/ChunkBlock.h>

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

void ServerNetworkThread::HandlePacket(json& packet, NetworkClient* client)
{
	if (packet["Type"] == "JoinWorld")
	{
		OnClientJoinWorld(packet, client);
	}
	else {
		std::cout << "Other packet: " << packet["Type"] << "\n";
	}
}

void ServerNetworkThread::OnClientJoinWorld(json& packet, NetworkClient* client)
{
	std::string sessionName = packet["Data"]["SessionName"];

	// The data is null if it is the first time the client connected
	if (!client->m_HasJoinedSession)
	{
		int clientId = 0;// rand() % 65535;
		client->m_Id = clientId;
	}

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

	// Send back a join packet
	json responsePacket;
	responsePacket["Type"] = "JoinWorld";
	responsePacket["Data"]["SessionName"] = sessionName;
	responsePacket["Data"]["ClientId"] = client->m_Id;
	client->SendJson(responsePacket);
}