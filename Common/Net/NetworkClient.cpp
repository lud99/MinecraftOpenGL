#include "NetworkClient.h"

#include "INetworkThread.h"

#include <iostream>

#include "NetworkMessages.h"

#ifdef SERVER_BUILD
#include "../../Net/ServerNetworkThread.h"
#else
#include "../../../Network/ClientNetworkThread.h"
#endif

NetworkClient::NetworkClient()
{
}

NetworkClient::NetworkClient(ENetPeer* peer, int id)
{
	m_Peer = peer;
	m_Id = id;
}

void NetworkClient::SendJson(json& message)
{
	NetworkThread::Get().SendJson(message, this);
}

void NetworkClient::SendString(const std::string& data)
{
	NetworkThread::Get().SendString(data, this);
}

NetworkClient::~NetworkClient()
{

}