#include "NetworkSession.h"
#include <Common/NetworkConnection.h>

NetworkSession::NetworkSession(std::string sessionId)
{
	m_SessionId = sessionId;
}

NetworkConnection* NetworkSession::CreateClient(ENetPeer* connection, int id)
{
	NetworkConnection* client = new NetworkConnection(connection, id);
	m_Clients[id] = client;

	return client;
}

void NetworkSession::AddClient(NetworkConnection* client)
{
	m_Clients[client->m_Id] = client;
}

NetworkConnection* NetworkSession::GetClient(int id)
{
	return m_Clients[id];
}

void NetworkSession::RemoveClient(NetworkConnection* client)
{

}

NetworkSession::~NetworkSession()
{

}