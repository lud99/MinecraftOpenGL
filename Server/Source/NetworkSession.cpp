#include "NetworkSession.h"
#include <Common/NetworkClient.h>

NetworkSession::NetworkSession(std::string sessionId)
{
	m_SessionId = sessionId;
}

NetworkClient* NetworkSession::CreateClient(ENetPeer* connection, int id)
{
	NetworkClient* client = new NetworkClient(connection, id);
	m_Clients[id] = client;

	return client;
}

void NetworkSession::AddClient(NetworkClient* client)
{
	m_Clients[client->m_Id] = client;
}

NetworkClient* NetworkSession::GetClient(int id)
{
	return m_Clients[id];
}

void NetworkSession::RemoveClient(NetworkClient* client)
{

}

NetworkSession::~NetworkSession()
{

}