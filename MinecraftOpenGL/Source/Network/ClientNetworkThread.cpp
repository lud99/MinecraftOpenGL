#include "ClientNetworkThread.h"

#include "../World/ClientWorld.h"
#include "../World/Chunk/ChunkMesh.h"
#include "../World/Player/ClientPlayer.h"
#include <Common/Chunk/Chunk.h>
#include <Common/Chunk/ChunkBlock.h>

#include <Common/Net/NetworkConstants.h>

ClientNetworkThread& ClientNetworkThread::Get()
{
	static ClientNetworkThread instance;
	return instance;
}

ClientNetworkThread::ClientNetworkThread()
{
}

bool ClientNetworkThread::Connect(const std::string& adress, int port)
{
	m_Adress = adress;
	m_Port = port;

	if (enet_initialize() != 0)
	{
		std::cout << "An error occurred while initializing ENet.\n";
		return false;
	}

	m_EnetClient = enet_host_create(NULL, MaxClients, Channels, MaxInBandwidth, MaxOutBandwidth);

	if (m_EnetClient == NULL)
	{
		std::cout << "An error occurred while trying to create an ENet client host\n";
		return false;
	}

	ENetAddress address;
	ENetEvent event;

	enet_address_set_host(&address, m_Adress.c_str());
	address.port = m_Port;

	ENetPeer* connection = enet_host_connect(m_EnetClient, &address, Channels, 0 /* to send at connect */);
	if (connection == NULL)
	{
		std::cout << "No available peers for initiating an ENet connection.\n";
		return false;
	}

	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(m_EnetClient, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		std::cout << "Connection to " << m_Adress << " succeeded \n";
		m_ThisClient = new NetworkClient(connection);
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset(connection);
		std::cout << "Connection to " << m_Adress << " failed \n";

		return false;
	}

	SetupThread();

	return true;
}


void ClientNetworkThread::HandlePacket(json& packet, NetworkClient* me)
{
	me = m_ThisClient;
	if (packet["Type"] == "JoinWorld")
	{
		OnJoinWorld(packet, me);
	}
	else if (packet["Type"] == "ChunkData")
	{
		OnChunkData(packet, me);
	}
	else 
	{
		std::cout << "Other packet: " << packet["Type"] << "\n";
	}
}

void ClientNetworkThread::OnJoinWorld(json& packet, NetworkClient* me)
{
	if (me->m_HasJoinedSession)
	{
		std::cout << "Client joining world, but is already in session: " << me->m_SessionName << "\n";
		return;
	}

	std::string sessionName = packet["Data"]["SessionName"];

	me->m_Id = packet["Data"]["ClientId"];
	me->m_SessionName = sessionName;

	// Create the session if it doesn't exist
	if (m_Sessions.count(sessionName) == 0)
	{
		m_Sessions[sessionName] = NetworkSession(sessionName);
	}

	NetworkSession& session = m_Sessions[sessionName];

	// Create the world
	session.m_World = (IWorld*)new ClientWorld();

	me->m_HasJoinedSession = true;

	std::cout << "Joining session " << sessionName << "\n";

	// Add a player
	ClientPlayer* player = new ClientPlayer();
	player->m_NetClient = me;
	player->m_Id = me->m_Id;
	player->m_World = session.m_World;
	session.m_World->AddPlayer((IPlayer*)player);

	GetThisWorld()->m_LocalPlayer = player;

	std::cout << "Me (" << me->m_Id << ") joined world " << sessionName << ". ";
	std::cout << session.m_World->GetPlayers().size() << " players are connected\n";
}

void ClientNetworkThread::OnChunkData(json& packet, NetworkClient* conn)
{
	glm::ivec2 pos(packet["Data"]["Position"]["X"], packet["Data"]["Position"]["Z"]);

	ClientWorld* world = GetThisWorld();
	assert(world);

	Chunk* chunk = world->GetChunkAt(pos);

	// Create the chunk if this is new terrain
	if (!chunk)
		chunk = world->CreateChunk(pos, (IWorld*)world);
		
	for (int i = 0; i < Chunk::BlockCount; i++)
	{
		glm::u8vec3 position = Utils::BlockIndexToPosition(i);
		ChunkBlock* block = chunk->GetBlockAt(position);
		block->m_BlockId = packet["Data"]["Blocks"][i];
	}

	chunk->SetDirty(true);
	chunk->m_HasGenerated = true;

	//chunk->m_ChunkMesh->RebuildMeshThreaded(ChunkAction::Priority::Normal);
}

NetworkSession* ClientNetworkThread::GetThisSession()
{
	if (m_Sessions.count(m_ThisClient->m_SessionName) == 0)
		return nullptr;

	return &m_Sessions[m_ThisClient->m_SessionName];
}

ClientWorld* ClientNetworkThread::GetThisWorld()
{
	NetworkSession* session = GetThisSession();

	if (!session) return nullptr;

	return (ClientWorld*)session->m_World;
}

ClientPlayer* ClientNetworkThread::GetThisPlayer()
{
	NetworkSession* session = GetThisSession();

	if (!session) return nullptr;

	return (ClientPlayer*)session->m_World->GetPlayer(m_ThisClient->m_Id);
}