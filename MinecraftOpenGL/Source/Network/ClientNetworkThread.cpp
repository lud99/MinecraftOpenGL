#include "ClientNetworkThread.h"

#include "../World/ClientWorld.h"
#include "../World/Chunk/ChunkMesh.h"
#include <Common/Chunk/Chunk.h>
#include <Common/Chunk/ChunkBlock.h>

#include <Common/NetworkConstants.h>

ClientNetworkThread& ClientNetworkThread::Instance()
{
	static ClientNetworkThread instance;
	return instance;
}


bool ClientNetworkThread::Connect(const std::string& adress, int port)
{
	m_Adress = adress;
	m_Port = port;

	enet_initialize();

	m_Client = enet_host_create(NULL, MaxClients, Channels, MaxInBandwidth, MaxOutBandwidth);

	if (m_Client == NULL) std::cout << "An error occurred while trying to create an ENet client host.\n";

	ENetAddress address;
	ENetEvent event;

	enet_address_set_host(&address, m_Adress.c_str());
	address.port = m_Port;

	ENetPeer* connection = enet_host_connect(m_Client, &address, Channels, 0 /* to send at connect */);
	if (connection == NULL)
	{
		std::cout << "No available peers for initiating an ENet connection.\n";
		return false;
	}

	m_Conn = new NetworkConnection(connection);

	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(m_Client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		std::cout << "Connection to " << m_Adress << " succeeded \n";
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


void ClientNetworkThread::HandlePacket(json& packet, NetworkConnection* conn)
{
}

void ClientNetworkThread::OnJoinWorld(json& packet, NetworkConnection* conn)
{
}

void ClientNetworkThread::OnChunkData(json& packet, NetworkConnection* conn)
{
	glm::ivec2 pos(packet["Data"]["Position"]["X"], packet["Data"]["Position"]["Z"]);

	Chunk* chunk = m_LocalWorld->GetChunkAt(pos);
	for (int i = 0; i < Chunk::BlockCount; i++)
	{
		glm::u8vec3 position = Utils::BlockIndexToPosition(i);
		ChunkBlock* block = chunk->GetBlockAt(position);
		block->m_BlockId = packet["Data"]["Blocks"][i];
	}

	chunk->SetDirty(true);
	chunk->m_HasGenerated = true;

	chunk->m_ChunkMesh->RebuildMeshThreaded(ChunkAction::Priority::Normal);
}
